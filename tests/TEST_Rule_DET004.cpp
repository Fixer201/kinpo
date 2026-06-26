/*!
* \file TEST_Rule_DET004.cpp
* \brief DDT-тесты для правила DET-004 (раздел 6.40-6.41 тесты_v3.md).
*
* Проверяет правило «Омофон в притяжательной позиции»:
*  — it's → its (it's PRON, nmod:poss, HEAD=NOUN);
*  — there → their (there ADV, det, HEAD=NOUN).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Det004Expect
* \brief Точечные ожидания для тестов правила DET-004.
*/
struct Det004Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (PRON/DET/ADV). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Det004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_DET004::TEST_Rule_DET004() {}
TEST_Rule_DET004::~TEST_Rule_DET004() {}

// ------------------------------------------------------------------------
// Данные тестов (6.40-6.41)
// ------------------------------------------------------------------------

void TEST_Rule_DET004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Det004Expect>("expect");

    // === 6.40 DET-004: it's → its ==================================
    // it's PRON, nmod:poss, HEAD=tail NOUN.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("it's tail"));
        RawToken its = makeRawToken(1, 1, "it's", "PRON", 2, "nmod:poss");
        its.lemma = QStringLiteral("it's");
        addToken(s, its);
        RawToken tail = makeRawToken(2, 2, "tail", "NOUN", 0, "root");
        tail.lemma = QStringLiteral("tail");
        addToken(s, tail);

        Det004Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-004");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.40_its_tail") << s << e;
    }

    // === 6.41 DET-004: there → their ===============================
    // there ADV, det, HEAD=house NOUN.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("there house"));
        RawToken there = makeRawToken(1, 1, "there", "ADV", 2, "det");
        there.lemma = QStringLiteral("there");
        addToken(s, there);
        RawToken house = makeRawToken(2, 2, "house", "NOUN", 0, "root");
        house.lemma = QStringLiteral("house");
        addToken(s, house);

        Det004Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-004");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.41_there_house") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_DET004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Det004Expect, expect);
    verifyAnchorRule<Det004Expect, Rule_DET004>(rawSentence, expect, QString(QTest::currentDataTag()));
}
