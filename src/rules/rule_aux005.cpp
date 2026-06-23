/*!
* \file rule_aux005.cpp
* \brief Реализация правила AUX-005: модальный + причастие, нет или
* искажён have.
*
* Якорь — модальный M (UPOS=AUX, лемма из модального списка, DEPREL=aux).
* V — голова M, V.VerbForm=Part. Если у V нет зависимого have (DEPREL=aux),
* срабатывает правило.
*
* Ветка (а): следующий за M (линейно) токен не of. Вставить have между
* M и V. displayTokenIds=[M.id, V.id], conflictTokenIds={V.id}.
*
* Ветка (б): следующий за M (линейно) токен имеет LEMMA=of, UPOS=ADP.
* Заменить of на have. displayTokenIds=[M.id, of.id, V.id],
* conflictTokenIds={of.id}.
*/

#include "rule_aux005.h"
#include <QSet>

const Rule_AUX005& Rule_AUX005::instance()
{
    static Rule_AUX005 inst;
    return inst;
}

QString Rule_AUX005::ruleId() const
{
    return QStringLiteral("AUX-005");
}

QSet<Upos> Rule_AUX005::anchorUpos() const
{
    return {Upos::AUX};
}

bool Rule_AUX005::canConflict() const
{
    return true;
}

namespace {

// Модальные глаголы. Закрытый грамматический класс.
const QSet<QString> modalVerbs = {
    QStringLiteral("can"), QStringLiteral("could"),
    QStringLiteral("may"), QStringLiteral("might"),
    QStringLiteral("must"), QStringLiteral("shall"),
    QStringLiteral("should"), QStringLiteral("will"),
    QStringLiteral("would")
};

/*!
* \brief Проверить, есть ли у V зависимый have (DEPREL=aux).
* \param [in] v Голова якоря M.
* \return true, если среди прямых зависимых V найден AUX с LEMMA=have
*         и DEPREL=aux.
*/
bool hasHaveChild(const TokenNode& v)
{
    for (const TokenNode* child : v.children) {
        if (child->upos != Upos::AUX)
            continue;
        if (child->deprel != Deprel::Aux)
            continue;
        if (child->lemma.toLower() == QStringLiteral("have"))
            return true;
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_AUX005::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — модальный AUX, присоединённый как aux
    if (anchor.upos != Upos::AUX || anchor.deprel != Deprel::Aux)
        return res;

    if (!modalVerbs.contains(anchor.lemma.toLower()))
        return res;

    // V — голова M
    const TokenNode* v = anchor.parent;
    if (v == nullptr)
        return res;

    // V должно быть причастием (VerbForm=Part)
    if (!v->features.verbForm.has_value() ||
        v->features.verbForm.value() != VerbFormValue::Part)
        return res;

    // У V не должно быть зависимого have
    if (hasHaveChild(*v))
        return res;

    // Следующий токен после M (линейно), пропуская PUNCT и MWT
    const TokenNode* nextTok = anchor.nextNonPunct();

    // Ветка (б): следующий токен — of (ADP). Заменить of на have.
    if (nextTok != nullptr &&
        nextTok->upos == Upos::ADP &&
        nextTok->lemma.toLower() == QStringLiteral("of")) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("AUX-005");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id, nextTok->id, v->id};
        ce.conflictTokenIds = {nextTok->id};
        res.insert(ce);
        return res;
    }

    // Ветка (а): of нет, вставить have между M и V
    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-005");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, v->id};
    ce.conflictTokenIds = {v->id};
    res.insert(ce);
    return res;
}