/*!
* \file TEST_Rule_ART004.cpp
* \brief тесты для правила ART-004 (раздел 6.17).
*
* Проверяет правило "Пропущен the перед географическим названием":
*  — PROPN-группа из geo_the.txt без артикля
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art004.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_ART004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_ART004::TEST_Rule_ART004() {}
TEST_Rule_ART004::~TEST_Rule_ART004() {}

void TEST_Rule_ART004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.17 — пропущен the перед географическим названием
    // Головной PROPN — Ocean (HEAD=0), Pacific — зависимый с flat:name
    // Группа = "Pacific Ocean" — есть в geo_the.txt, артикля нет → ошибка
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("Pacific Ocean"));
        RawToken pacific = makeRawToken(1, 1, "Pacific", "PROPN", 2, "flat:name");
        pacific.lemma = QStringLiteral("Pacific");
        addToken(s, pacific);
        RawToken ocean = makeRawToken(2, 2, "Ocean", "PROPN", 0, "root");
        ocean.lemma = QStringLiteral("Ocean");
        addToken(s, ocean);
        QTest::addRow("6.17_Pacific_Ocean")
            << s
            << 1
            << QStringLiteral("ART-004")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{1, 2}});
    }
}

void TEST_Rule_ART004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART004 rule;

    // Проверяем все PROPN в предложении, т.к. ART-004 может сработать
    // на нескольких группах в одном предложении
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::PROPN)
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