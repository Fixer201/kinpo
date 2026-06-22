/*!
* \file TEST_Rule_ART006.cpp
* \brief тесты для правила ART-006 (раздел 6.21–6.26).
*
* Проверяет правило "Неверный a/an":
*  — согласный звук (an cat → a)
*  — гласный звук (a apple → an)
*  — mute h (a hour → an)
*  — число (a 18 → an)
*  — согласный /j/ (an university → a)
*  — аббревиатура (a FBI → an)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art006.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_ART006]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_ART006::TEST_Rule_ART006() {}
TEST_Rule_ART006::~TEST_Rule_ART006() {}

void TEST_Rule_ART006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.21 — an cat → a (cat в CMUdict, K — согласная)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("an cat"));
        RawToken an = makeRawToken(1, 1, "an", "DET", 2, "det");
        an.lemma = QStringLiteral("a");
        addToken(s, an);
        RawToken cat = makeRawToken(2, 2, "cat", "NOUN", 0, "root");
        cat.lemma = QStringLiteral("cat");
        addToken(s, cat);
        QTest::addRow("6.21_an_cat")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.22 — a apple → an (apple в CMUdict, AE1 — гласная)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a apple"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken apple = makeRawToken(2, 2, "apple", "NOUN", 0, "root");
        apple.lemma = QStringLiteral("apple");
        addToken(s, apple);
        QTest::addRow("6.22_a_apple")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.23 — a hour → an (hour в CMUdict, AW1 — гласная, mute h)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a hour"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken hour = makeRawToken(2, 2, "hour", "NOUN", 0, "root");
        hour.lemma = QStringLiteral("hour");
        addToken(s, hour);
        QTest::addRow("6.23_a_hour")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.24 — a 18 → an (число, начинается с «8»)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a 18"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken n18 = makeRawToken(2, 2, "18", "NUM", 0, "root");
        n18.lemma = QStringLiteral("18");
        addToken(s, n18);
        QTest::addRow("6.24_a_18")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.25 — an university → a (university в CMUdict, Y — согласный /j/)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("an university"));
        RawToken an = makeRawToken(1, 1, "an", "DET", 2, "det");
        an.lemma = QStringLiteral("a");
        addToken(s, an);
        RawToken uni = makeRawToken(2, 2, "university", "NOUN", 0, "root");
        uni.lemma = QStringLiteral("university");
        addToken(s, uni);
        QTest::addRow("6.25_an_university")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.26 — a FBI → an (FBI в CMUdict как fbi, EH1 — гласная)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a FBI"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken fbi = makeRawToken(2, 2, "FBI", "PROPN", 0, "root");
        fbi.lemma = QStringLiteral("FBI");
        addToken(s, fbi);
        QTest::addRow("6.26_a_FBI")
            << s << 1
            << 1
            << QStringLiteral("ART-006")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_ART006::TestRule()
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
    Rule_ART006 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}