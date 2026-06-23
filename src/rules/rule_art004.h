/*!
* \file rule_art004.h
* \brief Объявление правила ART-004.
*/

#ifndef RULE_ART004_H
#define RULE_ART004_H

#include "datamodel.h"

class Rule_ART004 : public Rule {
public:
    static const Rule_ART004& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART004_H