/*!
* \file rule_prep003.h
* \brief Объявление правила PREP-003.
*
* Правило проверяет предлоги while/during: while требует именную группу,
* during требует клаузу. Тип HEAD различает именную группу и клаузу.
*/

#ifndef RULE_PREP003_H
#define RULE_PREP003_H

#include "datamodel.h"

class Rule_PREP003 : public Rule {
public:
    static const Rule_PREP003& instance();

    /*! \brief Идентификатор правила PREP-003. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_PREP003_H