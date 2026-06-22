/*!
* \file rule_art006.h
* \brief Объявление правила ART-006.
*/

#ifndef RULE_ART006_H
#define RULE_ART006_H

#include "datamodel.h"

class Rule_ART006 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART006_H