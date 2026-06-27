/*!
* \file rule_aux003.cpp
* \brief Реализация правила AUX-003: to после модального.
*
* Якорь — частица to (UPOS=PART, LEMMA=to, DEPREL=mark). V — голова to.
* Если у V есть зависимый модальный (AUX с леммой из модального списка
* и DEPREL=aux), частица to избыточна и удаляется.
* Модальный список: can, could, may, might, must, shall, should, will, would.
*/

#include "rule_aux003.h"
#include <QSet>

const Rule_AUX003& Rule_AUX003::instance()
{
    static Rule_AUX003 inst;
    return inst;
}

QString Rule_AUX003::ruleId() const
{
    return QStringLiteral("AUX-003");
}

QSet<Upos> Rule_AUX003::anchorUpos() const
{
    return {Upos::Part};
}

bool Rule_AUX003::canConflict() const
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
* \brief Проверить, есть ли у V зависимый модальный.
* \param [in] v Голова якоря T.
* \return true, если среди прямых зависимых V найден AUX с леммой
*         из модального списка и DEPREL=aux.
*/
bool hasModalChild(const TokenNode& v)
{
    bool found = false;
    for (const TokenNode* child : v.children) {
        if (child->upos == Upos::Aux && child->deprel == Deprel::Aux &&
            modalVerbs.contains(child->lemma.toLower()))
            found = true;
    }
    return found;
}

} // namespace

QSet<CandidateError> Rule_AUX003::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — частица to, присоединённая как маркер
    if (anchor.upos != Upos::Part || anchor.deprel != Deprel::Mark)
        return res;

    if (anchor.lemma.toLower() != QStringLiteral("to"))
        return res;

    // V — голова to
    const TokenNode* v = anchor.parent;
    if (v == nullptr)
        return res;

    // У V должен быть зависимый модальный
    if (!hasModalChild(*v))
        return res;

    // to избыточен, удаляем
    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-003");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::DeleteTokens;
        edit.targetTokenIds = {anchor.id};
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("Частица «to» не используется после модального глагола.");
    res.insert(ce);
    return res;
}