/*!
* \file TEST_Rule_PREP004.cpp
* \brief DDT-тесты для правила PREP-004 (раздел 6.54-6.55 тесты_v3.md).
*
* Проверяет правило «Perfect + ago»:
*  — He has went there two years ago → PREP-004 (ago + Perfect);
*  — has telephoned ago → PREP-004 (правильный глагол, fallback).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Prep004Expect
* \brief Точечные ожидания для тестов правила PREP-004.
*/
struct Prep004Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADV ago). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Prep004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_PREP004::TEST_Rule_PREP004() {}
TEST_Rule_PREP004::~TEST_Rule_PREP004() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {

/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns) {
        qDebug() << "[TEST_Rule_PREP004]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.54-6.55)
// ------------------------------------------------------------------------

void TEST_Rule_PREP004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Prep004Expect>("expect");

    // === 6.54 PREP-004: Perfect + ago (неправильный глагол) ========
    // He has went there two years ago. Якорь — ago (id=7).
    // AUX has (id=2) → went (id=3, VerbForm=Part).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He has went there two years ago"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken has = makeRawToken(2, 2, "has", "AUX", 3, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        RawToken went = makeRawToken(3, 3, "went", "VERB", 0, "root",
                                     QStringLiteral("VerbForm=Part"));
        went.lemma = QStringLiteral("go");
        addToken(s, went);
        RawToken there = makeRawToken(4, 4, "there", "ADV", 3, "advmod");
        there.lemma = QStringLiteral("there");
        addToken(s, there);
        RawToken two = makeRawToken(5, 5, "two", "NUM", 6, "nummod");
        two.lemma = QStringLiteral("two");
        addToken(s, two);
        RawToken years = makeRawToken(6, 6, "years", "NOUN", 7, "nmod:tmod");
        years.lemma = QStringLiteral("year");
        addToken(s, years);
        RawToken ago = makeRawToken(7, 7, "ago", "ADV", 3, "advmod");
        ago.lemma = QStringLiteral("ago");
        addToken(s, ago);

        Prep004Expect e;
        e.anchorTokenId = 7;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-004");
        e.expectedDisplayIds = {2, 3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.54_has_went_ago") << s << e;
    }

    // === 6.55 PREP-004: Perfect + ago (правильный глагол, fallback) =
    // has telephoned ago. Якорь — ago (id=3).
    // AUX has (id=1) → telephoned (id=2, VerbForm=Part).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("has telephoned ago"));
        RawToken has = makeRawToken(1, 1, "has", "AUX", 2, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        RawToken telephoned = makeRawToken(2, 2, "telephoned", "VERB", 0, "root",
                                           QStringLiteral("VerbForm=Part"));
        telephoned.lemma = QStringLiteral("telephone");
        addToken(s, telephoned);
        RawToken ago = makeRawToken(3, 3, "ago", "ADV", 2, "advmod");
        ago.lemma = QStringLiteral("ago");
        addToken(s, ago);

        Prep004Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-004");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.55_has_telephoned_ago") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_PREP004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Prep004Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_PREP004 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество кандидатов: ожидалось =" << expect.expectedCount
                     << "получено =" << actualCount;
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }

    if (expect.expectedCount == 0) {
        return;
    }

    if (!result.isEmpty()) {
        compareSingleCandidate(tag, *result.begin(),
                               expect.expectedRuleId,
                               expect.expectedDisplayIds,
                               expect.expectedConflictIds);
    }
}