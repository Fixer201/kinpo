/*!
* \file TEST_Rule_DET005.cpp
* \brief DDT-тесты для правила DET-005 (раздел 6.42 тесты_v3.md).
*
* Проверяет правило «Множественное число неисчисляемого»:
*  — informations → information (NOUN, Number=Plur, LEMMA ∈ uncountable.txt).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det005.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Det005Expect
* \brief Точечные ожидания для тестов правила DET-005.
*/
struct Det005Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (NOUN). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Det005Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_DET005::TEST_Rule_DET005() {}
TEST_Rule_DET005::~TEST_Rule_DET005() {}

// ------------------------------------------------------------------------
// Данные тестов (6.42)
// ------------------------------------------------------------------------

void TEST_Rule_DET005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Det005Expect>("expect");

    // === 6.42 DET-005: informations → information =================
    // NOUN во мн. числе, LEMMA=information есть в uncountable.txt.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("informations"));
        RawToken informations = makeRawToken(1, 1, "informations", "NOUN", 0, "root",
                                             QStringLiteral("Number=Plur"));
        informations.lemma = QStringLiteral("information");
        addToken(s, informations);

        Det005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-005");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.42_informations") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_DET005::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Det005Expect, expect);
    verifyAnchorRule<Det005Expect, Rule_DET005>(rawSentence, expect, QString(QTest::currentDataTag()));
}
