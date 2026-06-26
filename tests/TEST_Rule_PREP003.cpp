/*!
* \file TEST_Rule_PREP003.cpp
* \brief DDT-тесты для правила PREP-003 (раздел 6.52-6.53 тесты_v3.md).
*
* Проверяет правило «while ↔ during»:
*  — while перед именной группой (meeting/NOUN) → during;
*  — while перед клаузой (rained/VERB с зависимым nsubj) → без ошибок.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Prep003Expect
* \brief Точечные ожидания для тестов правила PREP-003.
*/
struct Prep003Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (SCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Prep003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_PREP003::TEST_Rule_PREP003() {}
TEST_Rule_PREP003::~TEST_Rule_PREP003() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.52-6.53)
// ------------------------------------------------------------------------

void TEST_Rule_PREP003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Prep003Expect>("expect");

    // === 6.52 PREP-003: while the meeting → during (именная группа)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("while the meeting"));
        RawToken whileTok = makeRawToken(1, 1, "while", "SCONJ", 3, "mark");
        whileTok.lemma = QStringLiteral("while");
        addToken(s, whileTok);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken meeting = makeRawToken(3, 3, "meeting", "NOUN", 0, "root");
        meeting.lemma = QStringLiteral("meeting");
        addToken(s, meeting);

        Prep003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.52_while_the_meeting") << s << e;
    }

    // === 6.53 PREP-003 (исключение): while it rained — без ошибок ==
    // Клауза, VERB в зависимых.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("while it rained"));
        RawToken whileTok = makeRawToken(1, 1, "while", "SCONJ", 3, "mark");
        whileTok.lemma = QStringLiteral("while");
        addToken(s, whileTok);
        RawToken it = makeRawToken(2, 2, "it", "PRON", 3, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rained = makeRawToken(3, 3, "rained", "VERB", 0, "root");
        rained.lemma = QStringLiteral("rain");
        addToken(s, rained);

        Prep003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.53_while_it_rained") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_PREP003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Prep003Expect, expect);
    verifyAnchorRule<Prep003Expect, Rule_PREP003>(rawSentence, expect, QString(QTest::currentDataTag()));
}
