/*!
* \file TEST_Rule_PREP001.cpp
* \brief DDT-тесты для правила PREP-001 (раздел 6.43-6.49 тесты_v3.md).
*
* Проверяет правило «Неверный временной предлог»:
*  — день недели (in Monday → on);
*  — месяц (on July → in);
*  — часть дня (at morning → in);
*  — год (on 2020 → in);
*  — ночь (in night → at);
*  — корректный предлог не срабатывает (at 5 o'clock — без ошибок);
*  — исключение compound + день недели (on Monday morning — без ошибок).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep001.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Prep001Expect
* \brief Точечные ожидания для тестов правила PREP-001.
*/
struct Prep001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADP). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Prep001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_PREP001::TEST_Rule_PREP001() {}
TEST_Rule_PREP001::~TEST_Rule_PREP001() {}

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
        qDebug() << "[TEST_Rule_PREP001]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.43-6.49)
// ------------------------------------------------------------------------

void TEST_Rule_PREP001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Prep001Expect>("expect");

    // === 6.43 PREP-001: in Monday → on (день недели) ===============
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("in Monday"));
        RawToken in = makeRawToken(1, 1, "in", "ADP", 2, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken monday = makeRawToken(2, 2, "Monday", "PROPN", 0, "root");
        monday.lemma = QStringLiteral("Monday");
        addToken(s, monday);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.43_in_Monday") << s << e;
    }

    // === 6.44 PREP-001: on July → in (месяц) =======================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("on July"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 2, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken july = makeRawToken(2, 2, "July", "PROPN", 0, "root");
        july.lemma = QStringLiteral("July");
        addToken(s, july);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.44_on_July") << s << e;
    }

    // === 6.45 PREP-001: at morning → in (часть дня) ================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("at morning"));
        RawToken at = makeRawToken(1, 1, "at", "ADP", 2, "case");
        at.lemma = QStringLiteral("at");
        addToken(s, at);
        RawToken morning = makeRawToken(2, 2, "morning", "NOUN", 0, "root");
        morning.lemma = QStringLiteral("morning");
        addToken(s, morning);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.45_at_morning") << s << e;
    }

    // === 6.46 PREP-001: on 2020 → in (год) =========================
    // NUM с FORM из 4 цифр.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("on 2020"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 2, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken year = makeRawToken(2, 2, "2020", "NUM", 0, "root");
        year.lemma = QStringLiteral("2020");
        addToken(s, year);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.46_on_2020") << s << e;
    }

    // === 6.47 PREP-001: in night → at (ночь) =======================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("in night"));
        RawToken in = makeRawToken(1, 1, "in", "ADP", 2, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken night = makeRawToken(2, 2, "night", "NOUN", 0, "root");
        night.lemma = QStringLiteral("night");
        addToken(s, night);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.47_in_night") << s << e;
    }

    // === 6.48 PREP-001 (negative): at 5 o'clock — без ошибок ======
    // Конкретное время, at корректен.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("at 5 o'clock"));
        RawToken at = makeRawToken(1, 1, "at", "ADP", 2, "case");
        at.lemma = QStringLiteral("at");
        addToken(s, at);
        RawToken five = makeRawToken(2, 2, "5", "NUM", 3, "nummod");
        five.lemma = QStringLiteral("5");
        addToken(s, five);
        RawToken oclock = makeRawToken(3, 3, "o'clock", "NOUN", 0, "root");
        oclock.lemma = QStringLiteral("o'clock");
        addToken(s, oclock);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.48_at_5_oclock") << s << e;
    }

    // === 6.49 PREP-001 (исключение): on Monday morning — без ошибок
    // Compound с днём недели перебивает правило.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("on Monday morning"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 3, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken monday = makeRawToken(2, 2, "Monday", "PROPN", 3, "compound");
        monday.lemma = QStringLiteral("Monday");
        addToken(s, monday);
        RawToken morning = makeRawToken(3, 3, "morning", "NOUN", 0, "root");
        morning.lemma = QStringLiteral("morning");
        addToken(s, morning);

        Prep001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.49_on_Monday_morning") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_PREP001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Prep001Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_PREP001 rule;

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