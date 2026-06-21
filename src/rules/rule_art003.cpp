/*!
* \file rule_art003.cpp
* \brief Реализация правила ART-003.
*/

#include "rule_art003.h"
#include <QSet>
#include <QStringList>

QString Rule_ART003::ruleId() const
{
    return QStringLiteral("ART-003");
}

QSet<Upos> Rule_ART003::anchorUpos() const
{
    return {Upos::DET};
}

bool Rule_ART003::canConflict() const
{
    return true;
}

namespace {

bool isArticle(const QString& form)
{
    const QString f = form.toLower();
    return f == QStringLiteral("a") || f == QStringLiteral("an") || f == QStringLiteral("the");
}

bool isArticleAorAn(const QString& form)
{
    const QString f = form.toLower();
    return f == QStringLiteral("a") || f == QStringLiteral("an");
}

bool nounHasAmodChild(const TokenNode& noun)
{
    for (const TokenNode* child : noun.children)
        if (child->deprel == Deprel::Amod)
            return true;
    return false;
}

bool nounHasCompoundChild(const TokenNode& noun)
{
    for (const TokenNode* child : noun.children)
        if (child->deprel == Deprel::Compound)
            return true;
    return false;
}

bool nounHasChildWithLemma(const TokenNode& noun, const QString& lemma)
{
    const QString lower = lemma.toLower();
    for (const TokenNode* child : noun.children)
        if (child->lemma.toLower() == lower)
            return true;
    return false;
}

bool isSportWithActivityVerb(const TokenNode& noun, const CheckerRuntime& runtime,
                              const QString& articleForm)
{
    if (!isArticleAorAn(articleForm))
        return false;

    if (!runtime.resources.sports.contains(noun.lemma.toLower()))
        return false;

    if (!noun.parent)
        return false;

    return runtime.resources.activityVerbs.contains(noun.parent->lemma.toLower());
}

} // namespace

QSet<CandidateError> Rule_ART003::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::DET)
        return res;

    if (!isArticle(anchor.form))
        return res;

    if (anchor.deprel != Deprel::Det)
        return res;

    if (!anchor.parent)
        return res;

    const TokenNode& noun = *anchor.parent;
    const QString nounLemma = noun.lemma.toLower();
    const QString articleForm = anchor.form.toLower();

    bool isLanguage = (noun.upos == Upos::PROPN && runtime.resources.languages.contains(nounLemma));
    bool isSport = runtime.resources.sports.contains(nounLemma);
    bool isMeal = runtime.resources.meals.contains(nounLemma);

    if (!isLanguage && !isSport && !isMeal)
        return res;

    if (isSport && isArticleAorAn(articleForm)) {
        if (!isSportWithActivityVerb(noun, runtime, articleForm))
            return res;
        if (nounHasCompoundChild(noun))
            return res;
    }

    if (isLanguage && nounHasChildWithLemma(noun, QStringLiteral("language")))
        return res;

    if ((isSport || isMeal) && nounHasAmodChild(noun))
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-003");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}