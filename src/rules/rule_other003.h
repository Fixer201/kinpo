/*!
* \file rule_other003.h
* \brief Объявление правила OTHER-003.
*
* Правило проверяет выбор между much и very перед прилагательным.
*/

#ifndef RULE_OTHER003_H
#define RULE_OTHER003_H

#include "datamodel.h"

class Rule_OTHER003 : public Rule {
public:
    static const Rule_OTHER003& instance();

    /*! \brief Идентификатор правила OTHER-003. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER003_H