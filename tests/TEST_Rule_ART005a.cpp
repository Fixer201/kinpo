/*!
* \file TEST_Rule_ART005a.cpp
* \brief тесты для правила ART-005a (раздел 6.18–6.20).
*
* Проверяет правило "Лишний the перед титулом + имя":
*  — the перед титулом + PROPN (nmod:desc)
*  — исключение: Reverend / Honorable
*  — описание, не титул (nmod:desc, не в titles.txt) — не срабатывает
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART005a.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art005a.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_ART005a]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_ART005a::TEST_Rule_ART005a() {}
TEST_Rule_ART005a::~TEST_Rule_ART005a() {}

void TEST_Rule_ART005a::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.18 — the перед титулом + PROPN (nmod:desc)
    // President — NOUN, nmod:desc к Obama, в titles.txt → ошибка
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the President Obama"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken president = makeRawToken(2, 2, "President", "NOUN", 3, "nmod:desc");
        president.lemma = QStringLiteral("President");
        addToken(s, president);
        RawToken obama = makeRawToken(3, 3, "Obama", "PROPN", 0, "root");
        obama.lemma = QStringLiteral("Obama");
        addToken(s, obama);
        QTest::addRow("6.18_the_President_Obama")
            << s << 2
            << 1
            << QStringLiteral("ART-005a")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.19 — исключение: Reverend
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the Reverend King"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken reverend = makeRawToken(2, 2, "Reverend", "NOUN", 3, "nmod:desc");
        reverend.lemma = QStringLiteral("Reverend");
        addToken(s, reverend);
        RawToken king = makeRawToken(3, 3, "King", "PROPN", 0, "root");
        king.lemma = QStringLiteral("King");
        addToken(s, king);
        QTest::addRow("6.19_the_Reverend_King")
            << s << 2
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.20 — описание, не титул
    // actor — NOUN с appos к Pitt, не в titles.txt → не срабатывает
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the actor Brad Pitt"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 2, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken actor = makeRawToken(2, 2, "actor", "NOUN", 4, "appos");
        actor.lemma = QStringLiteral("actor");
        addToken(s, actor);
        RawToken brad = makeRawToken(3, 3, "Brad", "PROPN", 4, "flat:name");
        brad.lemma = QStringLiteral("Brad");
        addToken(s, brad);
        RawToken pitt = makeRawToken(4, 4, "Pitt", "PROPN", 0, "root");
        pitt.lemma = QStringLiteral("Pitt");
        addToken(s, pitt);
        QTest::addRow("6.20_the_actor_Brad_Pitt")
            << s << 2
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_ART005a::TestRule()
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
    Rule_ART005a rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}