/*!
* \file rule_conj006.h
* \brief Объявление правила CONJ-006.
*
* Правило проверяет избыточный координатор (so, therefore, but) при
* подчинительном союзе (because/since/as, although/though).
*/

#ifndef RULE_CONJ006_H
#define RULE_CONJ006_H

#include "datamodel.h"

class Rule_CONJ006 : public Rule {
public:
    static const Rule_CONJ006& instance();

    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ006_H