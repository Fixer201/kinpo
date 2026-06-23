/*!
* \file rule_det003.cpp
* \brief Реализация правила DET-003: Притяжательный омофон как подлежащее.
*
* Срабатывает на PRON с XPOS=PRP$ (или Poss=Yes), DEPREL=nsubj, если HEAD —
* VERB, AUX или ADJ. Таблица омофонов: its → it's, their → they're, your → you're.
* Не срабатывает при DEPREL=nmod:poss.
*/

#include "rule_det003.h"
#include <QHash>
#include <QSet>

const Rule_DET003& Rule_DET003::instance()
{
    static Rule_DET003 inst;
    return inst;
}

QString Rule_DET003::ruleId() const
{
    return QStringLiteral("DET-003");
}

QSet<Upos> Rule_DET003::anchorUpos() const
{
    return {Upos::PRON};
}

bool Rule_DET003::canConflict() const
{
    return true;
}

namespace {

// Таблица омофонов: притяжательное → сокращение
const QHash<QString, QString> possToContraction = {
    {QStringLiteral("its"),   QStringLiteral("it's")},
    {QStringLiteral("their"), QStringLiteral("they're")},
    {QStringLiteral("your"),  QStringLiteral("you're")}
};

} // namespace

QSet<CandidateError> Rule_DET003::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::PRON)
        return res;

    // Признак притяжательности: XPOS=PRP$ или Poss=Yes в FEATS
    const bool isPossessive = (anchor.xpos == QStringLiteral("PRP$")) ||
                              anchor.features.poss;
    if (!isPossessive)
        return res;

    // Не срабатывает при DEPREL=nmod:poss
    if (anchor.deprel == Deprel::NmodPoss)
        return res;

    // DEPREL должен быть nsubj
    if (anchor.deprel != Deprel::Nsubj)
        return res;

    // HEAD должен быть VERB, AUX или ADJ
    if (!anchor.parent)
        return res;
    const Upos headUpos = anchor.parent->upos;
    if (headUpos != Upos::VERB && headUpos != Upos::AUX && headUpos != Upos::ADJ)
        return res;

    // LEMMA должна быть в таблице омофонов
    const QString lemmaLower = anchor.lemma.toLower();
    if (!possToContraction.contains(lemmaLower))
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("DET-003");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}