/*!
* \file rule_conj003.h
* \brief Объявление правила CONJ-003.
*
* Правило проверяет although ↔ despite (и in spite of): перед именной
* группой требуется despite/in spite of, перед клаузой — although.
*/

#ifndef RULE_CONJ003_H
#define RULE_CONJ003_H

#include "datamodel.h"

class Rule_CONJ003 : public Rule {
public:
    static const Rule_CONJ003& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ003_H