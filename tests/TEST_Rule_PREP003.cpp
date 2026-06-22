/*!
* \file TEST_Rule_PREP003.cpp
* \brief Тесты для правила PREP-003 (раздел 6.52–6.53).
*
* Проверяет правило "while ↔ during":
*  — 6.52: while перед именной группой (meeting/NOUN) → during
*  — 6.53: while перед клаузой (rained/VERB с зависимым nsubj) → NO ERRORS
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep003.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* PREP-003 не использует словари напрямую, но загрузка сохраняет совместимость
* с остальными правилами и тестами.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_PREP003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_PREP003::TEST_Rule_PREP003() {}
TEST_Rule_PREP003::~TEST_Rule_PREP003() {}

void TEST_Rule_PREP003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.52 — while the meeting → during (именная группа)
    // while/SCONJ[mark→meeting], the/DET[det→meeting], meeting/NOUN[HEAD=0]
    // HEAD(while)=meeting/NOUN, у meeting нет зависимых VERB/AUX → during
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("while the meeting"));
        RawToken whileTok = makeRawToken(1, 1, "while", "SCONJ", 3, "mark");
        whileTok.lemma = QStringLiteral("while");
        addToken(s, whileTok);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken meeting = makeRawToken(3, 3, "meeting", "NOUN", 0, "root");
        meeting.lemma = QStringLiteral("meeting");
        addToken(s, meeting);
        QTest::addRow("6.52_while_the_meeting")
            << s << 1
            << 1
            << QStringLiteral("PREP-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.53 — while it rained → NO ERRORS (клауза, VERB в зависимых)
    // while/SCONJ[mark→rained], it/PRON[nsubj→rained], rained/VERB[HEAD=0]
    // HEAD(while)=rained/VERB, не NOUN/PROPN → ветка (а) не срабатывает
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("while it rained"));
        RawToken whileTok = makeRawToken(1, 1, "while", "SCONJ", 3, "mark");
        whileTok.lemma = QStringLiteral("while");
        addToken(s, whileTok);
        RawToken it = makeRawToken(2, 2, "it", "PRON", 3, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rained = makeRawToken(3, 3, "rained", "VERB", 0, "root");
        rained.lemma = QStringLiteral("rain");
        addToken(s, rained);
        QTest::addRow("6.53_while_it_rained")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_PREP003::TestRule()
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
    Rule_PREP003 rule;

    // Якорь — сам маркер while/during, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}