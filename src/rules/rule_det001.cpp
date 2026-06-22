/*!
* \file rule_det001.cpp
* \brief Реализация правила DET-001: Несовместимость det/quant + NOUN.
*
* Срабатывает на DET (или ADV с advmod), LEMMA которого есть в det_compat.txt,
* если HEAD — NOUN, и его Number/countability несовместимы с записью таблицы.
* Исключение для a/an: не срабатывает, если у N есть зависимый с DEPREL=amod.
*/

#include "rule_det001.h"
#include <QSet>

QString Rule_DET001::ruleId() const
{
    return QStringLiteral("DET-001");
}

QSet<Upos> Rule_DET001::anchorUpos() const
{
    return {Upos::DET, Upos::ADV};
}

bool Rule_DET001::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Определить число существительного.
* \param [in] noun Токен NOUN.
* \return NumberCondition::Sing, Plur или Any (если неизвестно).
*/
NumberCondition nounNumber(const TokenNode& noun)
{
    if (noun.features.number.has_value()) {
        if (*noun.features.number == NumberValue::Plur)
            return NumberCondition::Plur;
        if (*noun.features.number == NumberValue::Sing)
            return NumberCondition::Sing;
    }
    return NumberCondition::Any;
}

/*!
* \brief Определить исчисляемость существительного.
* \param [in] noun Токен NOUN.
* \param [in] runtime Runtime со словарями.
* \return CountabilityCondition::Uncountable или Countable.
*/
CountabilityCondition nounCountability(const TokenNode& noun,
                                        const CheckerRuntime& runtime)
{
    if (runtime.resources.uncountable.contains(noun.lemma.toLower()))
        return CountabilityCondition::Uncountable;
    return CountabilityCondition::Countable;
}

/*!
* \brief Проверить, соответствует ли NOUN условиям записи det_compat.
* \param [in] entry Запись таблицы совместимости.
* \param [in] number Число NOUN.
* \param [in] countability Исчисляемость NOUN.
* \return true если NOUN попадает под условия записи (несовместимость).
*/
bool matchesEntry(const DetCompatEntry& entry,
                  NumberCondition number,
                  CountabilityCondition countability)
{
    if (entry.number != NumberCondition::Any && entry.number != number)
        return false;
    if (entry.countability != CountabilityCondition::Any &&
        entry.countability != countability)
        return false;
    return true;
}

/*!
* \brief Проверить, есть ли у NOUN зависимый с DEPREL=amod.
* \param [in] noun Токен NOUN.
* \return true если найден зависимый с amod.
*/
bool hasAmodChild(const TokenNode& noun)
{
    for (const TokenNode* child : noun.children)
        if (child->deprel == Deprel::Amod)
            return true;
    return false;
}

} // namespace

QSet<CandidateError> Rule_DET001::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Условие: DEPREL ∈ {det, advmod}
    if (anchor.deprel != Deprel::Det && anchor.deprel != Deprel::Advmod)
        return res;

    // LEMMA должна быть в det_compat.txt
    const QString lemmaLower = anchor.lemma.toLower();
    auto it = runtime.resources.detCompat.find(lemmaLower);
    if (it == runtime.resources.detCompat.end())
        return res;

    // HEAD должен быть NOUN
    if (!anchor.parent || anchor.parent->upos != Upos::NOUN)
        return res;

    const TokenNode& noun = *anchor.parent;
    const NumberCondition number = nounNumber(noun);
    const CountabilityCondition countability = nounCountability(noun, runtime);

    // Если Number неизвестен — не можем проверить условия
    if (number == NumberCondition::Any)
        return res;

    // Ищем запись, условиям которой соответствует NOUN (это несовместимость)
    QString correction;
    for (const DetCompatEntry& entry : it.value()) {
        if (matchesEntry(entry, number, countability)) {
            correction = entry.correction;
            break;
        }
    }

    if (correction.isEmpty())
        return res;

    // Исключение для a/an: не срабатывает, если у N есть amod
    if ((lemmaLower == QStringLiteral("a") || lemmaLower == QStringLiteral("an")) &&
        hasAmodChild(noun))
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("DET-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}