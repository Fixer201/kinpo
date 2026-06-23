/*!
* \file rule_prep002.h
* \brief Объявление правила PREP-002.
*
* Правило проверяет предлоги for/since: for требует момент времени
* (в конструкции Perfect), since требует длительность.
*/

#ifndef RULE_PREP002_H
#define RULE_PREP002_H

#include "datamodel.h"

class Rule_PREP002 : public Rule {
public:
    static const Rule_PREP002& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_PREP002_H