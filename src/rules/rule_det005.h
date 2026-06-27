/*!
* \file rule_det005.h
* \brief Объявление правила DET-005.
*
* Правило обнаруживает неисчисляемые существительные, поставленные
* во множественное число. Цель проверки — сам токен NOUN.
*/

#ifndef RULE_DET005_H
#define RULE_DET005_H

#include "datamodel.h"

class Rule_DET005 : public Rule {
public:
    static const Rule_DET005& instance();

    /*! \brief Идентификатор правила DET-005. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_DET005_H