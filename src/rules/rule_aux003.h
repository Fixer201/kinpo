/*!
* \file rule_aux003.h
* \brief Объявление правила AUX-003.
*
* Правило проверяет избыточную частицу to после модального глагола.
*/

#ifndef RULE_AUX003_H
#define RULE_AUX003_H

#include "datamodel.h"

class Rule_AUX003 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX003_H