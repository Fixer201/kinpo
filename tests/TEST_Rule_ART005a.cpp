/*!
* \file TEST_Rule_ART005a.cpp
* \brief DDT-тесты для правила ART-005a (раздел 6.18-6.20 тесты_v3.md).
*
* Проверяет правило «Лишний the перед титулом + имя»:
*  — the перед титулом + PROPN (nmod:desc);
*  — исключение: Reverend / Honorable;
*  — описание, не титул (nmod:desc, не в titles.txt) — не срабатывает.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART005a.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art005a.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art005aExpect
* \brief Точечные ожидания для тестов правила ART-005a.
*/
struct Art005aExpect {
    int anchorTokenId = -1;        ///< ID токена-якоря (NOUN). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Art005aExpect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART005a::TEST_Rule_ART005a() {}
TEST_Rule_ART005a::~TEST_Rule_ART005a() {}

// ------------------------------------------------------------------------
// Данные тестов (6.18-6.20)
// ------------------------------------------------------------------------

void TEST_Rule_ART005a::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art005aExpect>("expect");

    // === 6.18 ART-005a: the перед титулом + PROPN ===================
    // Вход: the President Obama. Ожидается: the→-, кандидат на токене 1.
    // President — NOUN, nmod:desc к Obama, в titles.txt → ошибка.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the President Obama"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken president = makeRawToken(2, 2, "President", "NOUN", 3, "nmod:desc");
        president.lemma = QStringLiteral("President");
        addToken(s, president);
        RawToken obama = makeRawToken(3, 3, "Obama", "PROPN", 0, "root");
        obama.lemma = QStringLiteral("Obama");
        addToken(s, obama);

        Art005aExpect e;
        e.anchorTokenId = 2;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-005a");
        e.expectedDisplayIds = {1, 2, 3};
        e.expectedConflictIds = {1};

        QTest::addRow("6.18_the_President_Obama") << s << e;
    }

    // === 6.19 ART-005a (исключение): Reverend ======================
    // Вход: the Reverend King. Ожидается: NO ERRORS (Reverend в titles).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the Reverend King"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken reverend = makeRawToken(2, 2, "Reverend", "NOUN", 3, "nmod:desc");
        reverend.lemma = QStringLiteral("Reverend");
        addToken(s, reverend);
        RawToken king = makeRawToken(3, 3, "King", "PROPN", 0, "root");
        king.lemma = QStringLiteral("King");
        addToken(s, king);

        Art005aExpect e;
        e.anchorTokenId = 2;
        e.expectedCount = 0;

        QTest::addRow("6.19_the_Reverend_King") << s << e;
    }

    // === 6.20 ART-005a (исключение): описание, не титул ============
    // Вход: the actor Brad Pitt. Ожидается: NO ERRORS (actor — не титул).
    // actor — NOUN с appos к Pitt, не в titles.txt.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the actor Brad Pitt"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken actor = makeRawToken(2, 2, "actor", "NOUN", 4, "appos");
        actor.lemma = QStringLiteral("actor");
        addToken(s, actor);
        RawToken brad = makeRawToken(3, 3, "Brad", "PROPN", 4, "flat:name");
        brad.lemma = QStringLiteral("Brad");
        addToken(s, brad);
        RawToken pitt = makeRawToken(4, 4, "Pitt", "PROPN", 0, "root");
        pitt.lemma = QStringLiteral("Pitt");
        addToken(s, pitt);

        Art005aExpect e;
        e.anchorTokenId = 2;
        e.expectedCount = 0;

        QTest::addRow("6.20_the_actor_Brad_Pitt") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART005a::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art005aExpect, expect);
    verifyAnchorRule<Art005aExpect, Rule_ART005a>(rawSentence, expect, QString(QTest::currentDataTag()));
}
