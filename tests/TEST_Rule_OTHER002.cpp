/*!
* \file TEST_Rule_OTHER002.cpp
* \brief Тесты для правила OTHER-002 (раздел 6.89–6.90).
*
* 6.89: He works good. good в наречной позиции, заменяется на well.
* 6.90: He does good. good как существительное (NOUN, obj), ошибок нет.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other002.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* OTHER-002 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER002]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER002::TEST_Rule_OTHER002() {}
TEST_Rule_OTHER002::~TEST_Rule_OTHER002() {}

void TEST_Rule_OTHER002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.89: He works good. good в наречной позиции, заменяется на well.
    // He/PRON[nsubj→works], works/VERB[HEAD=0], good/ADJ[advmod→works]
    // Якорь=good (id=3), V=works (id=2, VERB, LEMMA=work ≠ do)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("He works good"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 2, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken works = makeRawToken(2, 2, "works", "VERB", 0, "root");
        works.lemma = QStringLiteral("work");
        addToken(s, works);
        RawToken good = makeRawToken(3, 3, "good", "ADJ", 2, "advmod");
        good.lemma = QStringLiteral("good");
        addToken(s, good);
        QTest::addRow("6.89_works_good")
            << s << 3
            << 1
            << QStringLiteral("OTHER-002")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.90: He does good. good как существительное, ошибок нет.
    // He/PRON[nsubj→does], does/VERB[HEAD=0], good/NOUN[obj→does]
    // good имеет UPOS=NOUN и DEPREL=obj, правило не срабатывает.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("He does good"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 2, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken does = makeRawToken(2, 2, "does", "VERB", 0, "root");
        does.lemma = QStringLiteral("do");
        addToken(s, does);
        RawToken good = makeRawToken(3, 3, "good", "NOUN", 2, "obj");
        good.lemma = QStringLiteral("good");
        addToken(s, good);
        QTest::addRow("6.90_does_good")
            << s << 3
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_OTHER002::TestRule()
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
    Rule_OTHER002 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}