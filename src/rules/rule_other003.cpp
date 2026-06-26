/*!
* \file rule_other003.cpp
* \brief Реализация правила OTHER-003: much/very перед прилагательным.
*
* Якорь — наречие much или very (UPOS=ADV, DEPREL=advmod). A — голова
* якоря (UPOS=ADJ). Если LEMMA=much и A.Degree ≠ Cmp, much заменяется
* на very. Если LEMMA=very и A.Degree=Cmp, very заменяется на much.
*/

#include "rule_other003.h"
#include <QSet>

const Rule_OTHER003& Rule_OTHER003::instance()
{
    static Rule_OTHER003 inst;
    return inst;
}

QString Rule_OTHER003::ruleId() const
{
    return QStringLiteral("OTHER-003");
}

QSet<Upos> Rule_OTHER003::anchorUpos() const
{
    return {Upos::ADV};
}

bool Rule_OTHER003::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER003::check(const TokenNode& anchor,
                                          int /*sentenceIndex*/,
                                          const DocumentModel& /*document*/,
                                          const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — наречие, присоединённое как advmod
    if (anchor.upos != Upos::ADV || anchor.deprel != Deprel::Advmod)
        return res;

    const QString lemma = anchor.lemma.toLower();

    // A — голова якоря, должна быть прилагательным
    const TokenNode* a = anchor.parent;
    if (a == nullptr || a->upos != Upos::ADJ)
        return res;

    const bool isCmp = a->features.degree.has_value() &&
                       a->features.degree.value() == DegreeValue::Cmp;

    // much перед прилагательным без Cmp → very
    if (lemma == QStringLiteral("much") && !isCmp) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        {
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {anchor.id};
            edit.newTokens = {QStringLiteral("very")};
            ce.edits.append(edit);
        }
        ce.description = QStringLiteral("Перед прилагательным без сравнительной степени используется «very», а не «much».");
        res.insert(ce);
        return res;
    }

    // very перед сравнительной степенью → much
    if (lemma == QStringLiteral("very") && isCmp) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-003");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        {
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {anchor.id};
            edit.newTokens = {QStringLiteral("much")};
            ce.edits.append(edit);
        }
        ce.description = QStringLiteral("Перед прилагательным в сравнительной степени используется «much», а не «very».");
        res.insert(ce);
        return res;
    }

    return res;
}