/*!
* \file rule_prep003.cpp
* \brief Реализация правила PREP-003: while ↔ during.
*
* Ветка (а): while → during. W с LEMMA=while, UPOS=SCONJ; HEAD(W)=H;
* H.UPOS ∈ {NOUN, PROPN}, у H нет прямых зависимых с UPOS ∈ {VERB, AUX}
* (признак именной группы, а не клаузы) → during.
* Ветка (б): during → while. D с LEMMA=during, UPOS=ADP, DEPREL=case;
* HEAD(D)=H; у H есть прямой зависимый с DEPREL ∈ {nsubj, cop, aux, aux:pass}
* (признак клаузы, а не именной группы) → while.
*/

#include "rule_prep003.h"
#include <QSet>

QString Rule_PREP003::ruleId() const
{
    return QStringLiteral("PREP-003");
}

QSet<Upos> Rule_PREP003::anchorUpos() const
{
    // While — SCONJ, during — ADP
    return {Upos::SCONJ, Upos::ADP};
}

bool Rule_PREP003::canConflict() const
{
    // Один и тот же маркер может быть ошибочным для нескольких правил
    return true;
}

namespace {

/*!
* \brief Проверить, есть ли у токена прямой зависимый с UPOS ∈ {VERB, AUX}.
* \param [in] node Токен для проверки.
* \return true если найден зависимый VERB или AUX.
*
* Наличие VERB/AUX в прямых зависимых указывает на клаузу, а не на именную
* группу. Ветка (а) не срабатывает, если HEAD — клауза.
*/
bool hasVerbOrAuxChild(const TokenNode& node)
{
    for (const TokenNode* child : node.children)
        if (child->upos == Upos::VERB || child->upos == Upos::AUX)
            return true;
    return false;
}

/*!
* \brief Проверить, есть ли у токена прямой зависимый с DEPREL клаузы.
* \param [in] node Токен для проверки.
* \return true если найден зависимый с признаком клаузы.
*
* Признаки клаузы: nsubj, cop, aux, aux:pass. Их наличие у HEAD предлога
* during указывает, что перед нами клауза, а не именная группа.
*/
bool hasClauseMarkerChild(const TokenNode& node)
{
    for (const TokenNode* child : node.children)
        if (child->deprel == Deprel::Nsubj ||
            child->deprel == Deprel::Cop ||
            child->deprel == Deprel::Aux ||
            child->deprel == Deprel::AuxPass)
            return true;
    return false;
}

} // namespace

QSet<CandidateError> Rule_PREP003::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    const QString lemmaLower = anchor.lemma.toLower();

    // Ветка (а): while → during
    if (anchor.upos == Upos::SCONJ && lemmaLower == QStringLiteral("while")) {
        if (!anchor.parent)
            return res;

        const TokenNode& h = *anchor.parent;

        // HEAD должен быть именной группой: NOUN или PROPN
        if (h.upos != Upos::NOUN && h.upos != Upos::PROPN)
            return res;

        // У HEAD не должно быть прямых зависимых VERB или AUX
        // (иначе это клауза, а не именная группа)
        if (hasVerbOrAuxChild(h))
            return res;

        // Найдена ошибка: while перед именной группой вместо during
        CandidateError ce;
        ce.ruleId = QStringLiteral("PREP-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    // Ветка (б): during → while
    if (anchor.upos == Upos::ADP && lemmaLower == QStringLiteral("during")) {
        // DEPREL должен быть case
        if (anchor.deprel != Deprel::Case)
            return res;

        if (!anchor.parent)
            return res;

        const TokenNode& h = *anchor.parent;

        // У HEAD должен быть прямой зависимый с признаком клаузы
        if (!hasClauseMarkerChild(h))
            return res;

        // Найдена ошибка: during перед клаузой вместо while
        CandidateError ce;
        ce.ruleId = QStringLiteral("PREP-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    return res;
}