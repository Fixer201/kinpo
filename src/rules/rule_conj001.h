/*!
* \file rule_conj001.h
* \brief Объявление правила CONJ-001.
*
* Правило обнаруживает союз nor без отрицания в предложении. Если в
* поддереве root нет отрицания (not, never, no, none, Polarity=Neg) и нет
* коррелята neither, nor следует заменить на or.
*/

#ifndef RULE_CONJ001_H
#define RULE_CONJ001_H

#include "datamodel.h"

class Rule_CONJ001 : public Rule {
public:
    static const Rule_CONJ001& instance();

    /*! \brief Идентификатор правила CONJ-001. */
    QString ruleId() const override;
    QSet<Upos> anchorUpos() const override;
    /*! \brief Флаг возможности конфликта с другими правилами. */
    bool canConflict() const override;

    QSet<CandidateError> check(const TokenNode& anchor,
                               int sentenceIndex,
                               const DocumentModel& document,
                               const CheckerRuntime& runtime) const override;
};

#endif // RULE_CONJ001_H