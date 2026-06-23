/*!
* \file checkersystem.cpp
* \brief Реализация системы грамматической проверки.
*
* Содержит resolveCandidate и checkSentence.
*/

#include "checkersystem.h"
#include <QSet>

namespace {

/*!
* \brief Проверяет, выполняется ли условие приоритета для конкретного кандидата.
*
* Для PriorityConditionKind::Always всегда возвращает true.
* Для Art003LanguageCase проверяет структуру предложения:
*  — находит токен PROPN с lemma="English" среди conflictTokenIds;
*  — если у этого токена есть child с lemma="language", условие не выполняется;
*  — иначе выполняется.
*/
bool conditionApplies(PriorityConditionKind kind,
                      const CandidateError& higher,
                      const SentenceModel& sentence)
{
    if (kind == PriorityConditionKind::Always)
        return true;

    if (kind == PriorityConditionKind::Art003LanguageCase) {
        for (int tid : higher.conflictTokenIds) {
            TokenNode* node = sentence.tokensById.value(tid, nullptr);
            if (!node)
                continue;
            if (node->upos != Upos::PROPN ||
                (node->lemma != QStringLiteral("English") && node->form != QStringLiteral("English")))
                continue;
            for (TokenNode* child : node->children) {
                if (child->form == QStringLiteral("language") || child->lemma == QStringLiteral("language"))
                    return false;
            }
            return true;
        }
        return false;
    }

    return false;
}

} // namespace

/*!
* \brief Разрешение конфликтов приоритетов между кандидатами.
*
* Инкрементально добавляет кандидата в зону.
* Приоритеты между правилами обрабатываются на основе CheckerRuntime::priorityIndex.
* Условные приоритеты (Art003LanguageCase) проверяются через структуру предложения.
*/
void resolveCandidate(CandidateError candidate,
                      ConflictZoneMap& zoneMap,
                      const SentenceModel& sentence,
                      const CheckerRuntime& runtime)
{
    QSet<CandidateError>& zone = zoneMap.zones[candidate.conflictTokenIds];
    if (zone.isEmpty()) {
        zone.insert(candidate);
        return;
    }

    bool newSuppressed = false;
    QSet<CandidateError> toRemove;

    for (const CandidateError& existing : zone) {
        // Проверяем: существующий кандидат подавляет нового?
        auto higherIt = runtime.priorityIndex.conditionsByHigherRule.find(existing.ruleId);
        if (higherIt != runtime.priorityIndex.conditionsByHigherRule.end()) {
            auto condIt = higherIt->find(candidate.ruleId);
            if (condIt != higherIt->end()) {
                if (conditionApplies(condIt.value(), existing, sentence)) {
                    newSuppressed = true;
                    break;
                }
            }
        }

        // Проверяем: новый кандидат подавляет существующего?
        auto higherIt2 = runtime.priorityIndex.conditionsByHigherRule.find(candidate.ruleId);
        if (higherIt2 != runtime.priorityIndex.conditionsByHigherRule.end()) {
            auto condIt2 = higherIt2->find(existing.ruleId);
            if (condIt2 != higherIt2->end()) {
                if (conditionApplies(condIt2.value(), candidate, sentence)) {
                    toRemove.insert(existing);
                }
            }
        }
    }

    if (newSuppressed) {
        return;
    }

    for (const CandidateError& rem : toRemove) {
        zone.remove(rem);
    }
    zone.insert(candidate);
}

/*!
* \brief Проверка предложения всеми правилами.
*
* Диспетчеризует правила по UPOS токенов, собирает кандидатов
* и разрешает конфликты через resolveCandidate.
*/
QSet<CandidateError> checkSentence(const SentenceModel& sentence,
                                   int sentenceIndex,
                                   const DocumentModel& document,
                                   const CheckerRuntime& runtime)
{
    ConflictZoneMap zoneMap;
    QSet<CandidateError> nonConflict;

    // Вызываем все правила, зарегистрированные для UPOS каждого токена
    for (const TokenNode* token : sentence.tokens) {
        const QSet<const Rule*> rules = runtime.dispatch.value(token->upos);
        for (const Rule* rule : rules) {
            if (!rule)
                continue;
            QSet<CandidateError> candidates = rule->check(*token, sentenceIndex, document, runtime);
            for (const CandidateError& ce : candidates) {
                // Кандидаты от правил с canConflict проходят через resolveCandidate
                // для подавления менее приоритетных в той же зоне
                if (rule->canConflict()) {
                    resolveCandidate(ce, zoneMap, sentence, runtime);
                } else {
                    // Кандидаты от правил без canConflict добавляются напрямую
                    nonConflict.insert(ce);
                }
            }
        }
    }

    // Собираем все неподавленные кандидаты из зон конфликтов
    QSet<CandidateError> result = nonConflict;
    for (auto it = zoneMap.zones.begin(); it != zoneMap.zones.end(); ++it) {
        for (const CandidateError& ce : it.value()) {
            result.insert(ce);
        }
    }
    return result;
}

// Цикл проверки всех предложений документа. Для каждого предложения
// вызывает checkSentence и накапливает найденные ошибки в общий набор.
QSet<CandidateError> runAnalysis(const DocumentModel& document,
                                 const CheckerRuntime& runtime)
{
    QSet<CandidateError> allErrors;
    for (size_t i = 0; i < document.sentences.size(); ++i) {
        const SentenceModel& sentence = *document.sentences[i];
        QSet<CandidateError> sentenceErrors = checkSentence(sentence, i, document, runtime);
        for (const CandidateError& ce : sentenceErrors)
            allErrors.insert(ce);
    }
    return allErrors;
}
