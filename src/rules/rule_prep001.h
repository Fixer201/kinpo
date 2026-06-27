/*!
* \file rule_prep001.h
* \brief Объявление правила PREP-001.
*
* Правило проверяет временные предлоги in/on/at: правильный предлог
* определяется типом временного выражения (HEAD предлога).
*/

#ifndef RULE_PREP001_H
#define RULE_PREP001_H

#include "datamodel.h"

class Rule_PREP001 : public Rule {
public:
    static const Rule_PREP001& instance();

    /*! \brief Идентификатор правила PREP-001. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_PREP001_H