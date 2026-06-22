/*!
* \file TEST_Rule_DET004.cpp
* \brief тесты для правила DET-004 (раздел 6.40–6.41).
*
* Проверяет правило "Омофон в притяжательной позиции":
*  — it's → its (it's PRON, nmod:poss, HEAD=NOUN)
*  — there → their (there ADV, det, HEAD=NOUN)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det004.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_DET004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_DET004::TEST_Rule_DET004() {}
TEST_Rule_DET004::~TEST_Rule_DET004() {}

void TEST_Rule_DET004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.40 — it's tail → its (it's PRON, nmod:poss, HEAD=tail NOUN)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("it's tail"));
        RawToken its = makeRawToken(1, 1, "it's", "PRON", 2, "nmod:poss");
        its.lemma = QStringLiteral("it's");
        addToken(s, its);
        RawToken tail = makeRawToken(2, 2, "tail", "NOUN", 0, "root");
        tail.lemma = QStringLiteral("tail");
        addToken(s, tail);
        QTest::addRow("6.40_its_tail")
            << s << 1
            << 1
            << QStringLiteral("DET-004")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.41 — there house → their (there ADV, det, HEAD=house NOUN)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("there house"));
        RawToken there = makeRawToken(1, 1, "there", "ADV", 2, "det");
        there.lemma = QStringLiteral("there");
        addToken(s, there);
        RawToken house = makeRawToken(2, 2, "house", "NOUN", 0, "root");
        house.lemma = QStringLiteral("house");
        addToken(s, house);
        QTest::addRow("6.41_there_house")
            << s << 1
            << 1
            << QStringLiteral("DET-004")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_DET004::TestRule()
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
    Rule_DET004 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}