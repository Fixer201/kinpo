/*!
* \file rule_prep004.cpp
* \brief Реализация правила PREP-004: Perfect + ago.
*
* Срабатывает на ADV с LEMMA=ago, если в предложении есть AUX с LEMMA=have,
* DEPREL=aux к VERB с VerbForm=Part (конструкция Perfect). Ago несовместимо
* с Perfect, исправление — Past Simple. Приоритет над AUX-002: если оба
* сработали на одном VERB, выводится только PREP-004.
*/

#include "rule_prep004.h"
#include <QSet>

const Rule_PREP004& Rule_PREP004::instance()
{
    static Rule_PREP004 inst;
    return inst;
}

QString Rule_PREP004::ruleId() const
{
    return QStringLiteral("PREP-004");
}

QSet<Upos> Rule_PREP004::anchorUpos() const
{
    return {Upos::ADV};
}

bool Rule_PREP004::canConflict() const
{
    // Конфликтует с AUX-002 на одном VERB — приоритет за PREP-004
    return true;
}

namespace {

/*!
* \brief Найти корень предложения, поднимаясь от anchor к родителям.
* \param [in] anchor Стартовый токен.
* \return Корневой токен (без parent).
*/
const TokenNode* findSentenceRoot(const TokenNode& anchor)
{
    const TokenNode* node = &anchor;
    while (node->parent)
        node = node->parent;
    return node;
}

/*!
* \brief Найти конструкцию Perfect: AUX have → VERB с VerbForm=Part.
* \param [in] root Корневой токен предложения.
* \param [out] auxFound Найденный AUX (заполняется если найден).
* \param [out] verbFound Найденный VERB (заполняется если найден).
* \return true если найдена конструкция Perfect.
*
* Обходит дерево от root, ищет AUX с LEMMA=have и DEPREL=aux, у которого
* HEAD — VERB с VerbForm=Part.
*/
bool findPerfectAuxAndVerb(const TokenNode& root,
                           const TokenNode*& auxFound,
                           const TokenNode*& verbFound)
{
    for (const TokenNode* child : root.children) {
        if (child->upos == Upos::AUX &&
            child->lemma.toLower() == QStringLiteral("have") &&
            child->deprel == Deprel::Aux &&
            child->parent &&
            child->parent->upos == Upos::VERB &&
            child->parent->features.verbForm.has_value() &&
            *child->parent->features.verbForm == VerbFormValue::Part) {
            auxFound = child;
            verbFound = child->parent;
            return true;
        }
        if (findPerfectAuxAndVerb(*child, auxFound, verbFound))
            return true;
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_PREP004::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Якорь — ADV с LEMMA=ago
    if (anchor.upos != Upos::ADV)
        return res;
    if (anchor.lemma.toLower() != QStringLiteral("ago"))
        return res;

    // Ищем конструкцию Perfect: AUX have → VERB с VerbForm=Part
    const TokenNode* root = findSentenceRoot(anchor);
    const TokenNode* auxFound = nullptr;
    const TokenNode* verbFound = nullptr;
    if (!findPerfectAuxAndVerb(*root, auxFound, verbFound))
        return res;

    // Найдена ошибка: ago несовместимо с Perfect
    // displayTokenIds — AUX + VERB (читаемый фрагмент "has went")
    // conflictTokenIds — только VERB (зона конфликта с AUX-002)
    CandidateError ce;
    ce.ruleId = QStringLiteral("PREP-004");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {auxFound->id, verbFound->id};
    ce.conflictTokenIds = {verbFound->id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::ReplaceTokens;
        edit.targetTokenIds = {verbFound->id};
        // Past Simple из past_forms.txt; fallback — лемма + "(Past Simple)"
        const QString verbLemma = verbFound->lemma.toLower();
        auto pfIt = runtime.resources.pastForms.find(verbLemma);
        if (pfIt != runtime.resources.pastForms.end() && !pfIt->pastSimple.isEmpty())
            edit.newTokens.append(pfIt->pastSimple);
        else
            edit.newTokens.append(verbLemma + QStringLiteral(" (Past Simple)"));
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("«ago» не используется с Present Perfect. Замените на Simple Past.");
    res.insert(ce);
    return res;
}