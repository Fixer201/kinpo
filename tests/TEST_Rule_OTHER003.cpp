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
    verifyAnchorRule<Other003Expect, Rule_OTHER003>(rawSentence, expect, QString(QTest::currentDataTag()));
}
