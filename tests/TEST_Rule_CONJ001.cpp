/*!
* \file TEST_Rule_CONJ001.cpp
* \brief DDT-тесты для правила CONJ-001 (раздел 6.60-6.63 тесты_v3.md).
*
* Проверяет правило «nor без отрицания»:
*  — 6.60: nor без отрицания — ошибка;
*  — 6.61: not в поддереве снимает ошибку;
*  — 6.62: none в поддереве снимает ошибку;
*  — 6.63: neither...nor — корректно.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj001.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj001Expect
* \brief Точечные ожидания для тестов правила CONJ-001.
*/
struct Conj001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (CCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ001::TEST_Rule_CONJ001() {}
TEST_Rule_CONJ001::~TEST_Rule_CONJ001() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.60-6.63)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj001Expect>("expect");

    // === 6.60 CONJ-001: nor без отрицания ===========================
    // apples nor oranges → nor→or.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("apples nor oranges"));
        RawToken apples = makeRawToken(1, 1, "apples", "NOUN", 0, "root");
        apples.lemma = QStringLiteral("apple");
        addToken(s, apples);
        RawToken nor = makeRawToken(2, 2, "nor", "CCONJ", 3, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken oranges = makeRawToken(3, 3, "oranges", "NOUN", 1, "conj");
        oranges.lemma = QStringLiteral("orange");
        addToken(s, oranges);

        Conj001Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-001");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.60_apples_nor_oranges") << s << e;
    }

    // === 6.61 CONJ-001 (исключение): not в поддереве ================
    // I do not want apples nor want oranges — без ошибок.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("I do not want apples nor want oranges"));
        RawToken i = makeRawToken(1, 1, "I", "PRON", 4, "nsubj");
        i.lemma = QStringLiteral("I");
        addToken(s, i);
        RawToken doTok = makeRawToken(2, 2, "do", "AUX", 4, "aux");
        doTok.lemma = QStringLiteral("do");
        addToken(s, doTok);
        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod");
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);
        RawToken want1 = makeRawToken(4, 4, "want", "VERB", 0, "root");
        want1.lemma = QStringLiteral("want");
        addToken(s, want1);
        RawToken apples = makeRawToken(5, 5, "apples", "NOUN", 4, "obj");
        apples.lemma = QStringLiteral("apple");
        addToken(s, apples);
        RawToken nor = makeRawToken(6, 6, "nor", "CCONJ", 7, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken want2 = makeRawToken(7, 7, "want", "VERB", 4, "conj");
        want2.lemma = QStringLiteral("want");
        addToken(s, want2);
        RawToken oranges = makeRawToken(8, 8, "oranges", "NOUN", 7, "obj");
        oranges.lemma = QStringLiteral("orange");
        addToken(s, oranges);

        Conj001Expect e;
        e.anchorTokenId = 6;
        e.expectedCount = 0;

        QTest::addRow("6.61_not_want_nor") << s << e;
    }

    // === 6.62 CONJ-001 (исключение): none в поддереве ===============
    // None of them wanted tea nor coffee — без ошибок.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("None of them wanted tea nor coffee"));
        RawToken none = makeRawToken(1, 1, "None", "PRON", 4, "nsubj");
        none.lemma = QStringLiteral("none");
        addToken(s, none);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 3, "case");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken them = makeRawToken(3, 3, "them", "PRON", 1, "nmod");
        them.lemma = QStringLiteral("they");
        addToken(s, them);
        RawToken wanted = makeRawToken(4, 4, "wanted", "VERB", 0, "root");
        wanted.lemma = QStringLiteral("want");
        addToken(s, wanted);
        RawToken tea = makeRawToken(5, 5, "tea", "NOUN", 4, "obj");
        tea.lemma = QStringLiteral("tea");
        addToken(s, tea);
        RawToken nor = makeRawToken(6, 6, "nor", "CCONJ", 7, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken coffee = makeRawToken(7, 7, "coffee", "NOUN", 5, "conj");
        coffee.lemma = QStringLiteral("coffee");
        addToken(s, coffee);

        Conj001Expect e;
        e.anchorTokenId = 6;
        e.expectedCount = 0;

        QTest::addRow("6.62_none_nor") << s << e;
    }

    // === 6.63 CONJ-001 (положительный): neither...nor ===============
    // neither John nor Mary — без ошибок (корректный neither...nor).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("neither John nor Mary"));
        RawToken neither = makeRawToken(1, 1, "neither", "CCONJ", 2, "cc:preconj");
        neither.lemma = QStringLiteral("neither");
        addToken(s, neither);
        RawToken john = makeRawToken(2, 2, "John", "PROPN", 0, "root");
        john.lemma = QStringLiteral("John");
        addToken(s, john);
        RawToken nor = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken mary = makeRawToken(4, 4, "Mary", "PROPN", 2, "conj");
        mary.lemma = QStringLiteral("Mary");
        addToken(s, mary);

        Conj001Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 0;

        QTest::addRow("6.63_neither_nor") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj001Expect, expect);
    verifyAnchorRule<Conj001Expect, Rule_CONJ001>(rawSentence, expect, QString(QTest::currentDataTag()));
}
