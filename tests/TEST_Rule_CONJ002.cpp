/*!
* \file TEST_Rule_CONJ002.cpp
* \brief DDT-тесты для правила CONJ-002 (раздел 6.64-6.65 тесты_v3.md).
*
* Проверяет правило «because ↔ because of»:
*  — because перед именной группой (the rain) → because of;
*  — because of перед клаузой (it rains) → because.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj002Expect
* \brief Точечные ожидания для тестов правила CONJ-002.
*/
struct Conj002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (SCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ002::TEST_Rule_CONJ002() {}
TEST_Rule_CONJ002::~TEST_Rule_CONJ002() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.64-6.65)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj002Expect>("expect");

    // === 6.64 CONJ-002: because the rain → because of (именная группа)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("because the rain"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 3, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken rain = makeRawToken(3, 3, "rain", "NOUN", 0, "root");
        rain.lemma = QStringLiteral("rain");
        addToken(s, rain);

        Conj002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-002");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.64_because_the_rain") << s << e;
    }

    // === 6.65 CONJ-002: because of it rains → because (клауза) =====
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("because of it rains"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 4, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 1, "fixed");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 4, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rains = makeRawToken(4, 4, "rains", "VERB", 0, "root");
        rains.lemma = QStringLiteral("rain");
        addToken(s, rains);

        Conj002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {1, 2};

        QTest::addRow("6.65_because_of_it_rains") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj002Expect, expect);
    verifyAnchorRule<Conj002Expect, Rule_CONJ002>(rawSentence, expect, QString(QTest::currentDataTag()));
}
