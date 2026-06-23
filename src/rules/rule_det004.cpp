/*!
* \file rule_det004.cpp
* \brief Реализация правила DET-004: Омофон в притяжательной позиции.
*
* Срабатывает на токен с DEPREL ∈ {nmod:poss, det}, HEAD — NOUN,
* если FORM (без учёта регистра) совпадает с таблицей омофонов:
* it's → its, there → their, you're → your.
*/

#include "rule_det004.h"
#include <QHash>
#include <QSet>

const Rule_DET004& Rule_DET004::instance()
{
    static Rule_DET004 inst;
    return inst;
}

QString Rule_DET004::ruleId() const
{
    return QStringLiteral("DET-004");
}

QSet<Upos> Rule_DET004::anchorUpos() const
{
    return {Upos::PRON, Upos::ADV};
}

bool Rule_DET004::canConflict() const
{
    return true;
}

namespace {

// Таблица омофонов: ошибочная форма → правильная притяжательная
const QHash<QString, QString> homophoneToPossessive = {
    {QStringLiteral("it's"),   QStringLiteral("its")},
    {QStringLiteral("there"),  QStringLiteral("their")},
    {QStringLiteral("you're"),  QStringLiteral("your")}
};

} // namespace

QSet<CandidateError> Rule_DET004::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // DEPREL ∈ {nmod:poss, det}
    if (anchor.deprel != Deprel::NmodPoss && anchor.deprel != Deprel::Det)
        return res;

    // HEAD должен быть NOUN
    if (!anchor.parent || anchor.parent->upos != Upos::NOUN)
        return res;

    // FORM должна быть в таблице омофонов
    const QString formLower = anchor.form.toLower();
    if (!homophoneToPossessive.contains(formLower))
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("DET-004");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}