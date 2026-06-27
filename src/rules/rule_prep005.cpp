/*!
* \file rule_prep005.cpp
* \brief Реализация правила PREP-005: Неверный предлог при глаголе.
*
* Срабатывает на VERB, LEMMA которого есть в verb_prep.txt. Три типа ошибок:
*  — DeletePrep: у V есть obl-зависимый с case-зависимым LEMMA == wrongPrep.
*    Предлог лишний, удалить.
*  — ReplacePrep: у V есть obl-зависимый с case-зависимым LEMMA == wrongPrep.
*    Предлог неверный, заменить.
*  — InsertPrep: у V есть obj-зависимый без case-зависимого. Предлог пропущен,
*    вставить.
* displayTokenIds: DeletePrep/ReplacePrep — [case.id]; InsertPrep — [V.id, obj.id].
* conflictTokenIds: DeletePrep/ReplacePrep — {case.id}; InsertPrep — {obj.id}.
*/

#include "rule_prep005.h"
#include <QSet>

const Rule_PREP005& Rule_PREP005::instance()
{
    static Rule_PREP005 inst;
    return inst;
}

QString Rule_PREP005::ruleId() const
{
    return QStringLiteral("PREP-005");
}

QSet<Upos> Rule_PREP005::anchorUpos() const
{
    return {Upos::Verb};
}

bool Rule_PREP005::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Найти obl-зависимого V с case-зависимым заданной LEMMA.
* \param [in] verb Токен глагола.
* \param [in] wrongPrepLemma Лемма ошибочного предлога (нижний регистр).
* \param [out] caseNode Найденный case-токен (заполняется если найден).
* \return true если найден obl с case == wrongPrepLemma.
*
* Обходит прямых зависимых verb, ищет obl с дочерним case (ADP, deprel=Case),
* лемма которого совпадает с wrongPrepLemma.
*/
bool findOblWithCaseLemma(const TokenNode& verb,
                          const QString& wrongPrepLemma,
                          const TokenNode*& caseNode)
{
    bool found = false;
    for (const TokenNode* child : verb.children) {
        if (child->deprel == Deprel::Obl && !found)
            for (const TokenNode* grandchild : child->children)
                if (grandchild->deprel == Deprel::Case &&
                    grandchild->lemma.toLower() == wrongPrepLemma) {
                    caseNode = grandchild;
                    found = true;
                }
    }
    return found;
}

/*!
* \brief Найти obj-зависимого V без case-зависимого (пропущен предлог).
* \param [in] verb Токен глагола.
* \param [out] objNode Найденный obj-токен (заполняется если найден).
* \return true если найден obj без дочернего case.
*
* Обходит прямых зависимых verb, ищет obj без case-зависимого. Наличие
* case у obj означает, что предлог уже стоит, вставлять не нужно.
*/
bool findObjWithoutCase(const TokenNode& verb, const TokenNode*& objNode)
{
    bool found = false;
    for (const TokenNode* child : verb.children) {
        if (child->deprel == Deprel::Obj && !found) {
            // Проверяем, есть ли у obj дочерний case
            bool hasCase = false;
            for (const TokenNode* grandchild : child->children)
                if (grandchild->deprel == Deprel::Case)
                    hasCase = true;
            if (!hasCase) {
                objNode = child;
                found = true;
            }
        }
    }
    return found;
}

} // namespace

QSet<CandidateError> Rule_PREP005::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Якорь — VERB
    if (anchor.upos != Upos::Verb)
        return res;

    // LEMMA должна быть в verb_prep.txt
    const QString lemmaLower = anchor.lemma.toLower();
    auto it = runtime.resources.verbPrep.find(lemmaLower);
    if (it == runtime.resources.verbPrep.end())
        return res;

    // Перебираем все записи для этого глагола
    for (const VerbPrepEntry& entry : it.value()) {
        if (entry.action == VerbPrepAction::DeletePrep ||
            entry.action == VerbPrepAction::ReplacePrep) {
            // Лишний или неверный предлог: ищем obl с case == wrongPrep
            if (!entry.wrongPrep.has_value())
                continue;
            const QString wrongPrepLower = entry.wrongPrep->toLower();

            const TokenNode* caseNode = nullptr;
            if (findOblWithCaseLemma(anchor, wrongPrepLower, caseNode)) {
                CandidateError ce;
                ce.ruleId = QStringLiteral("PREP-005");
                ce.sentId = QStringLiteral("test");
                ce.displayTokenIds = {caseNode->id};
                ce.conflictTokenIds = {caseNode->id};
                if (entry.action == VerbPrepAction::DeletePrep) {
                    AtomicEdit edit;
                    edit.type = AtomicEditType::DeleteTokens;
                    edit.targetTokenIds = {caseNode->id};
                    ce.edits.append(edit);
                    ce.description = QStringLiteral("Глагол «%1» употребляется без предлога.")
                                         .arg(anchor.lemma);
                } else {
                    AtomicEdit edit;
                    edit.type = AtomicEditType::ReplaceTokens;
                    edit.targetTokenIds = {caseNode->id};
                    edit.newTokens.append(entry.prep.value_or(QString()));
                    ce.edits.append(edit);
                    ce.description = QStringLiteral("Глагол «%1» требует предлога «%2».")
                                         .arg(anchor.lemma).arg(entry.prep.value_or(QString()));
                }
                res.insert(ce);
            }
        } else if (entry.action == VerbPrepAction::InsertPrep) {
            // Пропущенный предлог: ищем obj без case
            const TokenNode* objNode = nullptr;
            if (findObjWithoutCase(anchor, objNode)) {
                CandidateError ce;
                ce.ruleId = QStringLiteral("PREP-005");
                ce.sentId = QStringLiteral("test");
                ce.displayTokenIds = {anchor.id, objNode->id};
                ce.conflictTokenIds = {objNode->id};
                {
                    AtomicEdit edit;
                    edit.type = AtomicEditType::InsertBefore;
                    edit.referenceTokenId = objNode->id;
                    edit.newTokens.append(entry.prep.value_or(QString()));
                    ce.edits.append(edit);
                }
                ce.description = QStringLiteral("Глагол «%1» требует предлога «%2».")
                                     .arg(anchor.lemma).arg(entry.prep.value_or(QString()));
                res.insert(ce);
            }
        }
    }

    return res;
}