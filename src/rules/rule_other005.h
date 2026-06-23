/*!
* \file rule_other005.h
* \brief Объявление правила OTHER-005.
*
* Правило проверяет двойное отрицание: у VERB есть прямой зависимый
* с Polarity=Neg и прямой зависимый-отрицательное местоимение/наречие
* (nothing, nobody, nowhere, never, none, neither, no one). Последнее
* заменяется на не-отрицательную форму.
*/

#ifndef RULE_OTHER005_H
#define RULE_OTHER005_H

#include "datamodel.h"

class Rule_OTHER005 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_OTHER005_H