/*!
* \file rule_aux002.h
* \brief Объявление правила AUX-002.
*
* Правило проверяет неверную форму глагола после вспомогательного
* do (ожидается инфинитив) или have (ожидается Past Participle).
*/

#ifndef RULE_AUX002_H
#define RULE_AUX002_H

#include "datamodel.h"

class Rule_AUX002 : public Rule {
public:
    static const Rule_AUX002& instance();

    /*! \brief Идентификатор правила AUX-002. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX002_H