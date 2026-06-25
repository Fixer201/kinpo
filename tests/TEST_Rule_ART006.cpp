/*!
* \file TEST_Rule_ART006.cpp
* \brief DDT-тесты для правила ART-006 (раздел 6.21-6.26 тесты_v3.md).
*
* Проверяет правило «Неверный a/an»:
*  — согласный звук (an cat → a);
*  — гласный звук (a apple → an);
*  — mute h (a hour → an);
*  — число (a 18 → an);
*  — согласный /j/ (an university → a);
*  — аббревиатура (a FBI → an).
*
* Для определения звучания используется фонетический словарь CMUdict.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art006.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art006Expect
* \brief Точечные ожидания для тестов правила ART-006.
*/
struct Art006Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (DET). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Art006Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART006::TEST_Rule_ART006() {}
TEST_Rule_ART006::~TEST_Rule_ART006() {}

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
        qDebug() << "[TEST_Rule_ART006]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.21-6.26)
// ------------------------------------------------------------------------

void TEST_Rule_ART006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art006Expect>("expect");

    // === 6.21 ART-006: an cat → a (согласный звук) ==================
    // cat в CMUdict, K — согласная.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("an cat"));
        RawToken an = makeRawToken(1, 1, "an", "DET", 2, "det");
        an.lemma = QStringLiteral("a");
        addToken(s, an);
        RawToken cat = makeRawToken(2, 2, "cat", "NOUN", 0, "root");
        cat.lemma = QStringLiteral("cat");
        addToken(s, cat);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.21_an_cat") << s << e;
    }

    // === 6.22 ART-006: a apple → an (гласный звук) =================
    // apple в CMUdict, AE1 — гласная.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a apple"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken apple = makeRawToken(2, 2, "apple", "NOUN", 0, "root");
        apple.lemma = QStringLiteral("apple");
        addToken(s, apple);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.22_a_apple") << s << e;
    }

    // === 6.23 ART-006: a hour → an (mute h) ========================
    // hour в CMUdict, AW1 — гласная, h немая.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a hour"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken hour = makeRawToken(2, 2, "hour", "NOUN", 0, "root");
        hour.lemma = QStringLiteral("hour");
        addToken(s, hour);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.23_a_hour") << s << e;
    }

    // === 6.24 ART-006: a 18 → an (число) ===========================
    // Число, начинается с «8» — гласный звук.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a 18"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken n18 = makeRawToken(2, 2, "18", "NUM", 0, "root");
        n18.lemma = QStringLiteral("18");
        addToken(s, n18);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.24_a_18") << s << e;
    }

    // === 6.25 ART-006: an university → a (согласный /j/) ===========
    // university в CMUdict, Y — согласный /j/.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("an university"));
        RawToken an = makeRawToken(1, 1, "an", "DET", 2, "det");
        an.lemma = QStringLiteral("a");
        addToken(s, an);
        RawToken uni = makeRawToken(2, 2, "university", "NOUN", 0, "root");
        uni.lemma = QStringLiteral("university");
        addToken(s, uni);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.25_an_university") << s << e;
    }

    // === 6.26 ART-006: a FBI → an (аббревиатура) ===================
    // FBI в CMUdict как fbi, EH1 — гласная.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a FBI"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken fbi = makeRawToken(2, 2, "FBI", "PROPN", 0, "root");
        fbi.lemma = QStringLiteral("FBI");
        addToken(s, fbi);

        Art006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-006");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.26_a_FBI") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART006::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art006Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART006 rule;

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