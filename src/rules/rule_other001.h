/*!
* \file rule_other001.h
* \brief Объявление правила OTHER-001.
*
* Правило проверяет двойное сравнение: more/most при синтетической
* степени прилагательного (Cmp/Sup).
*/

#ifndef RULE_OTHER001_H
#define RULE_OTHER001_H

#include "datamodel.h"

class Rule_OTHER001 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER001_H