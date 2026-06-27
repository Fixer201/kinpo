/*!
* \file rule_art002.h
* \brief Объявление правил ART-002 и ART-002a.
*/

#ifndef RULE_ART002_H
#define RULE_ART002_H

#include "datamodel.h"

class Rule_ART002 : public Rule {
public:
    static const Rule_ART002& instance();

    /*! \brief Идентификатор правила ART-002. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

class Rule_ART002a : public Rule {
public:
    /*! \brief Идентификатор правила ART-002a. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART002_H