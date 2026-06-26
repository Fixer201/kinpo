/*!
* \file TEST_Rule_AUX002.cpp
* \brief DDT-тесты для правила AUX-002 (раздел 6.79-6.80 тесты_v3.md).
*
* Проверяет срабатывание правила при неверной форме глагола после
* вспомогательного did/have. Ожидается замена на правильную форму.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux002Expect
* \brief Точечные ожидания для тестов правила AUX-002.
*/
struct Aux002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (AUX). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX002::TEST_Rule_AUX002() {}
TEST_Rule_AUX002::~TEST_Rule_AUX002() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.79-6.80)
// ------------------------------------------------------------------------

void TEST_Rule_AUX002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux002Expect>("expect");

    // === 6.79 AUX-002: did + неправильная форма ======================
    // Вход: did went. Ожидается: did went→did go, кандидат на токенах 1,2.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("did went"));
        RawToken d = makeRawToken(1, 1, "did", "AUX", 2, "aux");
        d.lemma = QStringLiteral("do");
        addToken(s, d);

        RawToken w = makeRawToken(2, 2, "went", "VERB", 0, "root");
        w.lemma = QStringLiteral("go");
        w.featsRaw = QStringLiteral("VerbForm=Past|Tense=Past");
        addToken(s, w);

        Aux002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.79_did_went") << s << e;
    }

    // === 6.80 AUX-002: have + неправильная форма =====================
    // Вход: have saw. Ожидается: have saw→have seen, кандидат на токенах 1,2.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("have saw"));
        RawToken h = makeRawToken(1, 1, "have", "AUX", 2, "aux");
        h.lemma = QStringLiteral("have");
        addToken(s, h);

        RawToken sw = makeRawToken(2, 2, "saw", "VERB", 0, "root");
        sw.lemma = QStringLiteral("see");
        sw.featsRaw = QStringLiteral("VerbForm=Past|Tense=Past");
        addToken(s, sw);

        Aux002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.80_have_saw") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux002Expect, expect);
    verifyAnchorRule<Aux002Expect, Rule_AUX002>(rawSentence, expect, QString(QTest::currentDataTag()));
}
