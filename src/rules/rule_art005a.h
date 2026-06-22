/*!
* \file rule_art005a.h
* \brief Объявление правила ART-005a.
*/

#ifndef RULE_ART005A_H
#define RULE_ART005A_H

#include "datamodel.h"

class Rule_ART005a : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART005A_H