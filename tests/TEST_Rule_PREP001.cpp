/*!
* \file TEST_Rule_PREP001.cpp
* \brief Тесты для правила PREP-001 (раздел 6.43–6.49).
*
* Проверяет правило "Неверный временной предлог":
*  — 6.43: день недели (in Monday → on)
*  — 6.44: месяц (on July → in)
*  — 6.45: часть дня (at morning → in)
*  — 6.46: год (on 2020 → in)
*  — 6.47: ночь (in night → at)
*  — 6.48: корректный предлог не срабатывает (at 5 o'clock — NO ERRORS)
*  — 6.49: исключение compound+день недели (on Monday morning — NO ERRORS)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep001.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* Словарь time_units.txt нужен для проверки дней недели, месяцев и сезонов.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_PREP001]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_PREP001::TEST_Rule_PREP001() {}
TEST_Rule_PREP001::~TEST_Rule_PREP001() {}

void TEST_Rule_PREP001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.43 — in Monday → on (день недели)
    // in/ADP[case→Monday], Monday/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("in Monday"));
        RawToken in = makeRawToken(1, 1, "in", "ADP", 2, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken monday = makeRawToken(2, 2, "Monday", "PROPN", 0, "root");
        monday.lemma = QStringLiteral("Monday");
        addToken(s, monday);
        QTest::addRow("6.43_in_Monday")
            << s << 1
            << 1
            << QStringLiteral("PREP-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.44 — on July → in (месяц)
    // on/ADP[case→July], July/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("on July"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 2, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken july = makeRawToken(2, 2, "July", "PROPN", 0, "root");
        july.lemma = QStringLiteral("July");
        addToken(s, july);
        QTest::addRow("6.44_on_July")
            << s << 1
            << 1
            << QStringLiteral("PREP-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.45 — at morning → in (часть дня)
    // at/ADP[case→morning], morning/NOUN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("at morning"));
        RawToken at = makeRawToken(1, 1, "at", "ADP", 2, "case");
        at.lemma = QStringLiteral("at");
        addToken(s, at);
        RawToken morning = makeRawToken(2, 2, "morning", "NOUN", 0, "root");
        morning.lemma = QStringLiteral("morning");
        addToken(s, morning);
        QTest::addRow("6.45_at_morning")
            << s << 1
            << 1
            << QStringLiteral("PREP-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.46 — on 2020 → in (год, NUM с FORM из 4 цифр)
    // on/ADP[case→2020], 2020/NUM[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("on 2020"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 2, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken year = makeRawToken(2, 2, "2020", "NUM", 0, "root");
        year.lemma = QStringLiteral("2020");
        addToken(s, year);
        QTest::addRow("6.46_on_2020")
            << s << 1
            << 1
            << QStringLiteral("PREP-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.47 — in night → at (ночь)
    // in/ADP[case→night], night/NOUN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("in night"));
        RawToken in = makeRawToken(1, 1, "in", "ADP", 2, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken night = makeRawToken(2, 2, "night", "NOUN", 0, "root");
        night.lemma = QStringLiteral("night");
        addToken(s, night);
        QTest::addRow("6.47_in_night")
            << s << 1
            << 1
            << QStringLiteral("PREP-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.48 — at 5 o'clock → NO ERRORS (конкретное время, at корректен)
    // at/ADP[case→5], 5/NUM[nummod→o'clock], o'clock/NOUN[HEAD=0]
    // N=5 (NUM), parent(N)=o'clock → конкретное время → ожидаемый at
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("at 5 o'clock"));
        RawToken at = makeRawToken(1, 1, "at", "ADP", 2, "case");
        at.lemma = QStringLiteral("at");
        addToken(s, at);
        RawToken five = makeRawToken(2, 2, "5", "NUM", 3, "nummod");
        five.lemma = QStringLiteral("5");
        addToken(s, five);
        RawToken oclock = makeRawToken(3, 3, "o'clock", "NOUN", 0, "root");
        oclock.lemma = QStringLiteral("o'clock");
        addToken(s, oclock);
        QTest::addRow("6.48_at_5_oclock")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.49 — on Monday morning → NO ERRORS (исключение: compound с днём недели)
    // on/ADP[case→morning], Monday/PROPN[compound→morning], morning/NOUN[HEAD=0]
    // morning с compound-зависимым Monday → ожидаемый on, форма on совпадает
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("on Monday morning"));
        RawToken on = makeRawToken(1, 1, "on", "ADP", 3, "case");
        on.lemma = QStringLiteral("on");
        addToken(s, on);
        RawToken monday = makeRawToken(2, 2, "Monday", "PROPN", 3, "compound");
        monday.lemma = QStringLiteral("Monday");
        addToken(s, monday);
        RawToken morning = makeRawToken(3, 3, "morning", "NOUN", 0, "root");
        morning.lemma = QStringLiteral("morning");
        addToken(s, morning);
        QTest::addRow("6.49_on_Monday_morning")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_PREP001::TestRule()
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
    Rule_PREP001 rule;

    // Якорь — сам предлог ADP, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}