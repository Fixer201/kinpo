/*!
* \file rule_prep004.h
* \brief Объявление правила PREP-004.
*
* Правило обнаруживает конструкцию Perfect + ago: ago несовместимо с
* Present Perfect, требуется Past Simple.
*/

#ifndef RULE_PREP004_H
#define RULE_PREP004_H

#include "datamodel.h"

class Rule_PREP004 : public Rule {
public:
    static const Rule_PREP004& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_PREP004_H