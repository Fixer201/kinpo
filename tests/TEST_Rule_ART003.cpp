/*!
* \file TEST_Rule_ART003.cpp
* \brief тесты для правила ART-003 (раздел 6.13–6.16).
*
* Проверяет правило "Лишний артикль (нулевой контекст)":
*  — артикль перед названием языка
*  — исключение: язык как часть составного (language)
*  — исключение: спорт как часть составного (compound)
*  — несколько спортов с артиклем (множественные кандидаты)
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

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_ART003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_ART003::TEST_Rule_ART003() {}
TEST_Rule_ART003::~TEST_Rule_ART003() {}

void TEST_Rule_ART003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.13 — лишний the перед языком
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the English"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken english = makeRawToken(2, 2, "English", "PROPN", 0, "root");
        english.lemma = QStringLiteral("English");
        addToken(s, english);
        QTest::addRow("6.13_the_English")
            << s
            << 1
            << QStringLiteral("ART-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.14 — исключение: язык как часть составного
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the English language"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken english = makeRawToken(2, 2, "English", "PROPN", 3, "compound");
        english.lemma = QStringLiteral("English");
        addToken(s, english);
        RawToken language = makeRawToken(3, 3, "language", "NOUN", 0, "root");
        language.lemma = QStringLiteral("language");
        addToken(s, language);
        QTest::addRow("6.14_the_English_language")
            << s
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.15 — исключение: спорт как часть составного
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a football game"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 3, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken football = makeRawToken(2, 2, "football", "NOUN", 3, "compound");
        football.lemma = QStringLiteral("football");
        addToken(s, football);
        RawToken game = makeRawToken(3, 3, "game", "NOUN", 0, "root");
        game.lemma = QStringLiteral("game");
        addToken(s, game);
        QTest::addRow("6.15_a_football_game")
            << s
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.16 — несколько спортов с артиклем the (2 кандидата)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("play the football and the tennis"));
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
        QTest::addRow("6.16_play_the_football_and_the_tennis")
            << s
            << 2
            << QStringLiteral("ART-003")
            << (QList<QList<int>>{QList<int>{2}, QList<int>{5}})
            << (QList<QSet<int>>{QSet<int>{2}, QSet<int>{5}});
    }
}

void TEST_Rule_ART003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART003 rule;

    // Проверяем все DET в предложении, т.к. ART-003 может сработать
    // на нескольких артиклях в одном предложении (тест 6.16)
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::DET)
            continue;
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found)
            result.insert(ce);
    }

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    // Каждый кандидат должен совпасть с одной из ожидаемых пар
    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}