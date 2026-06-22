/*!
* \file TEST_Rule_AUX002.cpp
* \brief Тесты для правила AUX-002 (раздел 6.79–6.80).
*
* 6.79: did went — после do ожидается инфинитив go.
* 6.80: have saw — после have ожидается Past Participle seen.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux002.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-002 использует past_forms.txt для ветки (б).
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX002::TEST_Rule_AUX002() {}
TEST_Rule_AUX002::~TEST_Rule_AUX002() {}

void TEST_Rule_AUX002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.79: did went → AUX-002, did went исправляется на did go
    // did/AUX[aux→went], went/VERB[HEAD=0]
    // A=did (id=1, LEMMA=do), V=went (id=2, VERB, VerbForm=Past)
    // После do ожидается инфинитив, went не инфинитив
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("did went"));
        RawToken did = makeRawToken(1, 1, "did", "AUX", 2, "aux");
        did.lemma = QStringLiteral("do");
        addToken(s, did);
        RawToken went = makeRawToken(2, 2, "went", "VERB", 0, "root");
        went.lemma = QStringLiteral("go");
        // VerbForm=Past, не Inf
        went.featsRaw = QStringLiteral("VerbForm=Past|Tense=Past");
        addToken(s, went);
        QTest::addRow("6.79_did_went")
            << s << 1
            << 1
            << QStringLiteral("AUX-002")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.80: have saw → AUX-002, have saw исправляется на have seen
    // have/AUX[aux→saw], saw/VERB[HEAD=0]
    // A=have (id=1), V=saw (id=2, VERB, LEMMA=see, VerbForm=Past)
    // После have ожидается Past Participle, saw не причастие
    // past_forms.txt: see → saw → seen, исправление на seen
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("have saw"));
        RawToken have = makeRawToken(1, 1, "have", "AUX", 2, "aux");
        have.lemma = QStringLiteral("have");
        addToken(s, have);
        RawToken saw = makeRawToken(2, 2, "saw", "VERB", 0, "root");
        saw.lemma = QStringLiteral("see");
        // VerbForm=Past, не Part
        saw.featsRaw = QStringLiteral("VerbForm=Past|Tense=Past");
        addToken(s, saw);
        QTest::addRow("6.80_have_saw")
            << s << 1
            << 1
            << QStringLiteral("AUX-002")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_AUX002::TestRule()
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
    Rule_AUX002 rule;

    // Якорь — вспомогательный do/have, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}