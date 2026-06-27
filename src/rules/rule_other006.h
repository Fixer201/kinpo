#ifndef RULE_OTHER006_H
#define RULE_OTHER006_H

#include "datamodel.h"

class Rule_OTHER006 : public Rule {
public:
    static const Rule_OTHER006& instance();

    /*! \brief Идентификатор правила OTHER-006. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER006_H