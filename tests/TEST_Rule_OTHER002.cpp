/*!
* \file TEST_Rule_OTHER002.cpp
* \brief DDT-тесты для правила OTHER-002 (раздел 6.89-6.90 тесты_v3.md).
*
* Проверяет правило «good как наречие»:
*  — 6.89: He works good → good→well (ADJ как advmod);
*  — 6.90: He does good → без ошибок (do good, UPOS=NOUN).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other002Expect
* \brief Точечные ожидания для тестов правила OTHER-002.
*/
struct Other002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER002::TEST_Rule_OTHER002() {}
TEST_Rule_OTHER002::~TEST_Rule_OTHER002() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.89-6.90)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other002Expect>("expect");

    // === 6.89 OTHER-002: good как наречие → good→well ==============
    // He works good. good — ADJ как advmod.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He works good"));
        RawToken h = makeRawToken(1, 1, "He", "PRON", 2, "nsubj");
        h.lemma = QStringLiteral("he");
        addToken(s, h);
        RawToken w = makeRawToken(2, 2, "works", "VERB", 0, "root");
        w.lemma = QStringLiteral("work");
        addToken(s, w);
        RawToken g = makeRawToken(3, 3, "good", "ADJ", 2, "advmod");
        g.lemma = QStringLiteral("good");
        addToken(s, g);

        Other002Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-002");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.89_works_good") << s << e;
    }

    // === 6.90 OTHER-002 (исключение): do good — без ошибок =========
    // He does good. good — NOUN как obj, устойчивое выражение.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He does good"));
        RawToken h = makeRawToken(1, 1, "He", "PRON", 2, "nsubj");
        h.lemma = QStringLiteral("he");
        addToken(s, h);
        RawToken d = makeRawToken(2, 2, "does", "VERB", 0, "root");
        d.lemma = QStringLiteral("do");
        addToken(s, d);
        RawToken g = makeRawToken(3, 3, "good", "NOUN", 2, "obj");
        g.lemma = QStringLiteral("good");
        addToken(s, g);

        Other002Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 0;

        QTest::addRow("6.90_does_good") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other002Expect, expect);
    verifyAnchorRule<Other002Expect, Rule_OTHER002>(rawSentence, expect, QString(QTest::currentDataTag()));
}
