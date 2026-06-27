/*!
* \file rule_other004.h
* \brief Объявление правила OTHER-004.
*
* Правило проверяет неверный падеж местоимения: объектная форма
* в позиции подлежащего и именительная форма в позиции дополнения.
*/

#ifndef RULE_OTHER004_H
#define RULE_OTHER004_H

#include "datamodel.h"

class Rule_OTHER004 : public Rule {
public:
    static const Rule_OTHER004& instance();

    /*! \brief Идентификатор правила OTHER-004. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER004_H