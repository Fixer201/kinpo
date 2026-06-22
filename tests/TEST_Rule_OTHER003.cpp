/*!
* \file TEST_Rule_OTHER003.cpp
* \brief Тесты для правила OTHER-003 (раздел 6.91–6.92).
*
* 6.91: much happy. much перед положительной степенью, заменяется на very.
* 6.92: very better. very перед сравнительной степенью, заменяется на much.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other003.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* OTHER-003 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER003::TEST_Rule_OTHER003() {}
TEST_Rule_OTHER003::~TEST_Rule_OTHER003() {}

void TEST_Rule_OTHER003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.91: much happy. much перед положительной степенью, заменяется на very.
    // much/ADV[advmod→happy], happy/ADJ[HEAD=0, Degree=Pos]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("much happy"));
        RawToken much = makeRawToken(1, 1, "much", "ADV", 2, "advmod");
        much.lemma = QStringLiteral("much");
        addToken(s, much);
        RawToken happy = makeRawToken(2, 2, "happy", "ADJ", 0, "root");
        happy.lemma = QStringLiteral("happy");
        happy.featsRaw = QStringLiteral("Degree=Pos");
        addToken(s, happy);
        QTest::addRow("6.91_much_happy")
            << s << 1
            << 1
            << QStringLiteral("OTHER-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.92: very better. very перед сравнительной степенью, заменяется на much.
    // very/ADV[advmod→better], better/ADJ[HEAD=0, Degree=Cmp]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("very better"));
        RawToken very = makeRawToken(1, 1, "very", "ADV", 2, "advmod");
        very.lemma = QStringLiteral("very");
        addToken(s, very);
        RawToken better = makeRawToken(2, 2, "better", "ADJ", 0, "root");
        better.lemma = QStringLiteral("good");
        better.featsRaw = QStringLiteral("Degree=Cmp");
        addToken(s, better);
        QTest::addRow("6.92_very_better")
            << s << 1
            << 1
            << QStringLiteral("OTHER-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_OTHER003::TestRule()
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
    Rule_OTHER003 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}