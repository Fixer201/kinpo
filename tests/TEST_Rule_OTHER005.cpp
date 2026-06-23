/*!
* \file TEST_Rule_OTHER005.cpp
* \brief Тесты правила OTHER-005: двойное отрицание.
*
* Раздел 6.96–6.97 утверждённой версии тестов (тесты_v3.docx).
* Якорём служит глагол (VERB). Правило срабатывает, если у VERB есть
* прямой зависимый с Polarity=Neg и прямой зависимый-отрицательное
* местоимение/наречие (nothing, nobody, nowhere, never, none, neither,
* no one). Срабатывание — по одному кандидату на каждый отрицательный
* зависимый из списка замен:
*  6.96 — "He does not know nothing": nothing → anything;
*  6.97 — "I do not know nobody":    nobody → anybody.
*
* Входной якорь — токен-VERB (know). displayTokenIds/conflictTokenIds
* указывают на заменяемый отрицательный зависимый.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other005.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* OTHER-005 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил и единообразие запуска.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER005]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER005::TEST_Rule_OTHER005() {}
TEST_Rule_OTHER005::~TEST_Rule_OTHER005() {}

void TEST_Rule_OTHER005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.96: He does not know nothing.
    // Якорь — know/VERB[HEAD=0]. Прямые зависимые:
    //   He/PRON[nsubj], does/AUX[aux], not/PART[advmod, Polarity=Neg],
    //   nothing/PRON[obj].
    // Срабатывание: один кандидат на nothing (id=5), display=[5], conflict={5}.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.96"),
                                        QStringLiteral("He does not know nothing"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 4, "nsubj",
                                   QStringLiteral("Case=Nom|Gender=Masc|Number=Sing|Person=3|PronType=Prs"));
        he.lemma = QStringLiteral("he");
        addToken(s, he);

        RawToken does = makeRawToken(2, 2, "does", "AUX", 4, "aux",
                                     QStringLiteral("Mood=Ind|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin"));
        does.lemma = QStringLiteral("do");
        addToken(s, does);

        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod",
                                       QStringLiteral("Polarity=Neg"));
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);

        RawToken know = makeRawToken(4, 4, "know", "VERB", 0, "root",
                                     QStringLiteral("Mood=Ind|Number=Sing|Person=1|Tense=Pres|VerbForm=Fin"));
        know.lemma = QStringLiteral("know");
        addToken(s, know);

        RawToken nothing = makeRawToken(5, 5, "nothing", "PRON", 4, "obj",
                                         QStringLiteral("Number=Sing|PronType=Neg"));
        nothing.lemma = QStringLiteral("nothing");
        addToken(s, nothing);

        QTest::addRow("6.96_not_know_nothing")
            << s << 4
            << 1
            << QStringLiteral("OTHER-005")
            << (QList<QList<int>>{QList<int>{5}})
            << (QList<QSet<int>>{QSet<int>{5}});
    }

    // 6.97: I do not know nobody.
    // Якорь — know/VERB[HEAD=0]. Прямые зависимые:
    //   I/PRON[nsubj], do/AUX[aux], not/PART[advmod, Polarity=Neg],
    //   nobody/PRON[obj].
    // Срабатывание: один кандидат на nobody (id=5), display=[5], conflict={5}.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.97"),
                                        QStringLiteral("I do not know nobody"));
        RawToken iTok = makeRawToken(1, 1, "I", "PRON", 4, "nsubj",
                                     QStringLiteral("Case=Nom|Number=Sing|Person=1|PronType=Prs"));
        iTok.lemma = QStringLiteral("I");
        addToken(s, iTok);

        RawToken doTok = makeRawToken(2, 2, "do", "AUX", 4, "aux",
                                      QStringLiteral("Mood=Ind|Number=Sing|Person=1|Tense=Pres|VerbForm=Fin"));
        doTok.lemma = QStringLiteral("do");
        addToken(s, doTok);

        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod",
                                       QStringLiteral("Polarity=Neg"));
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);

        RawToken know = makeRawToken(4, 4, "know", "VERB", 0, "root",
                                     QStringLiteral("Mood=Ind|Number=Sing|Person=1|Tense=Pres|VerbForm=Fin"));
        know.lemma = QStringLiteral("know");
        addToken(s, know);

        RawToken nobody = makeRawToken(5, 5, "nobody", "PRON", 4, "obj",
                                       QStringLiteral("Number=Sing|PronType=Neg"));
        nobody.lemma = QStringLiteral("nobody");
        addToken(s, nobody);

        QTest::addRow("6.97_do_not_know_nobody")
            << s << 4
            << 1
            << QStringLiteral("OTHER-005")
            << (QList<QList<int>>{QList<int>{5}})
            << (QList<QSet<int>>{QSet<int>{5}});
    }
}

void TEST_Rule_OTHER005::TestRule()
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
    Rule_OTHER005 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}