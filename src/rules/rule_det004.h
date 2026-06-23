/*!
* \file rule_det004.h
* \brief Объявление правила DET-004.
*/

#ifndef RULE_DET004_H
#define RULE_DET004_H

#include "datamodel.h"

class Rule_DET004 : public Rule {
public:
    static const Rule_DET004& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_DET004_H