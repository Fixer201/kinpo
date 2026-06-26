/*!
* \file TEST_Rule_ART001.cpp
* \brief DDT-тесты для правила ART-001 (раздел 6.1-6.6 тесты_v3.md).
*
* Проверяет срабатывание и исключения правила «Лишний артикль перед PROPN»:
*  — срабатывание на a, an, the перед PROPN;
*  — исключение: географические названия (geo_the.txt);
*  — исключение: фамилии во мн.ч.;
*  — неисключение: фамилии в ед.ч.;
*  — исключение: географические названия во мн.ч.;
*  — исключение: классификаторы (compound).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art001.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art001Expect
* \brief Точечные ожидания для тестов правила ART-001.
*/
struct Art001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (DET). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Art001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART001::TEST_Rule_ART001() {}
TEST_Rule_ART001::~TEST_Rule_ART001() {}

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
        qDebug() << "[TEST_Rule_ART001]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.1-6.6)
// ------------------------------------------------------------------------

void TEST_Rule_ART001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art001Expect>("expect");

    // === 6.1 ART-001: лишний артикль a перед PROPN ==================
    // Вход: a Europe. Ожидается: a→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a Europe"));
        addToken(s, makeRawToken(1, 1, "a", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Europe", "PROPN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.1_a_Europe") << s << e;
    }

    // === 6.2 ART-001: лишний артикль the перед PROPN ================
    // Вход: the London. Ожидается: the→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the London"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "London", "PROPN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.2_the_London") << s << e;
    }

    // === 6.3 ART-001: лишний артикль an перед PROPN =================
    // Вход: an Oxford. Ожидается: an→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("an Oxford"));
        addToken(s, makeRawToken(1, 1, "an", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Oxford", "PROPN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.3_an_Oxford") << s << e;
    }

    // === 6.4 ART-001 (исключение): географическое название =========
    // Вход: the Pacific Ocean. Ожидается: NO ERRORS (geo_the.txt).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the Pacific Ocean"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        addToken(s, makeRawToken(2, 2, "Pacific", "PROPN", 3, "compound"));
        addToken(s, makeRawToken(3, 3, "Ocean", "PROPN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.4_geo_the_Pacific_Ocean") << s << e;
    }

    // === 6.5 ART-001 (исключение): фамилия во мн.ч. =================
    // Вход: the Smiths. Ожидается: NO ERRORS (фамилия во мн.ч., Number=Plur).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                         QStringLiteral("the Smiths"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Smiths", "PROPN", 0, "root",
                                 QStringLiteral("Number=Plur")));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.5_family_plural_Smiths") << s << e;
    }

    // === 6.5a ART-001: фамилия в ед.ч. (ошибка) ====================
    // Вход: the Smith. Ожидается: the→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                         QStringLiteral("the Smith"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Smith", "PROPN", 0, "root",
                                 QStringLiteral("Number=Sing")));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.5a_family_singular_Smith") << s << e;
    }

    // === 6.5b ART-001 (исключение): географическое во мн.ч. =========
    // Вход: the Alps. Ожидается: NO ERRORS (geo_the.txt).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the Alps"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "Alps", "PROPN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.5b_geo_the_Alps") << s << e;
    }

    // === 6.6 ART-001 (исключение): классификатор ====================
    // Вход: the Sahara desert. Ожидается: NO ERRORS (классификатор).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the Sahara desert"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        addToken(s, makeRawToken(2, 2, "Sahara", "PROPN", 3, "compound"));
        addToken(s, makeRawToken(3, 3, "desert", "NOUN", 0, "root"));

        Art001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.6_classifier_Sahara_desert") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art001Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART001 rule;

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