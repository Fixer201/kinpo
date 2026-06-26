/*!
* \file rule_aux004.cpp
* \brief Реализация правила AUX-004: два модальных у одного VERB.
*
* Якорь — модальный AUX (UPOS=AUX, лемма из модального списка, DEPREL=aux).
* V — голова якоря. Если у V есть другой зависимый модальный с меньшим ID,
* то якорь (второй модальный) избыточен и удаляется.
* Модальный список: can, could, may, might, must, shall, should, will, would.
*/

#include "rule_aux004.h"
#include <QSet>

const Rule_AUX004& Rule_AUX004::instance()
{
    static Rule_AUX004 inst;
    return inst;
}

QString Rule_AUX004::ruleId() const
{
    return QStringLiteral("AUX-004");
}

QSet<Upos> Rule_AUX004::anchorUpos() const
{
    return {Upos::AUX};
}

bool Rule_AUX004::canConflict() const
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
* \brief Проверить, есть ли у V другой зависимый модальный с меньшим ID.
* \param [in] v Голова якоря.
* \param [in] anchorId ID якоря (второго модального).
* \return true, если среди прямых зависимых V найден AUX с леммой
*         из модального списка, DEPREL=aux и ID меньше anchorId.
*/
bool hasEarlierModalChild(const TokenNode& v, int anchorId)
{
    for (const TokenNode* child : v.children) {
        if (child->id >= anchorId)
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

QSet<CandidateError> Rule_AUX004::check(const TokenNode& anchor,
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

    // V — голова якоря
    const TokenNode* v = anchor.parent;
    if (v == nullptr)
        return res;

    // У V должен быть другой модальный с меньшим ID (первый модальный)
    if (!hasEarlierModalChild(*v, anchor.id))
        return res;

    // Якорь — второй модальный, удаляем
    CandidateError ce;
    ce.ruleId = QStringLiteral("AUX-004");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::DeleteTokens;
        edit.targetTokenIds = {anchor.id};
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("Два модальных глагола подряд недопустимы.");
    res.insert(ce);
    return res;
}