/*!
* \file rule_art003.h
* \brief Объявление правила ART-003.
*/

#ifndef RULE_ART003_H
#define RULE_ART003_H

#include "datamodel.h"

class Rule_ART003 : public Rule {
public:
    static const Rule_ART003& instance();

    /*! \brief Идентификатор правила ART-003. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART003_H