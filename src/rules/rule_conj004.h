/*!
* \file rule_conj004.h
* \brief Объявление правила CONJ-004.
*
* Правило проверяет неверный коррелят: or при neither либо nor при either.
*/

#ifndef RULE_CONJ004_H
#define RULE_CONJ004_H

#include "datamodel.h"

class Rule_CONJ004 : public Rule {
public:
    static const Rule_CONJ004& instance();

    /*! \brief Идентификатор правила CONJ-004. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ004_H