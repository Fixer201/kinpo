/*!
* \file rule_conj005.h
* \brief Объявление правила CONJ-005.
*
* Стилистическая рекомендация: конструкция if...or not заменяется на
* whether...or not.
*/

#ifndef RULE_CONJ005_H
#define RULE_CONJ005_H

#include "datamodel.h"

class Rule_CONJ005 : public Rule {
public:
    static const Rule_CONJ005& instance();

    /*! \brief Идентификатор правила CONJ-005. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ005_H