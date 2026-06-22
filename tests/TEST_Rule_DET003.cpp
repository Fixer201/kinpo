/*!
* \file TEST_Rule_DET003.cpp
* \brief тесты для правила DET-003 (раздел 6.37–6.39).
*
* Проверяет правило "Притяжательный омофон как подлежащее":
*  — its → it's (Poss=Yes, nsubj, HEAD=VERB)
*  — their → they're (Poss=Yes, nsubj, HEAD=VERB)
*  — исключение: nmod:poss (a bird in its nest — NO ERRORS)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det003.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_DET003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_DET003::TEST_Rule_DET003() {}
TEST_Rule_DET003::~TEST_Rule_DET003() {}

void TEST_Rule_DET003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.37 — Its raining → It's (its PRON, Poss=Yes, nsubj, HEAD raining VERB)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("Its raining"));
        RawToken its = makeRawToken(1, 1, "Its", "PRON", 2, "nsubj",
                                     QStringLiteral("Poss=Yes"));
        its.lemma = QStringLiteral("its");
        addToken(s, its);
        RawToken raining = makeRawToken(2, 2, "raining", "VERB", 0, "root");
        raining.lemma = QStringLiteral("rain");
        addToken(s, raining);
        QTest::addRow("6.37_Its_raining")
            << s << 1
            << 1
            << QStringLiteral("DET-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.38 — Their coming → They're (their PRON, Poss=Yes, nsubj, HEAD coming VERB)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("Their coming"));
        RawToken their = makeRawToken(1, 1, "Their", "PRON", 2, "nsubj",
                                       QStringLiteral("Poss=Yes"));
        their.lemma = QStringLiteral("their");
        addToken(s, their);
        RawToken coming = makeRawToken(2, 2, "coming", "VERB", 0, "root");
        coming.lemma = QStringLiteral("come");
        addToken(s, coming);
        QTest::addRow("6.38_Their_coming")
            << s << 1
            << 1
            << QStringLiteral("DET-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.39 — a bird in its nest → NO ERRORS (its nmod:poss — исключение)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a bird in its nest"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken bird = makeRawToken(2, 2, "bird", "NOUN", 0, "root");
        bird.lemma = QStringLiteral("bird");
        addToken(s, bird);
        RawToken in = makeRawToken(3, 3, "in", "ADP", 5, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken its = makeRawToken(4, 4, "its", "PRON", 5, "nmod:poss",
                                     QStringLiteral("Poss=Yes"));
        its.lemma = QStringLiteral("its");
        addToken(s, its);
        RawToken nest = makeRawToken(5, 5, "nest", "NOUN", 2, "nmod");
        nest.lemma = QStringLiteral("nest");
        addToken(s, nest);
        QTest::addRow("6.39_a_bird_in_its_nest")
            << s << 4
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_DET003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, anchorTokenId);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr, qPrintable(QString("[%1] Якорный токен %2 не найден").arg(tag).arg(anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET003 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}