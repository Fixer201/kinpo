#include "rule_other006.h"
#include <QSet>
#include <QString>

const Rule_OTHER006& Rule_OTHER006::instance()
{
    static Rule_OTHER006 inst;
    return inst;
}

namespace {

const QSet<QString>& comparativeLemmaAnchors()
{
    static const QSet<QString> s = {
        QStringLiteral("more"),
        QStringLiteral("less"),
        QStringLiteral("fewer"),
        QStringLiteral("rather"),
        QStringLiteral("other"),
    };
    return s;
}

bool isComparativeAnchor(const TokenNode& anchor)
{
    if (anchor.features.degree.has_value() &&
        anchor.features.degree.value() == DegreeValue::Cmp)
        return true;
    return comparativeLemmaAnchors().contains(anchor.lemma.toLower());
}

} // namespace

QString Rule_OTHER006::ruleId() const
{
    return QStringLiteral("OTHER-006");
}

QSet<Upos> Rule_OTHER006::anchorUpos() const
{
    return {Upos::ADJ, Upos::ADV};
}

bool Rule_OTHER006::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER006::check(const TokenNode& anchor,
                                           int /*sentenceIndex*/,
                                           const DocumentModel& /*document*/,
                                           const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::ADJ && anchor.upos != Upos::ADV)
        return res;

    if (!isComparativeAnchor(anchor))
        return res;

    QSet<int> foundThenIds;

    for (const TokenNode* child : anchor.children) {
        if (!child)
            continue;
        if (child->lemma.toLower() == QStringLiteral("then")) {
            foundThenIds.insert(child->id);
            if (const TokenNode* next = child->nextToken) {
                if (next->lemma.toLower() == QStringLiteral("then"))
                    foundThenIds.insert(next->id);
            }
        }
    }

    if (foundThenIds.isEmpty()) {
        if (const TokenNode* next = anchor.nextToken) {
            if (next->lemma.toLower() == QStringLiteral("then"))
                foundThenIds.insert(next->id);
        }
    }

    for (int id : foundThenIds) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-006");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {id};
        ce.conflictTokenIds = {id};
        {
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {id};
            edit.newTokens = {QStringLiteral("than")};
            ce.edits.append(edit);
        }
        ce.description = QStringLiteral("После сравнительной степени ожидается «than», а не «then».");
        res.insert(ce);
    }

    return res;
}