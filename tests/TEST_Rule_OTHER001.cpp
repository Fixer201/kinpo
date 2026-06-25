/*!
* \file TEST_Rule_OTHER001.cpp
* \brief DDT-тесты для правила OTHER-001 (раздел 6.87-6.88 тесты_v3.md).
*
* Проверяет правило «Двойное сравнение»:
*  — 6.87: more + Cmp → more удаляется;
*  — 6.88: most + Sup → most удаляется.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other001.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other001Expect
* \brief Точечные ожидания для тестов правила OTHER-001.
*/
struct Other001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADV). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER001::TEST_Rule_OTHER001() {}
TEST_Rule_OTHER001::~TEST_Rule_OTHER001() {}

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
        qDebug() << "[TEST_Rule_OTHER001]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.87-6.88)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other001Expect>("expect");

    // === 6.87 OTHER-001: more + Cmp → more удаляется ===============
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("more better"));
        RawToken m = makeRawToken(1, 1, "more", "ADV", 2, "advmod");
        m.lemma = QStringLiteral("more");
        addToken(s, m);
        RawToken b = makeRawToken(2, 2, "better", "ADJ", 0, "root");
        b.lemma = QStringLiteral("good");
        b.featsRaw = QStringLiteral("Degree=Cmp");
        addToken(s, b);

        Other001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.87_more_better") << s << e;
    }

    // === 6.88 OTHER-001: most + Sup → most удаляется ===============
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("most best"));
        RawToken m = makeRawToken(1, 1, "most", "ADV", 2, "advmod");
        m.lemma = QStringLiteral("most");
        addToken(s, m);
        RawToken b = makeRawToken(2, 2, "best", "ADJ", 0, "root");
        b.lemma = QStringLiteral("good");
        b.featsRaw = QStringLiteral("Degree=Sup");
        addToken(s, b);

        Other001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.88_most_best") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other001Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_OTHER001 rule;

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