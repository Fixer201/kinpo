/*!
* \file rule_prep005.h
* \brief Объявление правила PREP-005.
*
* Правило проверяет предлоги при глаголе: лишний, пропущенный или неверный
* предлог по таблице глагольного управления verb_prep.txt.
*/

#ifndef RULE_PREP005_H
#define RULE_PREP005_H

#include "datamodel.h"

class Rule_PREP005 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_PREP005_H