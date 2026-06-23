/*!
* \file rule_aux005.h
* \brief Объявление правила AUX-005.
*
* Правило проверяет пропущенный или искажённый have после модального
* перед причастием.
*/

#ifndef RULE_AUX005_H
#define RULE_AUX005_H

#include "datamodel.h"

class Rule_AUX005 : public Rule {
public:
    static const Rule_AUX005& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX005_H