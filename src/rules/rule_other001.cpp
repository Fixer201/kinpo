/*!
* \file rule_other001.cpp
* \brief Реализация правила OTHER-001: двойное сравнение.
*
* Якорь — наречие more или most (UPOS=ADV, DEPREL=advmod). A — голова
* якоря (UPOS=ADJ). Если LEMMA=more и A.Degree=Cmp, то more избыточен
* и удаляется. Если LEMMA=most и A.Degree=Sup, то most избыточен
* и удаляется.
*/

#include "rule_other001.h"
#include <QSet>

const Rule_OTHER001& Rule_OTHER001::instance()
{
    static Rule_OTHER001 inst;
    return inst;
}

QString Rule_OTHER001::ruleId() const
{
    return QStringLiteral("OTHER-001");
}

QSet<Upos> Rule_OTHER001::anchorUpos() const
{
    return {Upos::ADV};
}

bool Rule_OTHER001::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER001::check(const TokenNode& anchor,
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

    // more + Degree=Cmp → удалить more
    if (lemma == QStringLiteral("more")) {
        if (a->features.degree.has_value() &&
            a->features.degree.value() == DegreeValue::Cmp) {
            CandidateError ce;
            ce.ruleId = QStringLiteral("OTHER-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            {
                AtomicEdit edit;
                edit.type = AtomicEditType::DeleteTokens;
                edit.targetTokenIds = {anchor.id};
                ce.edits.append(edit);
            }
            ce.description = QStringLiteral("«%1» избыточно при синтетической степени «%2».")
                                 .arg(anchor.form).arg(a->form);
            res.insert(ce);
        }
        return res;
    }

    // most + Degree=Sup → удалить most
    if (lemma == QStringLiteral("most")) {
        if (a->features.degree.has_value() &&
            a->features.degree.value() == DegreeValue::Sup) {
            CandidateError ce;
            ce.ruleId = QStringLiteral("OTHER-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            {
                AtomicEdit edit;
                edit.type = AtomicEditType::DeleteTokens;
                edit.targetTokenIds = {anchor.id};
                ce.edits.append(edit);
            }
            ce.description = QStringLiteral("«%1» избыточно при синтетической степени «%2».")
                                 .arg(anchor.form).arg(a->form);
            res.insert(ce);
        }
        return res;
    }

    return res;
}