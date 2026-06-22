/*!
* \file rule_conj002.h
* \brief Объявление правила CONJ-002.
*
* Правило проверяет because ↔ because of: before именной группой
* требуется because of, перед клаузой — because.
*/

#ifndef RULE_CONJ002_H
#define RULE_CONJ002_H

#include "datamodel.h"

class Rule_CONJ002 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ002_H