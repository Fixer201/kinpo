/*!
* \file rule_art003.h
* \brief Объявление правила ART-003.
*/

#ifndef RULE_ART003_H
#define RULE_ART003_H

#include "datamodel.h"

class Rule_ART003 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART003_H