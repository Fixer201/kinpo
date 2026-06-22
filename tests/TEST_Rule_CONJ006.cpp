/*!
* \file TEST_Rule_CONJ006.cpp
* \brief Тесты для правила CONJ-006 (раздел 6.75–6.76).
*
* 6.75: because A, so B — избыточный so при подчинительном союзе because.
* 6.76: although A, but B — избыточный but при подчинительном союзе although.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj006.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-006 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ006]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ006::TEST_Rule_CONJ006() {}
TEST_Rule_CONJ006::~TEST_Rule_CONJ006() {}

void TEST_Rule_CONJ006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.75: because A, so B → CONJ-006, so удаляется
    // because/SCONJ[mark→A], A/PROPN[advcl→B], so/ADV[advmod→B], B/PROPN[HEAD=0]
    // S=because, V1=A (advcl), V2=B, C=so (HEAD=B=V2)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("because A, so B"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 2, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "advcl");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken so = makeRawToken(3, 3, "so", "ADV", 4, "advmod");
        so.lemma = QStringLiteral("so");
        addToken(s, so);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        QTest::addRow("6.75_because_so")
            << s << 3
            << 1
            << QStringLiteral("CONJ-006")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.76: although A, but B → CONJ-006, but удаляется
    // although/SCONJ[mark→A], A/PROPN[advcl→B], but/CCONJ[cc→B], B/PROPN[HEAD=0]
    // S=although, V1=A (advcl), V2=B, C=but (HEAD=B=V2)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("although A, but B"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 2, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "advcl");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken but = makeRawToken(3, 3, "but", "CCONJ", 4, "cc");
        but.lemma = QStringLiteral("but");
        addToken(s, but);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        QTest::addRow("6.76_although_but")
            << s << 3
            << 1
            << QStringLiteral("CONJ-006")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }
}

void TEST_Rule_CONJ006::TestRule()
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
    Rule_CONJ006 rule;

    // Якорь — координатор (so/but), проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}