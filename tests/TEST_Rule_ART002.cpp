/*!
* \file TEST_Rule_ART002.cpp
* \brief тесты для правил ART-002 и ART-002a (раздел 6.7–6.12).
*
* Проверяет срабатывание и исключения правила "Пропущен the перед ADJ":
*  — превосходная степень (Degree=Sup)
*  — порядковое числительное (NumType=Ord)
*  — аналитическая превосходная степень (most/least + ADJ)
*  — артикль уже присутствует (the/a/an)
*  — исключение: last/next + единица времени
*/

#include <QtTest>
#include <QObject>
#include <QSet>
#include <QDir>

#include "TEST_Rule_ART002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art002.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QDir dir(QDir::current());
    dir.cdUp();
    dir.cdUp();
    QString listsDir = dir.filePath("docs/lists");
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_ART002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_ART002::TEST_Rule_ART002() {}
TEST_Rule_ART002::~TEST_Rule_ART002() {}

void TEST_Rule_ART002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<int>>("expectedDisplayIds");
    QTest::addColumn<QSet<int>>("expectedConflictIds");

    // 6.7 — превосходная степень (Sup)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("best solution"));
        RawToken best = makeRawToken(1, 1, "best", "ADJ", 2, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(2, 2, "solution", "NOUN", 0, "root"));
        QTest::addRow("6.7_best_solution")
            << s << 1
            << QStringLiteral("ART-002")
            << (QList<int>{1, 2})
            << (QSet<int>{2});
    }

    // 6.7a — a перед best (det уже есть, ART-002 не срабатывает)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a best solution"));
        addToken(s, makeRawToken(1, 1, "a", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));
        QTest::addRow("6.7a_a_best_solution")
            << s << 2
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.7b — an перед best (det уже есть, ART-002 не срабатывает)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("an best solution"));
        addToken(s, makeRawToken(1, 1, "an", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));
        QTest::addRow("6.7b_an_best_solution")
            << s << 2
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.8 — порядковое числительное (Ord)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("second chance"));
        RawToken second = makeRawToken(1, 1, "second", "ADJ", 2, "amod", "NumType=Ord");
        second.lemma = QStringLiteral("second");
        addToken(s, second);
        addToken(s, makeRawToken(2, 2, "chance", "NOUN", 0, "root"));
        QTest::addRow("6.8_second_chance")
            << s << 1
            << QStringLiteral("ART-002")
            << (QList<int>{1, 2})
            << (QSet<int>{2});
    }

    // 6.9 — аналитическая превосходная степень (ART-002a)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("most beautiful girl"));
        RawToken most = makeRawToken(1, 1, "most", "ADV", 2, "advmod");
        most.lemma = QStringLiteral("most");
        addToken(s, most);
        addToken(s, makeRawToken(2, 2, "beautiful", "ADJ", 3, "amod"));
        addToken(s, makeRawToken(3, 3, "girl", "NOUN", 0, "root"));
        QTest::addRow("6.9_most_beautiful_girl")
            << s << 1
            << QStringLiteral("ART-002a")
            << (QList<int>{1, 2, 3})
            << (QSet<int>{3});
    }

    // 6.10 — артикль the уже присутствует
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the best solution"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));
        QTest::addRow("6.10_the_best_solution")
            << s << 2
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.11 — исключение: last + time_unit
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("last week"));
        RawToken last = makeRawToken(1, 1, "last", "ADJ", 2, "amod");
        last.lemma = QStringLiteral("last");
        addToken(s, last);
        RawToken week = makeRawToken(2, 2, "week", "NOUN", 0, "root");
        week.lemma = QStringLiteral("week");
        addToken(s, week);
        QTest::addRow("6.11_last_week")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }

    // 6.12 — исключение: next + time_unit
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("next year"));
        RawToken next = makeRawToken(1, 1, "next", "ADJ", 2, "amod");
        next.lemma = QStringLiteral("next");
        addToken(s, next);
        RawToken year = makeRawToken(2, 2, "year", "NOUN", 0, "root");
        year.lemma = QStringLiteral("year");
        addToken(s, year);
        QTest::addRow("6.12_next_year")
            << s << 1
            << QString()
            << QList<int>()
            << QSet<int>();
    }
}

void TEST_Rule_ART002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, anchorTokenId);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<int>, expectedDisplayIds);
    QFETCH(QSet<int>, expectedConflictIds);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr, qPrintable(QString("[%1] Якорный токен %2 не найден").arg(tag).arg(anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();

    // ART-002a вызывается если якорь — ADV, иначе ART-002
    QSet<CandidateError> result;
    if (anchor->upos == Upos::ADV) {
        Rule_ART002a rule;
        result = rule.check(*anchor, 0, DocumentModel(), runtime);
    } else {
        Rule_ART002 rule;
        result = rule.check(*anchor, 0, DocumentModel(), runtime);
    }

    // expectedRuleId пустой — правило не должно сработать
    if (expectedRuleId.isEmpty()) {
        // Если пришёл неожиданный кандидат — выводим его в qDebug для отладки
        if (result.size() != 0) {
            const CandidateError& ce = *result.begin();
            qDebug() << "[" << tag << "] Неожиданное срабатывание:" << ce.ruleId
                     << "display:" << ce.displayTokenIds << "conflict:" << ce.conflictTokenIds;
        }
        QCOMPARE(result.size(), 0);
    } else {
        // Ожидаем ровно один кандидат
        QCOMPARE(result.size(), 1);
        compareSingleCandidate(tag, *result.begin(), expectedRuleId, expectedDisplayIds, expectedConflictIds);
    }
}