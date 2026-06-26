/*!
* \file rule_other002.cpp
* \brief Реализация правила OTHER-002: good как наречие.
*
* Якорь — прилагательное good (UPOS=ADJ, LEMMA=good, DEPREL=advmod).
* V — голова якоря (UPOS=VERB). Исключение: V.LEMMA=do («do good»,
* устойчивое выражение, good существительное). При выполнении условий
* good заменяется на well.
*/

#include "rule_other002.h"
#include <QSet>

const Rule_OTHER002& Rule_OTHER002::instance()
{
    static Rule_OTHER002 inst;
    return inst;
}

QString Rule_OTHER002::ruleId() const
{
    return QStringLiteral("OTHER-002");
}

QSet<Upos> Rule_OTHER002::anchorUpos() const
{
    return {Upos::ADJ};
}

bool Rule_OTHER002::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER002::check(const TokenNode& anchor,
                                          int /*sentenceIndex*/,
                                          const DocumentModel& /*document*/,
                                          const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — прилагательное good в наречной позиции (advmod)
    if (anchor.upos != Upos::ADJ || anchor.deprel != Deprel::Advmod)
        return res;

    if (anchor.lemma.toLower() != QStringLiteral("good"))
        return res;

    // V — голова якоря, должна быть глаголом
    const TokenNode* v = anchor.parent;
    if (v == nullptr || v->upos != Upos::VERB)
        return res;

    // Исключение: «do good» — устойчивое выражение
    if (v->lemma.toLower() == QStringLiteral("do"))
        return res;

    // good заменяется на well
    CandidateError ce;
    ce.ruleId = QStringLiteral("OTHER-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::ReplaceTokens;
        edit.targetTokenIds = {anchor.id};
        edit.newTokens.append(QStringLiteral("well"));
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("В наречной позиции используется «well», а не «good».");
    res.insert(ce);
    return res;
}