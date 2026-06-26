/*!
* \file TEST_Rule_AUX005.cpp
* \brief DDT-тесты для правила AUX-005 (раздел 6.83-6.84 тесты_v3.md).
*
* Проверяет срабатывание правила при пропущенном have после модального
* и при замене have на of. Ожидается вставка have.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux005.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux005Expect
* \brief Точечные ожидания для тестов правила AUX-005.
*/
struct Aux005Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (AUX). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux005Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX005::TEST_Rule_AUX005() {}
TEST_Rule_AUX005::~TEST_Rule_AUX005() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.83-6.84)
// ------------------------------------------------------------------------

void TEST_Rule_AUX005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux005Expect>("expect");

    // === 6.83 AUX-005: пропущен have ================================
    // Вход: must gone. Ожидается: must gone→must have gone.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("must gone"));
        RawToken m = makeRawToken(1, 1, "must", "AUX", 2, "aux");
        m.lemma = QStringLiteral("must");
        addToken(s, m);

        RawToken g = makeRawToken(2, 2, "gone", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        g.featsRaw = QStringLiteral("VerbForm=Part");
        addToken(s, g);

        Aux005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-005");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.83_must_gone") << s << e;
    }

    // === 6.84 AUX-005: of вместо have ===============================
    // Вход: must of gone. Ожидается: must of gone→must have gone.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("must of gone"));
        RawToken m = makeRawToken(1, 1, "must", "AUX", 3, "aux");
        m.lemma = QStringLiteral("must");
        addToken(s, m);

        RawToken o = makeRawToken(2, 2, "of", "ADP", 3, "advmod");
        o.lemma = QStringLiteral("of");
        addToken(s, o);

        RawToken g = makeRawToken(3, 3, "gone", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        g.featsRaw = QStringLiteral("VerbForm=Part");
        addToken(s, g);

        Aux005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-005");
        e.expectedDisplayIds = {1, 2, 3};
        e.expectedConflictIds = {2};

        QTest::addRow("6.84_must_of_gone") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX005::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux005Expect, expect);
    verifyAnchorRule<Aux005Expect, Rule_AUX005>(rawSentence, expect, QString(QTest::currentDataTag()));
}
