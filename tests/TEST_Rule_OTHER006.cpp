/*!
* \file TEST_Rule_OTHER006.cpp
* \brief DDT-тесты для правила OTHER-006 (раздел 6.98 тесты_v3.md).
*
* Проверяет правило «then вместо than»:
*  — 6.98: better then → then→than (ADV после ADJ с Degree=Cmp).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other006.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other006Expect
* \brief Точечные ожидания для тестов правила OTHER-006.
*/
struct Other006Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other006Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER006::TEST_Rule_OTHER006() {}
TEST_Rule_OTHER006::~TEST_Rule_OTHER006() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.98)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other006Expect>("expect");

    // === 6.98 OTHER-006: better then → then→than ===================
    // better — ADJ с Degree=Cmp, then — ADV как advmod.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("better then"));
        RawToken better = makeRawToken(1, 1, "better", "ADJ", 0, "root");
        better.lemma = QStringLiteral("good");
        better.featsRaw = QStringLiteral("Degree=Cmp");
        addToken(s, better);
        RawToken then = makeRawToken(2, 2, "then", "ADV", 1, "advmod");
        then.lemma = QStringLiteral("then");
        addToken(s, then);

        Other006Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-006");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.98_better_then") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER006::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other006Expect, expect);
    verifyAnchorRule<Other006Expect, Rule_OTHER006>(rawSentence, expect, QString(QTest::currentDataTag()));
}
