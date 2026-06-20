/*!
* \file TEST_Rule_ART001.cpp
* \brief тесты для правила ART-001.
*
* Проверяет срабатывание и исключения правила "Лишний артикль перед PROPN":
*  — срабатывание на a, an, the перед PROPN
*  — исключение: географические названия
*  — исключение: фамилии во мн.ч.
*  — неисключение: фамилии в ед.ч.
*  — исключение: географические названия во мн.ч.
*  — исключение: классификаторы (compound)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art001.h"

TEST_Rule_ART001::TEST_Rule_ART001() {}
TEST_Rule_ART001::~TEST_Rule_ART001() {}

void TEST_Rule_ART001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<int>>("expectedDisplayIds");
    QTest::addColumn<QSet<int>>("expectedConflictIds");

    // 6.1 — лишний a перед PROPN
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a Europe"));
        addToken(s, makeRawToken(1, 1, "a", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Europe", "PROPN", 0, "root"));
        QTest::addRow("6.1_a_Europe")
            << s << 1
            << QStringLiteral("ART-001")
            << (QList<int>{1})
            << (QSet<int>{1});
    }

    // 6.2 — лишний the перед PROPN
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the London"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "London", "PROPN", 0, "root"));
        QTest::addRow("6.2_the_London")
            << s << 1
            << QStringLiteral("ART-001")
            << (QList<int>{1})
            << (QSet<int>{1});
    }

    // 6.3 — лишний an перед PROPN
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("an Oxford"));
        addToken(s, makeRawToken(1, 1, "an", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Oxford", "PROPN", 0, "root"));
        QTest::addRow("6.3_an_Oxford")
            << s << 1
            << QStringLiteral("ART-001")
            << (QList<int>{1})
            << (QSet<int>{1});
    }

    // 6.4 — исключение: географическое название (the Pacific Ocean)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Pacific Ocean"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        addToken(s, makeRawToken(2, 2, "Pacific", "PROPN", 3, "compound"));
        addToken(s, makeRawToken(3, 3, "Ocean", "PROPN", 0, "root"));
        QTest::addRow("6.4_geo_the_Pacific_Ocean")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.5 — исключение: фамилия во мн.ч. (the Smiths)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Smiths"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Smiths", "PROPN", 0, "root"));
        QTest::addRow("6.5_family_plural_Smiths")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.5a — фамилия в ед.ч. (the Smith) → ошибка
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Smith"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Smith", "PROPN", 0, "root"));
        QTest::addRow("6.5a_family_singular_Smith")
            << s << 1
            << QStringLiteral("ART-001")
            << (QList<int>{1})
            << (QSet<int>{1});
    }

    // 6.5b — исключение: географическое название во мн.ч. (the Alps)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Alps"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Alps", "PROPN", 0, "root"));
        QTest::addRow("6.5b_geo_the_Alps")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.6 — исключение: классификатор (the Sahara desert)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Sahara desert"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        addToken(s, makeRawToken(2, 2, "Sahara", "PROPN", 3, "compound"));
        addToken(s, makeRawToken(3, 3, "desert", "NOUN", 0, "root"));
        QTest::addRow("6.6_classifier_Sahara_desert")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }
}

void TEST_Rule_ART001::TestRule()
{
    // Получаем входные данные
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, anchorTokenId);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<int>, expectedDisplayIds);
    QFETCH(QSet<int>, expectedConflictIds);

    // Текущая метка row (например, "6.1_a_Europe")
    const QString tag = QString(QTest::currentDataTag());

    // Превращаем сырое предложение в аналитическую модель
    // с деревом зависимостей, линейными связями и типизированными полями
    SentenceModel sentence = buildSentenceModel(rawSentence);

    // Ищем токен-якорь по ID.
    TokenNode* anchor = sentence.tokensById.value(anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr, qPrintable(QString("[%1] Якорный токен %2 не найден").arg(tag).arg(anchorTokenId)));

    // DocumentModel и CheckerRuntime пустые, т.к. ART-001 не использует
    // навигацию по документу и runtime-ресурсы
    Rule_ART001 rule;
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), CheckerRuntime());

    // Проверяем результат.
    // expectedRuleId.isEmpty() означает, что ожидается NO ERRORS (правило не срабатывает)
    if (expectedRuleId.isEmpty()) {
        // Никаких ошибок не ожидается кандидатов быть не должно
        QCOMPARE(result.size(), 0);
    } else {
        // тест одного правила на одном якорном токене
        // каждый row в _data() содержит ровно 1 якорь DET;
        // ART-001 либо срабатывает на нём 1 раз (если parent PROPN и нет исключений),
        // либо не срабатывает вообще. Других якорей в предложении нет,
        // поэтому result.size() может быть только 0 или 1
        QCOMPARE(result.size(), 1);
        // Сравниваем ruleId, displayTokenIds и conflictTokenIds через хелпер
        // При несовпадении хелпер выдаст подробное сообщение с tag
        compareSingleCandidate(tag, *result.begin(), expectedRuleId, expectedDisplayIds, expectedConflictIds);
    }
}
