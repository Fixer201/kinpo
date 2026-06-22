/*!
* \file rule_prep002.cpp
* \brief Реализация правила PREP-002: for ↔ since.
*
* Ветка (а): for → since. P с LEMMA=for, DEPREL=case; HEAD(P)=N — момент
* времени (год вида \d{4}+NUM, день недели/месяц из time_units.txt, или
* NOUN с зависимым NumType=Ord). В предложении есть AUX с LEMMA=have,
* DEPREL=aux к VERB с VerbForm=Part → since.
* Ветка (б): since → for. P с LEMMA=since, DEPREL=case; HEAD(P)=N; у N есть
* зависимый UPOS=NUM; N.LEMMA ∈ durations.txt → for. Ошибочно в любом
* контексте, AUX не требуется.
*/

#include "rule_prep002.h"
#include <QSet>
#include <QRegularExpression>

QString Rule_PREP002::ruleId() const
{
    return QStringLiteral("PREP-002");
}

QSet<Upos> Rule_PREP002::anchorUpos() const
{
    return {Upos::ADP};
}

bool Rule_PREP002::canConflict() const
{
    // Один и тот же предлог может быть ошибочным для нескольких правил
    return true;
}

namespace {

// Паттерн года вида 2020
const QRegularExpression yearPattern(QStringLiteral("^\\d{4}$"));

/*!
* \brief Проверить, является ли N моментом времени (для ветки а).
* \param [in] n Токен HEAD предлога.
* \param [in] runtime Runtime со словарями.
* \return true если N — момент времени.
*
* Момент времени: год (NUM и FORM вида \d{4}), день недели или месяц
* из time_units.txt, или NOUN с зависимым NumType=Ord (дата).
*/
bool isTimeMoment(const TokenNode& n, const CheckerRuntime& runtime)
{
    const QString lemmaLower = n.lemma.toLower();

    // Год: NUM и FORM из 4 цифр
    if (n.upos == Upos::NUM && yearPattern.match(n.form).hasMatch())
        return true;

    // День недели или месяц из time_units.txt
    if (runtime.resources.timeUnits.contains(lemmaLower))
        return true;

    // Дата: NOUN с зависимым NumType=Ord
    if (n.upos == Upos::NOUN) {
        for (const TokenNode* child : n.children)
            if (child->features.numTypeOrd)
                return true;
    }

    return false;
}

/*!
* \brief Проверить, есть ли в предложении AUX have → VERB с VerbForm=Part.
* \param [in] root Корневой токен предложения.
* \return true если найдена конструкция Perfect (AUX have + VERB Part).
*
* Обходит дерево от root, ищет AUX с LEMMA=have и DEPREL=aux, у которого
* HEAD — VERB с VerbForm=Part.
*/
bool hasPerfectConstruction(const TokenNode& root)
{
    // Рекурсивный обход: проверяем текущий токен и всех потомков
    for (const TokenNode* child : root.children) {
        if (child->upos == Upos::AUX &&
            child->lemma.toLower() == QStringLiteral("have") &&
            child->deprel == Deprel::Aux) {
            // HEAD AUX должен быть VERB с VerbForm=Part
            if (child->parent &&
                child->parent->upos == Upos::VERB &&
                child->parent->features.verbForm.has_value() &&
                *child->parent->features.verbForm == VerbFormValue::Part)
                return true;
        }
        if (hasPerfectConstruction(*child))
            return true;
    }
    return false;
}

/*!
* \brief Найти корень предложения, поднимаясь от anchor к родителям.
* \param [in] anchor Стартовый токен.
* \return Корневой токен (с deprel=Root или без parent).
*/
const TokenNode* findSentenceRoot(const TokenNode& anchor)
{
    const TokenNode* node = &anchor;
    while (node->parent)
        node = node->parent;
    return node;
}

/*!
* \brief Проверить ветку (а): for → since.
* \param [in] anchor Предлог for.
* \param [in] root Корень предложения.
* \param [in] runtime Runtime со словарями.
* \return true если условие ветки (а) выполнено.
*/
bool checkForToSince(const TokenNode& anchor,
                     const TokenNode& root,
                     const CheckerRuntime& runtime)
{
    if (!anchor.parent)
        return false;

    const TokenNode& n = *anchor.parent;

    // N должен быть моментом времени
    if (!isTimeMoment(n, runtime))
        return false;

    // В предложении должна быть конструкция Perfect (AUX have + VERB Part)
    if (!hasPerfectConstruction(root))
        return false;

    return true;
}

/*!
* \brief Проверить ветку (б): since → for.
* \param [in] anchor Предлог since.
* \param [in] runtime Runtime со словарями.
* \return true если условие ветки (б) выполнено.
*
* Ошибочно в любом контексте: AUX have не требуется.
*/
bool checkSinceToFor(const TokenNode& anchor,
                     const CheckerRuntime& runtime)
{
    if (!anchor.parent)
        return false;

    const TokenNode& n = *anchor.parent;

    // N.LEMMA должна быть единицей длительности
    if (!runtime.resources.durations.contains(n.lemma.toLower()))
        return false;

    // У N должен быть зависимый UPOS=NUM
    for (const TokenNode* child : n.children)
        if (child->upos == Upos::NUM)
            return true;

    return false;
}

} // namespace

QSet<CandidateError> Rule_PREP002::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Якорь — ADP
    if (anchor.upos != Upos::ADP)
        return res;

    // DEPREL должен быть case
    if (anchor.deprel != Deprel::Case)
        return res;

    const QString formLower = anchor.lemma.toLower();
    bool error = false;

    if (formLower == QStringLiteral("for")) {
        // Ветка (а): for → since
        const TokenNode* root = findSentenceRoot(anchor);
        error = checkForToSince(anchor, *root, runtime);
    } else if (formLower == QStringLiteral("since")) {
        // Ветка (б): since → for
        error = checkSinceToFor(anchor, runtime);
    } else {
        return res;
    }

    if (!error)
        return res;

    // Найдена ошибка: предлог не соответствует типу временного выражения
    CandidateError ce;
    ce.ruleId = QStringLiteral("PREP-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}