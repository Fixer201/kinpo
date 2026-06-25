/*!
* \file TEST_Rule_OTHER003.cpp
* \brief DDT-тесты для правила OTHER-003 (раздел 6.91-6.92 тесты_v3.md).
*
* Проверяет правило «Неверный усилитель прилагательного»:
*  — 6.91: much + Pos → much→very;
*  — 6.92: very + Cmp → very→much.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other003Expect
* \brief Точечные ожидания для тестов правила OTHER-003.
*/
struct Other003Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADV). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER003::TEST_Rule_OTHER003() {}
TEST_Rule_OTHER003::~TEST_Rule_OTHER003() {}

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
        qDebug() << "[TEST_Rule_OTHER003]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.91-6.92)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other003Expect>("expect");

    // === 6.91 OTHER-003: much + Pos → much→very ====================
    // much good. good — ADJ с Degree=Pos (по умолчанию).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("much good"));
        RawToken m = makeRawToken(1, 1, "much", "ADV", 2, "advmod");
        m.lemma = QStringLiteral("much");
        addToken(s, m);
        RawToken g = makeRawToken(2, 2, "good", "ADJ", 0, "root");
        g.lemma = QStringLiteral("good");
        addToken(s, g);

        Other003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.91_much_good") << s << e;
    }

    // === 6.92 OTHER-003: very + Cmp → very→much ====================
    // very better. better — ADJ с Degree=Cmp.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("very better"));
        RawToken v = makeRawToken(1, 1, "very", "ADV", 2, "advmod");
        v.lemma = QStringLiteral("very");
        addToken(s, v);
        RawToken b = makeRawToken(2, 2, "better", "ADJ", 0, "root");
        b.lemma = QStringLiteral("good");
        b.featsRaw = QStringLiteral("Degree=Cmp");
        addToken(s, b);

        Other003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.92_very_better") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other003Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_OTHER003 rule;

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