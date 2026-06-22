/*!
* \file rule_det003.h
* \brief Объявление правила DET-003.
*/

#ifndef RULE_DET003_H
#define RULE_DET003_H

#include "datamodel.h"

class Rule_DET003 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_DET003_H