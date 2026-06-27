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
    return {Upos::Pron, Upos::Det, Upos::Adv};
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

    // MWT-формы исключаются: it's, you're как составные токены отсутствуют
    if (anchor.isMwtFragment)
        return res;

    // HEAD должен быть NOUN
    if (!anchor.parent || anchor.parent->upos != Upos::Noun)
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
    const QString possessive = homophoneToPossessive.value(formLower);
    AtomicEdit edit;
    edit.type = AtomicEditType::ReplaceTokens;
    edit.targetTokenIds = {anchor.id};
    edit.newTokens.append(possessive);
    ce.edits.append(edit);
    ce.description = QStringLiteral("«%1» в притяжательной позиции: ожидается «%2».")
                         .arg(anchor.form).arg(possessive);
    res.insert(ce);
    return res;
}