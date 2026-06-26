/*!
* \file rule_aux006.cpp
* \brief Реализация правила AUX-006: to после let/make.
*
* Якорь — частица to (UPOS=PART, LEMMA=to, DEPREL=mark). V2 — голова to
* (VERB, присоединён к V через xcomp). V — голова V2 (VERB, LEMMA=let
* или make). У V должен быть зависимый с DEPREL=obj (дополнение).
* Исключение: у V есть зависимый с DEPREL=aux:pass или nsubj:pass
* (пассивная конструкция, to корректен). При выполнении условий to
* избыточен и удаляется.
*/

#include "rule_aux006.h"
#include <QSet>

const Rule_AUX006& Rule_AUX006::instance()
{
    static Rule_AUX006 inst;
    return inst;
}

QString Rule_AUX006::ruleId() const
{
    return QStringLiteral("AUX-006");
}

QSet<Upos> Rule_AUX006::anchorUpos() const
{
    return {Upos::PART};
}

bool Rule_AUX006::canConflict() const
{
    return true;
}

namespace {

// Глаголы, после которых в активной конструкции с дополнением
// инфинитив идёт без to (bare infinitive).
const QSet<QString> bareInfinitiveVerbs = {
    QStringLiteral("let"), QStringLiteral("make")
};

/*!
* \brief Проверить, есть ли у V зависимый с указанным DEPREL.
* \param [in] v Голова V2.
* \param [in] deprel Искомый DEPREL.
* \return true, если среди прямых зависимых V есть токен с таким DEPREL.
*/
bool hasChildWithDeprel(const TokenNode& v, Deprel deprel)
{
    for (const TokenNode* child : v.children) {
        if (child->deprel == deprel)
            return true;
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_AUX006::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — частица to, присоединённая как маркер
    if (anchor.upos != Upos::PART || anchor.deprel != Deprel::Mark)
        return res;

    if (anchor.lemma.toLower() != QStringLiteral("to"))
        return res;

    // V2 — голова to (должна быть глаголом)
    const TokenNode* v2 = anchor.parent;
    if (v2 == nullptr || v2->upos != Upos::VERB)
        return res;

    // V — голова V2 (должна быть глаголом let или make)
    const TokenNode* v = v2->parent;
    if (v == nullptr || v->upos != Upos::VERB)
        return res;

    if (!bareInfinitiveVerbs.contains(v->lemma.toLower()))
        return res;

    // V2 должен быть присоединён к V через xcomp
    if (v2->deprel != Deprel::Xcomp)
        return res;

    // У V должен быть зависимый с DEPREL=obj (дополнение)
    if (!hasChildWithDeprel(*v, Deprel::Obj))
        return res;

    // Исключение: пассивная конструкция. У V есть aux:pass или nsubj:pass.
    // В пассиве to после made корректен («He was made to clean»).
    if (hasChildWithDeprel(*v, Deprel::AuxPass) ||
        hasChildWithDeprel(*v, Deprel::NsubjPass))
        return res;

    // to избыточен, удаляем
    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-006");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::DeleteTokens;
        edit.targetTokenIds = {anchor.id};
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("После «%1» + дополнение частица «to» не используется (bare infinitive).")
                         .arg(v->lemma);
    res.insert(ce);
    return res;
}