/*!
* \file TEST_Rule_AUX006.cpp
* \brief DDT-тесты для правила AUX-006 (раздел 6.85-6.86 тесты_v3.md).
*
* Проверяет срабатывание правила при наличии частицы to после let
* и исключение для пассивной конструкции (was made to clean).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux006.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux006Expect
* \brief Точечные ожидания для тестов правила AUX-006.
*/
struct Aux006Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (PART). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux006Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX006::TEST_Rule_AUX006() {}
TEST_Rule_AUX006::~TEST_Rule_AUX006() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.85-6.86)
// ------------------------------------------------------------------------

void TEST_Rule_AUX006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux006Expect>("expect");

    // === 6.85 AUX-006: to после let =================================
    // Вход: let him to go. Ожидается: to→-, кандидат на токене 3.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("let him to go"));
        RawToken l = makeRawToken(1, 1, "let", "VERB", 0, "root");
        l.lemma = QStringLiteral("let");
        addToken(s, l);

        RawToken h = makeRawToken(2, 2, "him", "PRON", 1, "obj");
        h.lemma = QStringLiteral("he");
        addToken(s, h);

        RawToken t = makeRawToken(3, 3, "to", "PART", 4, "mark");
        t.lemma = QStringLiteral("to");
        addToken(s, t);

        RawToken g = makeRawToken(4, 4, "go", "VERB", 1, "xcomp");
        g.lemma = QStringLiteral("go");
        addToken(s, g);

        Aux006Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-006");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.85_let_him_to_go") << s << e;
    }

    // === 6.86 AUX-006 (исключение): пассив ==========================
    // Вход: He was made to clean. Ожидается: NO ERRORS (пассив, to нужен).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He was made to clean"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj:pass");
        he.lemma = QStringLiteral("he");
        addToken(s, he);

        RawToken was = makeRawToken(2, 2, "was", "AUX", 3, "aux:pass");
        was.lemma = QStringLiteral("be");
        addToken(s, was);

        RawToken made = makeRawToken(3, 3, "made", "VERB", 0, "root");
        made.lemma = QStringLiteral("make");
        addToken(s, made);

        RawToken t = makeRawToken(4, 4, "to", "PART", 5, "mark");
        t.lemma = QStringLiteral("to");
        addToken(s, t);

        RawToken c = makeRawToken(5, 5, "clean", "VERB", 3, "xcomp");
        c.lemma = QStringLiteral("clean");
        addToken(s, c);

        Aux006Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 0;

        QTest::addRow("6.86_made_to_clean_passive") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX006::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux006Expect, expect);
    verifyAnchorRule<Aux006Expect, Rule_AUX006>(rawSentence, expect, QString(QTest::currentDataTag()));
}
