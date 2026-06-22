/*!
* \file rule_aux002.cpp
* \brief Реализация правила AUX-002: неверная форма после AUX.
*
* Якорь — вспомогательный глагол A (UPOS=AUX, DEPREL=aux). V — голова A,
* UPOS=VERB. Ветка (а): A.LEMMA=do, V.VerbForm ≠ Inf, ожидается
* инфинитив (лемма V). Ветка (б): A.LEMMA=have, V.VerbForm ≠ Part,
* ожидается Past Participle (форма из past_forms.txt, колонка
* Past Participle; если лемма V не найдена в словаре, используется
* лемма V). displayTokenIds = [A.id, V.id], conflictTokenIds = {V.id}.
*/

#include "rule_aux002.h"
#include <QSet>

QString Rule_AUX002::ruleId() const
{
    return QStringLiteral("AUX-002");
}

QSet<Upos> Rule_AUX002::anchorUpos() const
{
    return {Upos::AUX};
}

bool Rule_AUX002::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_AUX002::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — вспомогательный глагол, присоединённый как aux
    if (anchor.upos != Upos::AUX || anchor.deprel != Deprel::Aux)
        return res;

    const QString aLemma = anchor.lemma.toLower();

    // Ветка (а): do + не-инфинитив
    // Ветка (б): have + не-причастие
    const bool isDo = (aLemma == QStringLiteral("do"));
    const bool isHave = (aLemma == QStringLiteral("have"));
    if (!isDo && !isHave)
        return res;

    // V — голова A, должна быть обычным глаголом
    const TokenNode* v = anchor.parent;
    if (v == nullptr || v->upos != Upos::VERB)
        return res;

    if (isDo) {
        // После do ожидается инфинитив (VerbForm=Inf)
        // Если VerbForm отсутствует или не Inf, форма неверная
        if (v->features.verbForm.has_value() &&
            v->features.verbForm.value() == VerbFormValue::Inf)
            return res;

        // do избыточен, исправляем V на инфинитив (лемма V)
        CandidateError ce;
        ce.ruleId = QStringLiteral("AUX-002");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id, v->id};
        ce.conflictTokenIds = {v->id};
        res.insert(ce);
        return res;
    }

    // isHave: после have ожидается Past Participle (VerbForm=Part)
    if (v->features.verbForm.has_value() &&
        v->features.verbForm.value() == VerbFormValue::Part)
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, v->id};
    ce.conflictTokenIds = {v->id};
    res.insert(ce);
    return res;
}