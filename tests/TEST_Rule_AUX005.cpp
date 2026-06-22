/*!
* \file TEST_Rule_AUX005.cpp
* \brief Тесты для правила AUX-005 (раздел 6.83–6.84).
*
* 6.83: must gone. Пропущен have после модального перед причастием.
* 6.84: must of gone. of вместо have после модального перед причастием.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux005.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-005 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX005]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX005::TEST_Rule_AUX005() {}
TEST_Rule_AUX005::~TEST_Rule_AUX005() {}

void TEST_Rule_AUX005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.83: must gone. Пропущен have.
    // must/AUX[aux→gone], gone/VERB[HEAD=0, VerbForm=Part]
    // M=must (id=1), V=gone (id=2, причастие), нет зависимого have у V
    // Ветка (а): вставить have. displayTokenIds=[1,2], conflictTokenIds={2}
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("must gone"));
        RawToken must = makeRawToken(1, 1, "must", "AUX", 2, "aux");
        must.lemma = QStringLiteral("must");
        addToken(s, must);
        RawToken gone = makeRawToken(2, 2, "gone", "VERB", 0, "root");
        gone.lemma = QStringLiteral("go");
        gone.featsRaw = QStringLiteral("VerbForm=Part");
        addToken(s, gone);
        QTest::addRow("6.83_must_gone")
            << s << 1
            << 1
            << QStringLiteral("AUX-005")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.84: must of gone. of вместо have.
    // must/AUX[aux→gone], of/ADP[advmod→gone], gone/VERB[HEAD=0, VerbForm=Part]
    // M=must (id=1), of (id=2, ADP, следующий за M), V=gone (id=3, причастие)
    // Ветка (б): заменить of на have.
    // displayTokenIds=[1,2,3], conflictTokenIds={2}
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("must of gone"));
        RawToken must = makeRawToken(1, 1, "must", "AUX", 3, "aux");
        must.lemma = QStringLiteral("must");
        addToken(s, must);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 3, "advmod");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken gone = makeRawToken(3, 3, "gone", "VERB", 0, "root");
        gone.lemma = QStringLiteral("go");
        gone.featsRaw = QStringLiteral("VerbForm=Part");
        addToken(s, gone);
        QTest::addRow("6.84_must_of_gone")
            << s << 1
            << 1
            << QStringLiteral("AUX-005")
            << (QList<QList<int>>{QList<int>{1, 2, 3}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_AUX005::TestRule()
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
    Rule_AUX005 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}