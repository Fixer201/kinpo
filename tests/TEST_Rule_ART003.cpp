/*!
* \file TEST_Rule_ART003.cpp
* \brief DDT-тесты для правила ART-003 (раздел 6.13-6.16 тесты_v3.md).
*
* Проверяет правило «Лишний артикль перед языком/спортом»:
*  — артикль перед названием языка;
*  — исключение: язык как часть составного (compound);
*  — исключение: спорт как часть составного (compound);
*  — несколько спортов с артиклем (множественные кандидаты).
*
* Обход всех DET: ART-003 может сработать на нескольких артиклях
* в одном предложении (тест 6.16). Для multi-candidate используется
* expectedConflictZones.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art003Expect
* \brief Точечные ожидания для тестов правила ART-003.
*
* Структура не содержит anchorTokenId, так как правило использует
* паттерн обхода всех DET-токенов в предложении.
*/
struct Art003Expect {
    int expectedCount = -1;            ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;            ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<QSet<int>> expectedConflictZones; ///< Ожидаемые зоны конфликтов.
};

Q_DECLARE_METATYPE(Art003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART003::TEST_Rule_ART003() {}
TEST_Rule_ART003::~TEST_Rule_ART003() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.13-6.16)
// ------------------------------------------------------------------------

void TEST_Rule_ART003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art003Expect>("expect");

    // === 6.13 ART-003: лишний the перед языком ======================
    // Вход: the English. Ожидается: the→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the English"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken english = makeRawToken(2, 2, "English", "PROPN", 0, "root");
        english.lemma = QStringLiteral("English");
        addToken(s, english);

        Art003Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-003");
        e.expectedConflictZones = {QSet<int>{1}};

        QTest::addRow("6.13_the_English") << s << e;
    }

    // === 6.13a ART-003 (исключение): народ/нация ====================
    // Вход: The English are polite. Ожидается: NO ERRORS.
    // В текущей реализации теста нет отдельного addRow для 6.13a,
    // используется 6.13a_a_French для проверки артикля перед другим языком.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a French"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken french = makeRawToken(2, 2, "French", "PROPN", 0, "root");
        french.lemma = QStringLiteral("French");
        addToken(s, french);

        Art003Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-003");
        e.expectedConflictZones = {QSet<int>{1}};

        QTest::addRow("6.13a_a_French") << s << e;
    }

    // === 6.14 ART-003 (исключение): язык как часть составного =======
    // Вход: the English language. Ожидается: NO ERRORS (compound блокирует).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the English language"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken english = makeRawToken(2, 2, "English", "PROPN", 3, "compound");
        english.lemma = QStringLiteral("English");
        addToken(s, english);
        RawToken language = makeRawToken(3, 3, "language", "NOUN", 0, "root");
        language.lemma = QStringLiteral("language");
        addToken(s, language);

        Art003Expect e;
        e.expectedCount = 0;

        QTest::addRow("6.14_the_English_language") << s << e;
    }

    // === 6.15 ART-003 (исключение): спорт как часть составного ======
    // Вход: a football game. Ожидается: NO ERRORS (compound блокирует).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a football game"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 3, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken football = makeRawToken(2, 2, "football", "NOUN", 3, "compound");
        football.lemma = QStringLiteral("football");
        addToken(s, football);
        RawToken game = makeRawToken(3, 3, "game", "NOUN", 0, "root");
        game.lemma = QStringLiteral("game");
        addToken(s, game);

        Art003Expect e;
        e.expectedCount = 0;

        QTest::addRow("6.15_a_football_game") << s << e;
    }

    // === 6.16 ART-003: несколько спортов с артиклем the =============
    // Вход: play the football and the tennis. Ожидается: 2 кандидата ART-003.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("play the football and the tennis"));
        addToken(s, makeRawToken(1, 1, "play", "VERB", 0, "root"));
        RawToken the1 = makeRawToken(2, 2, "the", "DET", 3, "det");
        the1.lemma = QStringLiteral("the");
        addToken(s, the1);
        RawToken football = makeRawToken(3, 3, "football", "NOUN", 1, "obj");
        football.lemma = QStringLiteral("football");
        addToken(s, football);
        addToken(s, makeRawToken(4, 4, "and", "CCONJ", 6, "cc"));
        RawToken the2 = makeRawToken(5, 5, "the", "DET", 6, "det");
        the2.lemma = QStringLiteral("the");
        addToken(s, the2);
        RawToken tennis = makeRawToken(6, 6, "tennis", "NOUN", 3, "conj");
        tennis.lemma = QStringLiteral("tennis");
        addToken(s, tennis);

        Art003Expect e;
        e.expectedCount = 2;
        e.expectedRuleId = QStringLiteral("ART-003");
        e.expectedConflictZones = {QSet<int>{2}, QSet<int>{5}};

        QTest::addRow("6.16_play_the_football_and_the_tennis") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art003Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART003 rule;

    // Обходим все DET: правило может сработать на нескольких артиклях.
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::Det) {
            continue;
        }
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found) {
            result.insert(ce);
        }
    }

    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество кандидатов: ожидалось =" << expect.expectedCount
                     << "получено =" << actualCount;
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }

    if (expect.expectedCount == 0) {
        return;
    }

    // Сравнение зон конфликта для multi-candidate.
    if (!expect.expectedConflictZones.isEmpty()) {
        compareConflictZones(tag, result, expect.expectedConflictZones);
    }

    // Проверка ruleId для каждого кандидата.
    if (!expect.expectedRuleId.isEmpty()) {
        for (const CandidateError& ce : result) {
            if (ce.ruleId != expect.expectedRuleId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "ruleId: ожидался =" << expect.expectedRuleId
                         << "получено =" << ce.ruleId;
            }
            QCOMPARE(ce.ruleId, expect.expectedRuleId);
        }
    }
}