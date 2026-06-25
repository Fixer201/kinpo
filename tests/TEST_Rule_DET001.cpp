/*!
* \file TEST_Rule_DET001.cpp
* \brief DDT-тесты для правила DET-001 (раздел 6.27-6.33 тесты_v3.md).
*
* Проверяет правило «Несовместимость детерминатива с существительным»:
*  — much + мн.ч. исчисляемое → many;
*  — many + ед.ч. неисчисляемое → much;
*  — a + мн.ч. → -;
*  — this + мн.ч. → these;
*  — few + неисчисляемое → little;
*  — little + мн.ч. исчисляемое → few;
*  — a + ед.ч. неисчисляемое → -.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det001.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Det001Expect
* \brief Точечные ожидания для тестов правила DET-001.
*/
struct Det001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (DET). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Det001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_DET001::TEST_Rule_DET001() {}
TEST_Rule_DET001::~TEST_Rule_DET001() {}

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
        qDebug() << "[TEST_Rule_DET001]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.27-6.33)
// ------------------------------------------------------------------------

void TEST_Rule_DET001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Det001Expect>("expect");

    // === 6.27 DET-001: much + мн.ч. → many ==========================
    // much несовместим с Plur + исчисляемое.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("much books"));
        RawToken much = makeRawToken(1, 1, "much", "DET", 2, "det");
        much.lemma = QStringLiteral("much");
        addToken(s, much);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                      QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.27_much_books") << s << e;
    }

    // === 6.28 DET-001: many + неисчисляемое → much =================
    // many несовместим с неисчисляемым.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("many water"));
        RawToken many = makeRawToken(1, 1, "many", "DET", 2, "det");
        many.lemma = QStringLiteral("many");
        addToken(s, many);
        RawToken water = makeRawToken(2, 2, "water", "NOUN", 0, "root",
                                      QStringLiteral("Number=Sing"));
        water.lemma = QStringLiteral("water");
        addToken(s, water);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.28_many_water") << s << e;
    }

    // === 6.29 DET-001: a + мн.ч. → - ===============================
    // a несовместим с Plur.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a books"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                      QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.29_a_books") << s << e;
    }

    // === 6.30 DET-001: this + мн.ч. → these =======================
    // this несовместим с Plur.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("this books"));
        RawToken thisTok = makeRawToken(1, 1, "this", "DET", 2, "det");
        thisTok.lemma = QStringLiteral("this");
        addToken(s, thisTok);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                      QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.30_this_books") << s << e;
    }

    // === 6.31 DET-001: few + неисчисляемое → little ===============
    // few несовместим с неисчисляемым.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("few water"));
        RawToken few = makeRawToken(1, 1, "few", "DET", 2, "det");
        few.lemma = QStringLiteral("few");
        addToken(s, few);
        RawToken water = makeRawToken(2, 2, "water", "NOUN", 0, "root",
                                      QStringLiteral("Number=Sing"));
        water.lemma = QStringLiteral("water");
        addToken(s, water);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.31_few_water") << s << e;
    }

    // === 6.32 DET-001: little + мн.ч. → few =======================
    // little несовместим с Plur + исчисляемое.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("little books"));
        RawToken little = makeRawToken(1, 1, "little", "DET", 2, "det");
        little.lemma = QStringLiteral("little");
        addToken(s, little);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                      QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.32_little_books") << s << e;
    }

    // === 6.33 DET-001: a + неисчисляемое → - =======================
    // a несовместим с неисчисляемым.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a furniture"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken furniture = makeRawToken(2, 2, "furniture", "NOUN", 0, "root",
                                          QStringLiteral("Number=Sing"));
        furniture.lemma = QStringLiteral("furniture");
        addToken(s, furniture);

        Det001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.33_a_furniture") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_DET001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Det001Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET001 rule;

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