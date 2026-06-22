/*!
* \file TEST_Rule_AUX006.cpp
* \brief Тесты для правила AUX-006 (раздел 6.85–6.86).
*
* 6.85: let him to go. Избыточная частица to после let с дополнением.
* 6.86: He was made to clean. Пассивная конструкция, to корректен.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux006.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-006 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX006]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX006::TEST_Rule_AUX006() {}
TEST_Rule_AUX006::~TEST_Rule_AUX006() {}

void TEST_Rule_AUX006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.85: let him to go. to удаляется, так как после let
    // с дополнением инфинитив идёт без to.
    // let/VERB[HEAD=0], him/PRON[obj→let], to/PART[mark→go],
    // go/VERB[xcomp→let]
    // T=to (id=3), V2=go (id=4, xcomp), V=let (id=1), OBJ=him (id=2)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("let him to go"));
        RawToken let = makeRawToken(1, 1, "let", "VERB", 0, "root");
        let.lemma = QStringLiteral("let");
        addToken(s, let);
        RawToken him = makeRawToken(2, 2, "him", "PRON", 1, "obj");
        him.lemma = QStringLiteral("he");
        addToken(s, him);
        RawToken to = makeRawToken(3, 3, "to", "PART", 4, "mark");
        to.lemma = QStringLiteral("to");
        addToken(s, to);
        RawToken go = makeRawToken(4, 4, "go", "VERB", 1, "xcomp");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        QTest::addRow("6.85_let_him_to_go")
            << s << 3
            << 1
            << QStringLiteral("AUX-006")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.86: He was made to clean. Пассивная конструкция, to корректен.
    // He/PRON[nsubj:pass→made], was/AUX[aux:pass→made], made/VERB[HEAD=0],
    // to/PART[mark→clean], clean/VERB[xcomp→made]
    // T=to (id=4), V2=clean (id=5, xcomp), V=made (id=3),
    // у V есть aux:pass=was (id=2) → исключение, NO ERRORS
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("He was made to clean"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 3, "nsubj:pass");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken was = makeRawToken(2, 2, "was", "AUX", 3, "aux:pass");
        was.lemma = QStringLiteral("be");
        addToken(s, was);
        RawToken made = makeRawToken(3, 3, "made", "VERB", 0, "root");
        made.lemma = QStringLiteral("make");
        addToken(s, made);
        RawToken to = makeRawToken(4, 4, "to", "PART", 5, "mark");
        to.lemma = QStringLiteral("to");
        addToken(s, to);
        RawToken clean = makeRawToken(5, 5, "clean", "VERB", 3, "xcomp");
        clean.lemma = QStringLiteral("clean");
        addToken(s, clean);
        QTest::addRow("6.86_made_to_clean_passive")
            << s << 4
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_AUX006::TestRule()
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
    Rule_AUX006 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}