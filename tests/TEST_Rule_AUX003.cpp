/*!
* \file TEST_Rule_AUX003.cpp
* \brief DDT-тесты для правила AUX-003 (раздел 6.81 тесты_v3.md).
*
* Проверяет срабатывание правила при наличии частицы to после
* модального глагола. Ожидается удаление лишнего to.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux003Expect
* \brief Точечные ожидания для тестов правила AUX-003.
*/
struct Aux003Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (PART). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX003::TEST_Rule_AUX003() {}
TEST_Rule_AUX003::~TEST_Rule_AUX003() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.81)
// ------------------------------------------------------------------------

void TEST_Rule_AUX003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux003Expect>("expect");

    // === 6.81 AUX-003: to после модального ============================
    // Вход: must to go. Ожидается: to→-, кандидат на токене 2.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("must to go"));
        RawToken m = makeRawToken(1, 1, "must", "AUX", 3, "aux");
        m.lemma = QStringLiteral("must");
        addToken(s, m);

        RawToken t = makeRawToken(2, 2, "to", "PART", 3, "mark");
        t.lemma = QStringLiteral("to");
        addToken(s, t);

        RawToken g = makeRawToken(3, 3, "go", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        addToken(s, g);

        Aux003Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-003");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.81_must_to_go") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux003Expect, expect);
    verifyAnchorRule<Aux003Expect, Rule_AUX003>(rawSentence, expect, QString(QTest::currentDataTag()));
}
