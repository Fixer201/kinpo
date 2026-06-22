/*!
* \file rule_aux001.h
* \brief Объявление правила AUX-001.
*
* Правило проверяет избыточный вспомогательный do при наличии
* модального глагола у того же VERB.
*/

#ifndef RULE_AUX001_H
#define RULE_AUX001_H

#include "datamodel.h"

class Rule_AUX001 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX001_H