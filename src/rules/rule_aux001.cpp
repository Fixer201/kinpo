/*!
* \file rule_aux001.cpp
* \brief Реализация правила AUX-001: do + модальный.
*
* Якорь — вспомогательный глагол do (UPOS=AUX, LEMMA=do, DEPREL=aux).
* V — голова do. Если у V есть другой зависимый M (отличный от D) с
* UPOS=AUX, леммой из модального списка и DEPREL=aux, do избыточен
* и удаляется.
* Модальный список: can, could, may, might, must, shall, should, will, would.
*/

#include "rule_aux001.h"
#include <QSet>

const Rule_AUX001& Rule_AUX001::instance()
{
    static Rule_AUX001 inst;
    return inst;
}

QString Rule_AUX001::ruleId() const
{
    return QStringLiteral("AUX-001");
}

QSet<Upos> Rule_AUX001::anchorUpos() const
{
    return {Upos::AUX};
}

bool Rule_AUX001::canConflict() const
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
* \brief Проверить, есть ли у V другой зависимый-модальный.
* \param [in] v Голова якоря D.
* \param [in] d Якорь D, исключаемый из поиска.
* \return true, если среди прямых зависимых V найден M, отличный от D,
*         с UPOS=AUX, леммой из модального списка и DEPREL=aux.
*/
bool hasModalChild(const TokenNode& v, const TokenNode& d)
{
    for (const TokenNode* child : v.children) {
        if (child == &d)
            continue;
        if (child->upos != Upos::AUX)
            continue;
        if (child->deprel != Deprel::Aux)
            continue;
        if (modalVerbs.contains(child->lemma.toLower()))
            return true;
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_AUX001::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — вспомогательный do, присоединённый как aux
    if (anchor.upos != Upos::AUX || anchor.deprel != Deprel::Aux)
        return res;

    // LEMMA=do покрывает формы did/does (лемма одинаковая)
    if (anchor.lemma.toLower() != QStringLiteral("do"))
        return res;

    // V — голова do
    const TokenNode* v = anchor.parent;
    if (v == nullptr)
        return res;

    // У V должен быть другой зависимый-модальный
    if (!hasModalChild(*v, anchor))
        return res;

    // do избыточен, удаляем
    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}