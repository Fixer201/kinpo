/*!
* \file rule_conj002.cpp
* \brief Реализация правила CONJ-002: because ↔ because of.
*
* Ветка (а): because → because of. B с LEMMA=because, UPOS=SCONJ,
* DEPREL=mark; HEAD(B)=H; H.UPOS ∈ {NOUN, PROPN}, у H нет прямых зависимых
* с UPOS ∈ {VERB, AUX} (признак именной группы, а не клаузы) → because of.
* Ветка (б): because of → because. B с LEMMA=because; B имеет зависимого
* с LEMMA=of, DEPREL=fixed; HEAD(B)=H; у H есть прямой зависимый с DEPREL
* ∈ {nsubj, cop, aux, aux:pass} (признак клаузы) → because.
* displayTokenIds: ветка (а) — [B.id]; ветка (б) — [B.id, of.id].
* conflictTokenIds: ветка (а) — {B.id}; ветка (б) — {B.id, of.id}.
*/

#include "rule_conj002.h"
#include <QSet>

QString Rule_CONJ002::ruleId() const
{
    return QStringLiteral("CONJ-002");
}

QSet<Upos> Rule_CONJ002::anchorUpos() const
{
    // because — SCONJ; в конструкции "because of" because всё ещё SCONJ
    // (of прикрепляется к because через fixed)
    return {Upos::SCONJ};
}

bool Rule_CONJ002::canConflict() const
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
*
* Признаки клаузы: nsubj, cop, aux, aux:pass. Их наличие у HEAD выражения
* because of указывает, что перед нами клауза, а не именная группа.
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
* \brief Найти зависимого с LEMMA=of, DEPREL=fixed (фиксированное выражение).
* \param [in] node Токен для проверки.
* \param [out] ofNode Найденный of-токен (заполняется если найден).
* \return true если найден fixed-зависимый с LEMMA=of.
*
* Ветка (б) требует, чтобы у because был fixed-зависимый of, образуя
* выражение "because of".
*/
bool findFixedOfChild(const TokenNode& node, const TokenNode*& ofNode)
{
    for (const TokenNode* child : node.children)
        if (child->deprel == Deprel::Fixed &&
            child->lemma.toLower() == QStringLiteral("of")) {
            ofNode = child;
            return true;
        }
    return false;
}

} // namespace

QSet<CandidateError> Rule_CONJ002::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — SCONJ с LEMMA=because
    if (anchor.upos != Upos::SCONJ)
        return res;
    if (anchor.lemma.toLower() != QStringLiteral("because"))
        return res;

    // DEPREL должен быть mark
    if (anchor.deprel != Deprel::Mark)
        return res;

    // HEAD(B) = H
    if (!anchor.parent)
        return res;
    const TokenNode& h = *anchor.parent;

    // Проверяем наличие fixed-зависимого of (форма "because of")
    const TokenNode* ofNode = nullptr;
    const bool hasFixedOf = findFixedOfChild(anchor, ofNode);

    if (!hasFixedOf) {
        // Ветка (а): because → because of (нет fixed-of)
        // H должен быть именной группой: NOUN или PROPN
        if (h.upos != Upos::NOUN && h.upos != Upos::PROPN)
            return res;
        // У H не должно быть прямых зависимых VERB или AUX (иначе клауза)
        if (hasVerbOrAuxChild(h))
            return res;

        // Найдена ошибка: because перед именной группой вместо because of
        CandidateError ce;
        ce.ruleId = QStringLiteral("CONJ-002");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    // Ветка (б): because of → because (есть fixed-of)
    // У H должен быть прямой зависимый с признаком клаузы
    if (!hasClauseMarkerChild(h))
        return res;

    // Найдена ошибка: because of перед клаузой вместо because
    CandidateError ce;
    ce.ruleId = QStringLiteral("CONJ-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, ofNode->id};
    ce.conflictTokenIds = {anchor.id, ofNode->id};
    res.insert(ce);
    return res;
}