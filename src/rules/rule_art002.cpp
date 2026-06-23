/*!
* \file rule_art002.cpp
* \brief Реализация правил ART-002 и ART-002a.
*
* ART-002: пропущен "the" перед ADJ (Degree=Sup / NumType=Ord / adj_requires_the).
* ART-002a: аналитическая превосходная степень (most/least + ADJ + NOUN).
*/

#include "rule_art002.h"
#include <QSet>

const Rule_ART002& Rule_ART002::instance()
{
    static Rule_ART002 inst;
    return inst;
}

// ========================================================================
// ART-002: Пропущен "the" перед ADJ
// ========================================================================

QString Rule_ART002::ruleId() const
{
    return QStringLiteral("ART-002");
}

QSet<Upos> Rule_ART002::anchorUpos() const
{
    return {Upos::ADJ};
}

bool Rule_ART002::canConflict() const
{
    return true;
}

namespace {

bool nounHasDet(const TokenNode& noun)
{
    for (const TokenNode* child : noun.children)
        if (child->deprel == Deprel::Det)
            return true;
    return false;
}

bool isTimeException(const TokenNode& adj, const TokenNode& noun,
                     const CheckerRuntime& runtime)
{
    const QString adjLemma = adj.lemma.toLower();
    if (adjLemma != QStringLiteral("last") && adjLemma != QStringLiteral("next"))
        return false;

    const QString nounLemma = noun.lemma.toLower();
    if (!runtime.resources.timeUnits.contains(nounLemma) &&
        !runtime.resources.durations.contains(nounLemma))
        return false;

    for (const TokenNode* child : noun.children) {
        if (child->deprel == Deprel::Nmod) {
            for (const TokenNode* sub : child->children) {
                if (sub->deprel == Deprel::Case && sub->lemma.toLower() == QStringLiteral("of"))
                    return false;
            }
        }
        if (child->upos == Upos::NUM)
            return false;
        if (child->deprel == Deprel::Case) {
            const QString caseLemma = child->lemma.toLower();
            if (caseLemma == QStringLiteral("for") ||
                caseLemma == QStringLiteral("in") ||
                caseLemma == QStringLiteral("over"))
                return false;
        }
    }
    return true;
}

} // namespace

QSet<CandidateError> Rule_ART002::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::ADJ)
        return res;
    if (anchor.deprel != Deprel::Amod)
        return res;
    if (!anchor.parent)
        return res;

    const TokenNode& noun = *anchor.parent;
    if (noun.upos != Upos::NOUN)
        return res;

    const QString adjLemma = anchor.lemma.toLower();
    const bool isSup = (anchor.features.degree == DegreeValue::Sup);
    const bool isOrd = anchor.features.numTypeOrd;
    const bool isAdjRequiresThe = runtime.resources.adjRequiresThe.contains(adjLemma);

    if (!isSup && !isOrd && !isAdjRequiresThe)
        return res;

    if (nounHasDet(noun))
        return res;

    if (noun.upos == Upos::PROPN)
        return res;

    if (isTimeException(anchor, noun, runtime))
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, noun.id};
    ce.conflictTokenIds = {noun.id};
    res.insert(ce);
    return res;
}

// ========================================================================
// ART-002a: Аналитическая превосходная степень
// ========================================================================

QString Rule_ART002a::ruleId() const
{
    return QStringLiteral("ART-002a");
}

QSet<Upos> Rule_ART002a::anchorUpos() const
{
    return {Upos::ADV};
}

bool Rule_ART002a::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_ART002a::check(const TokenNode& anchor,
                                          int /*sentenceIndex*/,
                                          const DocumentModel& /*document*/,
                                          const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::ADV)
        return res;
    if (anchor.deprel != Deprel::Advmod)
        return res;

    const QString advLemma = anchor.lemma.toLower();
    if (advLemma != QStringLiteral("most") && advLemma != QStringLiteral("least"))
        return res;

    if (!anchor.parent)
        return res;
    const TokenNode& adj = *anchor.parent;
    if (adj.upos != Upos::ADJ)
        return res;
    if (adj.deprel != Deprel::Amod)
        return res;

    if (!adj.parent)
        return res;
    const TokenNode& noun = *adj.parent;
    if (noun.upos != Upos::NOUN)
        return res;

    if (nounHasDet(noun))
        return res;

    if (noun.upos == Upos::PROPN)
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-002a");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, adj.id, noun.id};
    ce.conflictTokenIds = {noun.id};
    res.insert(ce);
    return res;
}