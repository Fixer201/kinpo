/*!
* \file rule_det002.h
* \brief Объявление правила DET-002.
*/

#ifndef RULE_DET002_H
#define RULE_DET002_H

#include "datamodel.h"

class Rule_DET002 : public Rule {
public:
    static const Rule_DET002& instance();

    /*! \brief Идентификатор правила DET-002. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_DET002_H