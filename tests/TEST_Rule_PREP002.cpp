/*!
* \file TEST_Rule_PREP002.cpp
* \brief DDT-тесты для правила PREP-002 (раздел 6.50-6.51 тесты_v3.md).
*
* Проверяет правило «for ↔ since»:
*  — for перед моментом времени (2020) в Perfect → since;
*  — since перед длительностью (two days) → for.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Prep002Expect
* \brief Точечные ожидания для тестов правила PREP-002.
*/
struct Prep002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADP). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Prep002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_PREP002::TEST_Rule_PREP002() {}
TEST_Rule_PREP002::~TEST_Rule_PREP002() {}

// ------------------------------------------------------------------------
// Данные тестов (6.50-6.51)
// ------------------------------------------------------------------------

void TEST_Rule_PREP002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Prep002Expect>("expect");

    // === 6.50 PREP-002: for 2020 → since (момент + Perfect) ========
    // I have lived for 2020 → since.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("I have lived for 2020"));
        RawToken i = makeRawToken(1, 1, "I", "PRON", 3, "nsubj");
        i.lemma = QStringLiteral("I");
        addToken(s, i);
        RawToken have = makeRawToken(2, 2, "have", "AUX", 3, "aux");
        have.lemma = QStringLiteral("have");
        addToken(s, have);
        RawToken lived = makeRawToken(3, 3, "lived", "VERB", 0, "root",
                                      QStringLiteral("VerbForm=Part"));
        lived.lemma = QStringLiteral("live");
        addToken(s, lived);
        RawToken forTok = makeRawToken(4, 4, "for", "ADP", 5, "case");
        forTok.lemma = QStringLiteral("for");
        addToken(s, forTok);
        RawToken year = makeRawToken(5, 5, "2020", "NUM", 3, "nmod:tmod");
        year.lemma = QStringLiteral("2020");
        addToken(s, year);

        Prep002Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-002");
        e.expectedDisplayIds = {4};
        e.expectedConflictIds = {4};

        QTest::addRow("6.50_for_2020") << s << e;
    }

    // === 6.51 PREP-002: since two days → for (длительность) =======
    // He has waited since two days → for.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He has waited since two days"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken has = makeRawToken(2, 2, "has", "AUX", 3, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        RawToken waited = makeRawToken(3, 3, "waited", "VERB", 0, "root",
                                       QStringLiteral("VerbForm=Part"));
        waited.lemma = QStringLiteral("wait");
        addToken(s, waited);
        RawToken since = makeRawToken(4, 4, "since", "ADP", 6, "case");
        since.lemma = QStringLiteral("since");
        addToken(s, since);
        RawToken two = makeRawToken(5, 5, "two", "NUM", 6, "nummod");
        two.lemma = QStringLiteral("two");
        addToken(s, two);
        RawToken days = makeRawToken(6, 6, "days", "NOUN", 3, "nmod:tmod");
        days.lemma = QStringLiteral("day");
        addToken(s, days);

        Prep002Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-002");
        e.expectedDisplayIds = {4};
        e.expectedConflictIds = {4};

        QTest::addRow("6.51_since_two_days") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_PREP002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Prep002Expect, expect);
    verifyAnchorRule<Prep002Expect, Rule_PREP002>(rawSentence, expect, QString(QTest::currentDataTag()));
}
