/*!
* \file TEST_Rule_OTHER001.cpp
* \brief Тесты для правила OTHER-001 (раздел 6.87–6.88).
*
* 6.87: more better. Двойное сравнение, more избыточен при Cmp.
* 6.88: most best. Двойное сравнение, most избыточен при Sup.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other001.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* OTHER-001 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER001]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER001::TEST_Rule_OTHER001() {}
TEST_Rule_OTHER001::~TEST_Rule_OTHER001() {}

void TEST_Rule_OTHER001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.87: more better. more избыточен при сравнительной степени.
    // more/ADV[advmod→better], better/ADJ[HEAD=0, Degree=Cmp]
    // M=more (id=1), A=better (id=2, ADJ, Degree=Cmp)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("more better"));
        RawToken more = makeRawToken(1, 1, "more", "ADV", 2, "advmod");
        more.lemma = QStringLiteral("more");
        addToken(s, more);
        RawToken better = makeRawToken(2, 2, "better", "ADJ", 0, "root");
        better.lemma = QStringLiteral("good");
        better.featsRaw = QStringLiteral("Degree=Cmp");
        addToken(s, better);
        QTest::addRow("6.87_more_better")
            << s << 1
            << 1
            << QStringLiteral("OTHER-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.88: most best. most избыточен при превосходной степени.
    // most/ADV[advmod→best], best/ADJ[HEAD=0, Degree=Sup]
    // M=most (id=1), A=best (id=2, ADJ, Degree=Sup)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("most best"));
        RawToken most = makeRawToken(1, 1, "most", "ADV", 2, "advmod");
        most.lemma = QStringLiteral("most");
        addToken(s, most);
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 0, "root");
        best.lemma = QStringLiteral("good");
        best.featsRaw = QStringLiteral("Degree=Sup");
        addToken(s, best);
        QTest::addRow("6.88_most_best")
            << s << 1
            << 1
            << QStringLiteral("OTHER-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_OTHER001::TestRule()
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
    Rule_OTHER001 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}