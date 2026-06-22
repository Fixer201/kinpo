/*!
* \file rule_aux006.h
* \brief Объявление правила AUX-006.
*
* Правило проверяет избыточную частицу to после глаголов let и make
* в активной конструкции с дополнением.
*/

#ifndef RULE_AUX006_H
#define RULE_AUX006_H

#include "datamodel.h"

class Rule_AUX006 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_AUX006_H