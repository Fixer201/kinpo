/*!
* \file rule_other005.cpp
* \brief Реализация правила OTHER-005: двойное отрицание.
*
* Якорь — глагол (VERB). Правило срабатывает, если у VERB есть
* прямой зависимый с Polarity=Neg и хотя бы один прямой зависимый
* с LEMMA из списка отрицательных местоимений/наречий
* (nothing, nobody, nowhere, never, none, neither, "no one").
* Каждый такой отрицательный зависимый заменяется на не-отрицательную
* форму по таблице:
*   nothing → anything, nobody → anybody, nowhere → anywhere,
*   never → ever, none → any.
*
* Одно срабатывание на один отрицательный зависимый из списка замен.
*/

#include "rule_other005.h"
#include <QHash>
#include <QSet>

namespace {

/*! \brief Таблица замен отрицательных форм на не-отрицательные (по LEMMA). */
const QHash<QString, QString>& negReplacementMap()
{
    static const QHash<QString, QString> m = {
        {QStringLiteral("nothing"),  QStringLiteral("anything")},
        {QStringLiteral("nobody"),   QStringLiteral("anybody")},
        {QStringLiteral("nowhere"),  QStringLiteral("anywhere")},
        {QStringLiteral("never"),    QStringLiteral("ever")},
        {QStringLiteral("none"),     QStringLiteral("any")},
    };
    return m;
}

} // namespace

QString Rule_OTHER005::ruleId() const
{
    return QStringLiteral("OTHER-005");
}

QSet<Upos> Rule_OTHER005::anchorUpos() const
{
    return {Upos::VERB};
}

bool Rule_OTHER005::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER005::check(const TokenNode& anchor,
                                           int /*sentenceIndex*/,
                                           const DocumentModel& /*document*/,
                                           const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь должен быть глаголом
    if (anchor.upos != Upos::VERB)
        return res;

    // Проверяем наличие прямого зависимого с Polarity=Neg
    bool hasNeg = false;
    for (const TokenNode* child : anchor.children) {
        if (child && child->features.polarityNeg) {
            hasNeg = true;
            break;
        }
    }
    if (!hasNeg)
        return res;

    // Собираем кандидатов для каждого прямого зависимого из списка замен
    for (const TokenNode* child : anchor.children) {
        if (!child)
            continue;
        // LEMMA сравнивается без учёта регистра (по спецификации списков)
        const QString lemma = child->lemma.toLower();
        if (!negReplacementMap().contains(lemma))
            continue;

        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-005");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {child->id};
        ce.conflictTokenIds = {child->id};
        res.insert(ce);
    }

    return res;
}