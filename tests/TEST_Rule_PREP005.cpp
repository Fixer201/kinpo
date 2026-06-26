/*!
* \file TEST_Rule_PREP005.cpp
* \brief DDT-тесты для правила PREP-005 (раздел 6.56-6.59 тесты_v3.md).
*
* Проверяет правило «Неверный предлог при глаголе»:
*  — discuss about the plan → PREP-005 (лишний about, удалить);
*  — explain me → PREP-005 (пропущен to, вставить);
*  — depend of it → PREP-005 (of неверный, заменить на on);
*  — explain about it → без ошибок (about допустим, записи нет в словаре).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep005.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Prep005Expect
* \brief Точечные ожидания для тестов правила PREP-005.
*/
struct Prep005Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (VERB). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Prep005Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_PREP005::TEST_Rule_PREP005() {}
TEST_Rule_PREP005::~TEST_Rule_PREP005() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.56-6.59)
// ------------------------------------------------------------------------

void TEST_Rule_PREP005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Prep005Expect>("expect");

    // === 6.56 PREP-005: discuss about the plan (лишний about) ======
    // Запись словаря: discuss about - → DeletePrep, wrongPrep=about.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("discuss about the plan"));
        RawToken discuss = makeRawToken(1, 1, "discuss", "VERB", 0, "root");
        discuss.lemma = QStringLiteral("discuss");
        addToken(s, discuss);
        RawToken about = makeRawToken(2, 2, "about", "ADP", 4, "case");
        about.lemma = QStringLiteral("about");
        addToken(s, about);
        RawToken the = makeRawToken(3, 3, "the", "DET", 4, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken plan = makeRawToken(4, 4, "plan", "NOUN", 1, "obl");
        plan.lemma = QStringLiteral("plan");
        addToken(s, plan);

        Prep005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-005");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.56_discuss_about_plan") << s << e;
    }

    // === 6.57 PREP-005: explain me (пропущен to) ===================
    // Запись словаря: explain - to → InsertPrep, prep=to.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("explain me"));
        RawToken explain = makeRawToken(1, 1, "explain", "VERB", 0, "root");
        explain.lemma = QStringLiteral("explain");
        addToken(s, explain);
        RawToken me = makeRawToken(2, 2, "me", "PRON", 1, "obj");
        me.lemma = QStringLiteral("I");
        addToken(s, me);

        Prep005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-005");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.57_explain_me") << s << e;
    }

    // === 6.58 PREP-005: depend of it (of неверный → on) ============
    // Запись словаря: depend of on → ReplacePrep, wrongPrep=of, prep=on.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("depend of it"));
        RawToken depend = makeRawToken(1, 1, "depend", "VERB", 0, "root");
        depend.lemma = QStringLiteral("depend");
        addToken(s, depend);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 3, "case");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 1, "obl");
        it.lemma = QStringLiteral("it");
        addToken(s, it);

        Prep005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("PREP-005");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.58_depend_of_it") << s << e;
    }

    // === 6.59 PREP-005 (исключение): explain about it — без ошибок =
    // about допустим с explain, записи нет в словаре.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("explain about it"));
        RawToken explain = makeRawToken(1, 1, "explain", "VERB", 0, "root");
        explain.lemma = QStringLiteral("explain");
        addToken(s, explain);
        RawToken about = makeRawToken(2, 2, "about", "ADP", 3, "case");
        about.lemma = QStringLiteral("about");
        addToken(s, about);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 1, "obl");
        it.lemma = QStringLiteral("it");
        addToken(s, it);

        Prep005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.59_explain_about_it") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_PREP005::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Prep005Expect, expect);
    verifyAnchorRule<Prep005Expect, Rule_PREP005>(rawSentence, expect, QString(QTest::currentDataTag()));
}
