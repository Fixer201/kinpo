/*!
* \file TEST_Rule_CONJ003.cpp
* \brief DDT-тесты для правила CONJ-003 (раздел 6.66, 6.99-6.100 тесты_v3.md).
*
* Проверяет правило «although ↔ despite»:
*  — 6.66: although перед именной группой (the rain) → despite;
*  — 6.99: although перед притяжательным (his mistake) → despite;
*  — 6.100: although перед клаузой (it rained) → без ошибок.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj003Expect
* \brief Точечные ожидания для тестов правила CONJ-003.
*/
struct Conj003Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (SCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ003::TEST_Rule_CONJ003() {}
TEST_Rule_CONJ003::~TEST_Rule_CONJ003() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.66, 6.99-6.100)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj003Expect>("expect");

    // === 6.66 CONJ-003: although the rain → despite (именная группа)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("although the rain"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken rain = makeRawToken(3, 3, "rain", "NOUN", 0, "root");
        rain.lemma = QStringLiteral("rain");
        addToken(s, rain);

        Conj003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.66_although_the_rain") << s << e;
    }

    // === 6.99 CONJ-003: although his mistake → despite (притяжательное)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("although his mistake"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken his = makeRawToken(2, 2, "his", "PRON", 3, "nmod:poss",
                                    QStringLiteral("Poss=Yes"));
        his.lemma = QStringLiteral("he");
        addToken(s, his);
        RawToken mistake = makeRawToken(3, 3, "mistake", "NOUN", 0, "root");
        mistake.lemma = QStringLiteral("mistake");
        addToken(s, mistake);

        Conj003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.99_although_his_mistake") << s << e;
    }

    // === 6.100 CONJ-003 (исключение): although it rained — без ошибок
    // Клауза с глаголом, despite не подходит.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("although it rained"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken it = makeRawToken(2, 2, "it", "PRON", 3, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rained = makeRawToken(3, 3, "rained", "VERB", 0, "root");
        rained.lemma = QStringLiteral("rain");
        addToken(s, rained);

        Conj003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.100_although_it_rained") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj003Expect, expect);
    verifyAnchorRule<Conj003Expect, Rule_CONJ003>(rawSentence, expect, QString(QTest::currentDataTag()));
}
