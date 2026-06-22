/*!
* \file TEST_Rule_CONJ003.cpp
* \brief Тесты для правила CONJ-003 (раздел 6.66, 6.99–6.100).
*
* Проверяет правило "although ↔ despite":
*  — 6.66: although the rain → despite (именная группа)
*  — 6.99: although his efforts → despite (именная группа с притяжательным)
*  — 6.100: although it rains → NO ERRORS (клауза с глаголом)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj003.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-003 не использует словари, но загрузка сохраняет совместимость.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ003]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ003::TEST_Rule_CONJ003() {}
TEST_Rule_CONJ003::~TEST_Rule_CONJ003() {}

void TEST_Rule_CONJ003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.66 — although the rain → despite (именная группа)
    // although/SCONJ[mark→rain], the/DET[det→rain], rain/NOUN[HEAD=0]
    // HEAD(although)=rain/NOUN, у rain нет зависимых VERB/AUX → despite
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("although the rain"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken rain = makeRawToken(3, 3, "rain", "NOUN", 0, "root");
        rain.lemma = QStringLiteral("rain");
        addToken(s, rain);
        QTest::addRow("6.66_although_the_rain")
            << s << 1
            << 1
            << QStringLiteral("CONJ-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.99 — although his efforts → despite (именная группа с притяжательным)
    // although/SCONJ[mark→efforts], his/PRON[nmod:poss→efforts],
    // efforts/NOUN[HEAD=0]
    // HEAD(although)=efforts/NOUN, у efforts нет зависимых VERB/AUX → despite
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("although his efforts"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken his = makeRawToken(2, 2, "his", "PRON", 3, "nmod:poss",
                                     QStringLiteral("Poss=Yes"));
        his.lemma = QStringLiteral("his");
        addToken(s, his);
        RawToken efforts = makeRawToken(3, 3, "efforts", "NOUN", 0, "root");
        efforts.lemma = QStringLiteral("effort");
        addToken(s, efforts);
        QTest::addRow("6.99_although_his_efforts")
            << s << 1
            << 1
            << QStringLiteral("CONJ-003")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.100 — although it rains → NO ERRORS (клауза с глаголом)
    // although/SCONJ[mark→rains], it/PRON[nsubj→rains], rains/VERB[HEAD=0]
    // HEAD(although)=rains/VERB, не NOUN/PROPN → ветка (а) не срабатывает
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("although it rains"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 3, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken it = makeRawToken(2, 2, "it", "PRON", 3, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rains = makeRawToken(3, 3, "rains", "VERB", 0, "root");
        rains.lemma = QStringLiteral("rain");
        addToken(s, rains);
        QTest::addRow("6.100_although_it_rains")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_CONJ003::TestRule()
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
    Rule_CONJ003 rule;

    // Якорь — although (SCONJ), проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}