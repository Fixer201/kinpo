#ifndef RULE_OTHER006_H
#define RULE_OTHER006_H

#include "datamodel.h"

class Rule_OTHER006 : public Rule {
public:
    static const Rule_OTHER006& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER006_H