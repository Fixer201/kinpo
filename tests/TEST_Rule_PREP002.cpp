/*!
* \file TEST_Rule_PREP002.cpp
* \brief Тесты для правила PREP-002 (раздел 6.50–6.51).
*
* Проверяет правило "for ↔ since":
*  — 6.50: for перед моментом времени (2020) в Perfect → since
*  — 6.51: since перед длительностью (two days) → for
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep002.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* Словари time_units.txt и durations.txt нужны для определения
* момента времени и длительности.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_PREP002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_PREP002::TEST_Rule_PREP002() {}
TEST_Rule_PREP002::~TEST_Rule_PREP002() {}

void TEST_Rule_PREP002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.50 — I have lived for 2020 → since (момент времени + Perfect)
    // I/PRON[nsubj→lived], have/AUX[aux→lived], lived/VERB[HEAD=0],
    // for/ADP[case→2020], 2020/NUM[nmod:tmod→lived]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("I have lived for 2020"));
        RawToken i = makeRawToken(1, 1, "I", "PRON", 3, "nsubj");
        i.lemma = QStringLiteral("I");
        addToken(s, i);
        RawToken have = makeRawToken(2, 2, "have", "AUX", 3, "aux");
        have.lemma = QStringLiteral("have");
        addToken(s, have);
        // lived — VERB с VerbForm=Part (конструкция Perfect)
        RawToken lived = makeRawToken(3, 3, "lived", "VERB", 0, "root",
                                       QStringLiteral("VerbForm=Part"));
        lived.lemma = QStringLiteral("live");
        addToken(s, lived);
        RawToken forTok = makeRawToken(4, 4, "for", "ADP", 5, "case");
        forTok.lemma = QStringLiteral("for");
        addToken(s, forTok);
        RawToken year = makeRawToken(5, 5, "2020", "NUM", 3, "nmod:tmod");
        year.lemma = QStringLiteral("2020");
        addToken(s, year);
        QTest::addRow("6.50_for_2020")
            << s << 4
            << 1
            << QStringLiteral("PREP-002")
            << (QList<QList<int>>{QList<int>{4}})
            << (QList<QSet<int>>{QSet<int>{4}});
    }

    // 6.51 — He has waited since two days → for (длительность)
    // He/PRON[nsubj→waited], has/AUX[aux→waited], waited/VERB[HEAD=0],
    // since/ADP[case→days], two/NUM[nummod→days], days/NOUN[nmod:tmod→waited]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("He has waited since two days"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken has = makeRawToken(2, 2, "has", "AUX", 3, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        // waited — VERB с VerbForm=Part
        RawToken waited = makeRawToken(3, 3, "waited", "VERB", 0, "root",
                                         QStringLiteral("VerbForm=Part"));
        waited.lemma = QStringLiteral("wait");
        addToken(s, waited);
        RawToken since = makeRawToken(4, 4, "since", "ADP", 6, "case");
        since.lemma = QStringLiteral("since");
        addToken(s, since);
        RawToken two = makeRawToken(5, 5, "two", "NUM", 6, "nummod");
        two.lemma = QStringLiteral("two");
        addToken(s, two);
        RawToken days = makeRawToken(6, 6, "days", "NOUN", 3, "nmod:tmod");
        days.lemma = QStringLiteral("day");
        addToken(s, days);
        QTest::addRow("6.51_since_two_days")
            << s << 4
            << 1
            << QStringLiteral("PREP-002")
            << (QList<QList<int>>{QList<int>{4}})
            << (QList<QSet<int>>{QSet<int>{4}});
    }
}

void TEST_Rule_PREP002::TestRule()
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
    Rule_PREP002 rule;

    // Якорь — сам предлог ADP, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}