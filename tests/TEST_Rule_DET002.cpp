/*!
* \file TEST_Rule_DET002.cpp
* \brief тесты для правила DET-002 (раздел 6.34–6.36).
*
* Проверяет правило "Два центральных детерминатива":
*  — артикль + притяжательное (the my friend → the)
*  — притяжательное + артикль (my the friend → the)
*  — исключение: предетерминатив (all the people — NO ERRORS)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det002.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_DET002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_DET002::TEST_Rule_DET002() {}
TEST_Rule_DET002::~TEST_Rule_DET002() {}

void TEST_Rule_DET002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.34 — the my friend → the (артикль + притяжательное)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the my friend"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken my = makeRawToken(2, 2, "my", "DET", 3, "det");
        my.lemma = QStringLiteral("my");
        addToken(s, my);
        RawToken friendTok = makeRawToken(3, 3, "friend", "NOUN", 0, "root");
        friendTok.lemma = QStringLiteral("friend");
        addToken(s, friendTok);
        QTest::addRow("6.34_the_my_friend")
            << s
            << 1
            << QStringLiteral("DET-002")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.35 — my the friend → the (притяжательное + артикль)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("my the friend"));
        RawToken my = makeRawToken(1, 1, "my", "DET", 3, "det");
        my.lemma = QStringLiteral("my");
        addToken(s, my);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken friendTok = makeRawToken(3, 3, "friend", "NOUN", 0, "root");
        friendTok.lemma = QStringLiteral("friend");
        addToken(s, friendTok);
        QTest::addRow("6.35_my_the_friend")
            << s
            << 1
            << QStringLiteral("DET-002")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.36 — all the people → NO ERRORS (all — предетерминатив)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("all the people"));
        RawToken all = makeRawToken(1, 1, "all", "DET", 3, "det");
        all.lemma = QStringLiteral("all");
        addToken(s, all);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken people = makeRawToken(3, 3, "people", "NOUN", 0, "root");
        people.lemma = QStringLiteral("people");
        addToken(s, people);
        QTest::addRow("6.36_all_the_people")
            << s
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_DET002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET002 rule;

    // Проверяем все NOUN в предложении
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::NOUN)
            continue;
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found)
            result.insert(ce);
    }

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}