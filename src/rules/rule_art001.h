/*!
* \file rule_art001.h
* \brief Объявление правила ART-001: Лишний артикль перед PROPN.
*/

#ifndef RULE_ART001_H
#define RULE_ART001_H

#include "datamodel.h"

/*!
* \class Rule_ART001
* \brief Правило ART-001 — лишний артикль перед именем собственным.
*
* Проверяет, что артикли a, an, the не используются перед PROPN,
* за исключением географических названий, фамилий во мн.ч.
* и классификаторов (compound).
*/
class Rule_ART001 : public Rule {
public:
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_ART001_H
