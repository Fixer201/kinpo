/*!
* \file TEST_Rule_CONJ002.cpp
* \brief Тесты для правила CONJ-002 (раздел 6.64–6.65).
*
* Проверяет правило "because ↔ because of":
*  — 6.64: because the rain → because of (именная группа, нет VERB/AUX в зависимых)
*  — 6.65: because of it rains → because (клауза, есть nsubj у HEAD)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj002.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-002 не использует словари, но загрузка сохраняет совместимость.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ002::TEST_Rule_CONJ002() {}
TEST_Rule_CONJ002::~TEST_Rule_CONJ002() {}

void TEST_Rule_CONJ002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.64 — because the rain → because of (именная группа)
    // because/SCONJ[mark→rain], the/DET[det→rain], rain/NOUN[HEAD=0]
    // HEAD(because)=rain/NOUN, у rain нет зависимых VERB/AUX → because of
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("because the rain"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 3, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken rain = makeRawToken(3, 3, "rain", "NOUN", 0, "root");
        rain.lemma = QStringLiteral("rain");
        addToken(s, rain);
        QTest::addRow("6.64_because_the_rain")
            << s << 1
            << 1
            << QStringLiteral("CONJ-002")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.65 — because of it rains → because (клауза)
    // because/SCONJ[mark→rains], of/ADP[fixed→because],
    // it/PRON[nsubj→rains], rains/VERB[HEAD=0]
    // HEAD(because)=rains/VERB, у rains есть зависимый it (nsubj) → because
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("because of it rains"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 4, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 1, "fixed");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 4, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rains = makeRawToken(4, 4, "rains", "VERB", 0, "root");
        rains.lemma = QStringLiteral("rain");
        addToken(s, rains);
        QTest::addRow("6.65_because_of_it_rains")
            << s << 1
            << 1
            << QStringLiteral("CONJ-002")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{1, 2}});
    }
}

void TEST_Rule_CONJ002::TestRule()
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
    Rule_CONJ002 rule;

    // Якорь — because (SCONJ), проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}