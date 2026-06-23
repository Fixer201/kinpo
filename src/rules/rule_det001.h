/*!
* \file rule_det001.h
* \brief Объявление правила DET-001.
*/

#ifndef RULE_DET001_H
#define RULE_DET001_H

#include "datamodel.h"

class Rule_DET001 : public Rule {
public:
    static const Rule_DET001& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_DET001_H