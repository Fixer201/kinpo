/*!
* \file TEST_Rule_PREP004.cpp
* \brief Тесты для правила PREP-004 (раздел 6.54–6.55).
*
* Проверяет правило "Perfect + ago":
*  — 6.54: He has went there two years ago → PREP-004 (ago + Perfect)
*  — 6.55: has telephoned ago → PREP-004 (правильный глагол, fallback)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep004.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* Словарь past_forms.txt нужен для формирования исправления (Past Simple),
* но в текущей реализации CandidateError не хранит текст исправления.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_PREP004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_PREP004::TEST_Rule_PREP004() {}
TEST_Rule_PREP004::~TEST_Rule_PREP004() {}

void TEST_Rule_PREP004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.54 — He has went there two years ago → PREP-004
    // He/PRON[nsubj→went], has/AUX[aux→went], went/VERB[HEAD=0],
    // there/ADV[advmod→went], two/NUM[nummod→years],
    // years/NOUN[nmod:tmod→ago], ago/ADV[advmod→went]
    // Якорь — ago (id=7). AUX has (id=2) → went (id=3, VerbForm=Part)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("He has went there two years ago"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken has = makeRawToken(2, 2, "has", "AUX", 3, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        // went — VERB с VerbForm=Part (конструкция Perfect)
        RawToken went = makeRawToken(3, 3, "went", "VERB", 0, "root",
                                      QStringLiteral("VerbForm=Part"));
        went.lemma = QStringLiteral("go");
        addToken(s, went);
        RawToken there = makeRawToken(4, 4, "there", "ADV", 3, "advmod");
        there.lemma = QStringLiteral("there");
        addToken(s, there);
        RawToken two = makeRawToken(5, 5, "two", "NUM", 6, "nummod");
        two.lemma = QStringLiteral("two");
        addToken(s, two);
        RawToken years = makeRawToken(6, 6, "years", "NOUN", 7, "nmod:tmod");
        years.lemma = QStringLiteral("year");
        addToken(s, years);
        RawToken ago = makeRawToken(7, 7, "ago", "ADV", 3, "advmod");
        ago.lemma = QStringLiteral("ago");
        addToken(s, ago);
        QTest::addRow("6.54_has_went_ago")
            << s << 7
            << 1
            << QStringLiteral("PREP-004")
            << (QList<QList<int>>{QList<int>{2, 3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.55 — has telephoned ago → PREP-004 (правильный глагол, fallback)
    // has/AUX[aux→telephoned], telephoned/VERB[HEAD=0], ago/ADV[advmod→telephoned]
    // Якорь — ago (id=3). AUX has (id=1) → telephoned (id=2, VerbForm=Part)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("has telephoned ago"));
        RawToken has = makeRawToken(1, 1, "has", "AUX", 2, "aux");
        has.lemma = QStringLiteral("have");
        addToken(s, has);
        // telephoned — VERB с VerbForm=Part (правильный глагол, нет в past_forms.txt)
        RawToken telephoned = makeRawToken(2, 2, "telephoned", "VERB", 0, "root",
                                            QStringLiteral("VerbForm=Part"));
        telephoned.lemma = QStringLiteral("telephone");
        addToken(s, telephoned);
        RawToken ago = makeRawToken(3, 3, "ago", "ADV", 2, "advmod");
        ago.lemma = QStringLiteral("ago");
        addToken(s, ago);
        QTest::addRow("6.55_has_telephoned_ago")
            << s << 3
            << 1
            << QStringLiteral("PREP-004")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }
}

void TEST_Rule_PREP004::TestRule()
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
    Rule_PREP004 rule;

    // Якорь — ago (ADV), проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}