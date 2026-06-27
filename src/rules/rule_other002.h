/*!
* \file rule_other002.h
* \brief Объявление правила OTHER-002.
*
* Правило проверяет использование good в наречной позиции вместо well.
*/

#ifndef RULE_OTHER002_H
#define RULE_OTHER002_H

#include "datamodel.h"

class Rule_OTHER002 : public Rule {
public:
    static const Rule_OTHER002& instance();

    /*! \brief Идентификатор правила OTHER-002. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER002_H