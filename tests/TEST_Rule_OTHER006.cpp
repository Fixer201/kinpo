#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other006.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER006]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER006::TEST_Rule_OTHER006() {}
TEST_Rule_OTHER006::~TEST_Rule_OTHER006() {}

void TEST_Rule_OTHER006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.98: better then.
    // better/ADJ[HEAD=0, Degree=Cmp], then/ADV[advmod->better]
    // Срабатывание: один кандидат на then (id=2), display=[2], conflict={2}.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.98"),
                                        QStringLiteral("better then"));
        RawToken better = makeRawToken(1, 1, "better", "ADJ", 0, "root",
                                       QStringLiteral("Degree=Cmp"));
        better.lemma = QStringLiteral("good");
        addToken(s, better);

        RawToken then = makeRawToken(2, 2, "then", "ADV", 1, "advmod");
        then.lemma = QStringLiteral("then");
        addToken(s, then);

        QTest::addRow("6.98_better_then")
            << s << 1
            << 1
            << QStringLiteral("OTHER-006")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_OTHER006::TestRule()
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
    Rule_OTHER006 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}