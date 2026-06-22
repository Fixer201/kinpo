/*!
* \file rule_det005.cpp
* \brief Реализация правила DET-005: Множественное число неисчисляемого.
*
* Срабатывает на NOUN, у которого в FEATS указано Number=Plur, а LEMMA
* найдена в словаре uncountable.txt. Неисчисляемое существительное не
* употребляется во множественном числе, поэтому правильная форма —
* единственное число. Исключений нет.
*/

#include "rule_det005.h"
#include <QSet>

QString Rule_DET005::ruleId() const
{
    return QStringLiteral("DET-005");
}

QSet<Upos> Rule_DET005::anchorUpos() const
{
    // Цель проверки — сам NOUN, а не подчинённый детерминатив
    return {Upos::NOUN};
}

bool Rule_DET005::canConflict() const
{
    // Правило участвует в разрешении приоритетов: один и тот же NOUN
    // может быть ошибочным сразу для нескольких правил
    return true;
}

QSet<CandidateError> Rule_DET005::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Цель — только NOUN
    if (anchor.upos != Upos::NOUN)
        return res;

    // Должно быть явно указано множественное число
    if (!anchor.features.number.has_value() ||
        *anchor.features.number != NumberValue::Plur)
        return res;

    // LEMMA должна быть в словаре неисчисляемых существительных
    const QString lemmaLower = anchor.lemma.toLower();
    if (!runtime.resources.uncountable.contains(lemmaLower))
        return res;

    // Найдена ошибка: неисчисляемое существительное во множественном числе
    CandidateError ce;
    ce.ruleId = QStringLiteral("DET-005");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}