/*!
* \file rule_aux004.h
* \brief Объявление правила AUX-004.
*
* Правило проверяет два модальных глагола у одного VERB.
* Оставляется первый модальный (с меньшим ID), второй удаляется.
*/

#ifndef RULE_AUX004_H
#define RULE_AUX004_H

#include "datamodel.h"

class Rule_AUX004 : public Rule {
public:
    static const Rule_AUX004& instance();

    /*! \brief Идентификатор правила AUX-004. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX004_H