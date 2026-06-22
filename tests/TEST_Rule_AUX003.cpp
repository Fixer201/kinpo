/*!
* \file TEST_Rule_AUX003.cpp
* \brief Тесты для правила AUX-003 (раздел 6.81).
*
* 6.81: must to go. Избыточная частица to после модального must.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux003.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-003 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX003::TEST_Rule_AUX003() {}
TEST_Rule_AUX003::~TEST_Rule_AUX003() {}

void TEST_Rule_AUX003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.81: must to go. to удаляется, так как после модального
    // частица to не нужна.
    // must/AUX[aux→go], to/PART[mark→go], go/VERB[HEAD=0]
    // T=to (id=2), V=go (id=3), M=must (id=1) — aux-зависимый V, модальный
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("must to go"));
        RawToken must = makeRawToken(1, 1, "must", "AUX", 3, "aux");
        must.lemma = QStringLiteral("must");
        addToken(s, must);
        RawToken to = makeRawToken(2, 2, "to", "PART", 3, "mark");
        to.lemma = QStringLiteral("to");
        addToken(s, to);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        QTest::addRow("6.81_must_to_go")
            << s << 2
            << 1
            << QStringLiteral("AUX-003")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_AUX003::TestRule()
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
    Rule_AUX003 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}