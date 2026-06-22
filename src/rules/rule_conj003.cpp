/*!
* \file rule_conj003.cpp
* \brief Реализация правила CONJ-003: although ↔ despite.
*
* Ветка (а): although → despite. A с LEMMA=although, UPOS=SCONJ,
* DEPREL=mark; HEAD(A)=H; H.UPOS ∈ {NOUN, PROPN}, у H нет прямых зависимых
* с UPOS ∈ {VERB, AUX} → despite. Не срабатывает если H.DEPREL=advcl
* (эллиптическая клауза: «Although tired, he continued» — корректно).
* Ветка (б): despite → although. D с LEMMA=despite, UPOS=ADP, DEPREL=case;
* HEAD(D)=H; у H есть прямой зависимый с DEPREL ∈ {nsubj, cop, aux, aux:pass}
* (признак клаузы) → although. Аналогично для «in spite of» (LEMMA=in +
* зависимые spite, of через DEPREL=fixed).
*/

#include "rule_conj003.h"
#include <QSet>

QString Rule_CONJ003::ruleId() const
{
    return QStringLiteral("CONJ-003");
}

QSet<Upos> Rule_CONJ003::anchorUpos() const
{
    // although — SCONJ; despite — ADP; in (in spite of) — ADP
    return {Upos::SCONJ, Upos::ADP};
}

bool Rule_CONJ003::canConflict() const
{
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
* \brief Проверить, есть ли у токена прямой зависимый с признаком клаузы.
* \param [in] node Токен для проверки.
* \return true если найден зависимый с DEPREL ∈ {nsubj, cop, aux, aux:pass}.
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

/*!
* \brief Собрать цепочку fixed-зависимых и проверить LEMMA.
* \param [in] node Токен для проверки.
* \param [in] expectedLemmas Ожидаемые леммы в цепочке (нижний регистр).
* \return true если все expectedLemmas найдены среди fixed-зависимых.
*
* Для "in spite of": у in должны быть fixed-зависимые со LEMMA=spite и of.
*/
bool hasFixedChain(const TokenNode& node, const QSet<QString>& expectedLemmas)
{
    QSet<QString> found;
    for (const TokenNode* child : node.children)
        if (child->deprel == Deprel::Fixed)
            found.insert(child->lemma.toLower());
    for (const QString& expected : expectedLemmas)
        if (!found.contains(expected))
            return false;
    return true;
}

} // namespace

QSet<CandidateError> Rule_CONJ003::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    const QString lemmaLower = anchor.lemma.toLower();

    // Ветка (а): although → despite
    if (anchor.upos == Upos::SCONJ && lemmaLower == QStringLiteral("although")) {
        // DEPREL должен быть mark
        if (anchor.deprel != Deprel::Mark)
            return res;

        // HEAD(A) = H
        if (!anchor.parent)
            return res;
        const TokenNode& h = *anchor.parent;

        // H должен быть именной группой: NOUN или PROPN
        if (h.upos != Upos::NOUN && h.upos != Upos::PROPN)
            return res;

        // Не срабатывает если H.DEPREL=advcl (эллиптическая клауза)
        if (h.deprel == Deprel::Advcl)
            return res;

        // У H не должно быть прямых зависимых VERB или AUX
        if (hasVerbOrAuxChild(h))
            return res;

        // Найдена ошибка: although перед именной группой вместо despite
        CandidateError ce;
        ce.ruleId = QStringLiteral("CONJ-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    // Ветка (б): despite → although (и in spite of → although)
    if (anchor.upos == Upos::ADP &&
        (lemmaLower == QStringLiteral("despite") || lemmaLower == QStringLiteral("in"))) {

        // Для despite: DEPREL=case
        // Для in (in spite of): DEPREL=case + fixed-цепочка spite, of
        if (anchor.deprel != Deprel::Case)
            return res;

        // Для "in" проверяем наличие fixed-цепочки spite + of
        if (lemmaLower == QStringLiteral("in")) {
            const QSet<QString> expected = {QStringLiteral("spite"), QStringLiteral("of")};
            if (!hasFixedChain(anchor, expected))
                return res;
        }

        // HEAD(D) = H
        if (!anchor.parent)
            return res;
        const TokenNode& h = *anchor.parent;

        // У H должен быть прямой зависимый с признаком клаузы
        if (!hasClauseMarkerChild(h))
            return res;

        // Найдена ошибка: despite/in spite of перед клаузой вместо although
        CandidateError ce;
        ce.ruleId = QStringLiteral("CONJ-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    return res;
}