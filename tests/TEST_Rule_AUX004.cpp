/*!
* \file TEST_Rule_AUX004.cpp
* \brief Тесты для правила AUX-004 (раздел 6.82).
*
* 6.82: can will go. Два модальных у одного VERB, второй (will) удаляется.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux004.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-004 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX004::TEST_Rule_AUX004() {}
TEST_Rule_AUX004::~TEST_Rule_AUX004() {}

void TEST_Rule_AUX004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.82: can will go. Два модальных у одного VERB.
    // can/AUX[aux→go], will/AUX[aux→go], go/VERB[HEAD=0]
    // Первый модальный can (id=1) остаётся, второй will (id=2) удаляется.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("can will go"));
        RawToken can = makeRawToken(1, 1, "can", "AUX", 3, "aux");
        can.lemma = QStringLiteral("can");
        addToken(s, can);
        RawToken will = makeRawToken(2, 2, "will", "AUX", 3, "aux");
        will.lemma = QStringLiteral("will");
        addToken(s, will);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        QTest::addRow("6.82_can_will_go")
            << s << 1
            << QStringLiteral("AUX-004")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_AUX004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_AUX004 rule;

    // Обходим все AUX. Правило срабатывает только на втором модальном.
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::AUX)
            continue;
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found)
            result.insert(ce);
    }

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}