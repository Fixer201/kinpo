/*!
* \file TEST_Rule_CONJ001.cpp
* \brief Тесты для правила CONJ-001 (раздел 6.60–6.63).
*
* Проверяет правило "nor без отрицания":
*  — 6.60: apples nor oranges → CONJ-001 (nor без отрицания)
*  — 6.61: I do not want apples nor oranges → NO ERRORS (not снимает ошибку)
*  — 6.62: None of them wanted tea nor coffee → NO ERRORS (none в поддереве)
*  — 6.63: neither John nor Mary → NO ERRORS (C2.DEPREL=root, не conj)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj001.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-001 не использует словари, но загрузка сохраняет совместимость.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ001]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ001::TEST_Rule_CONJ001() {}
TEST_Rule_CONJ001::~TEST_Rule_CONJ001() {}

void TEST_Rule_CONJ001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.60 — apples nor oranges → CONJ-001 (nor без отрицания)
    // apples/NOUN[HEAD=0], nor/CCONJ[cc→oranges], oranges/NOUN[conj→apples]
    // C2=oranges (conj), C1=apples (root), поддерево apples нет отрицания
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("apples nor oranges"));
        RawToken apples = makeRawToken(1, 1, "apples", "NOUN", 0, "root");
        apples.lemma = QStringLiteral("apple");
        addToken(s, apples);
        RawToken nor = makeRawToken(2, 2, "nor", "CCONJ", 3, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken oranges = makeRawToken(3, 3, "oranges", "NOUN", 1, "conj");
        oranges.lemma = QStringLiteral("orange");
        addToken(s, oranges);
        QTest::addRow("6.60_apples_nor_oranges")
            << s << 2
            << 1
            << QStringLiteral("CONJ-001")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.61 — I do not want apples nor oranges → NO ERRORS (not снимает ошибку)
    // I/PRON[nsubj→want], do/AUX[aux→want], not/PART[advmod→want],
    // want/VERB[HEAD=0], apples/NOUN[obj→want], nor/CCONJ[cc→oranges],
    // oranges/NOUN[conj→apples]
    // C2=oranges (conj), C1=apples, root=want. Поддерево want содержит not.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("I do not want apples nor oranges"));
        RawToken i = makeRawToken(1, 1, "I", "PRON", 4, "nsubj");
        i.lemma = QStringLiteral("I");
        addToken(s, i);
        RawToken doTok = makeRawToken(2, 2, "do", "AUX", 4, "aux");
        doTok.lemma = QStringLiteral("do");
        addToken(s, doTok);
        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod");
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);
        RawToken want = makeRawToken(4, 4, "want", "VERB", 0, "root");
        want.lemma = QStringLiteral("want");
        addToken(s, want);
        RawToken apples = makeRawToken(5, 5, "apples", "NOUN", 4, "obj");
        apples.lemma = QStringLiteral("apple");
        addToken(s, apples);
        RawToken nor = makeRawToken(6, 6, "nor", "CCONJ", 7, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken oranges = makeRawToken(7, 7, "oranges", "NOUN", 5, "conj");
        oranges.lemma = QStringLiteral("orange");
        addToken(s, oranges);
        QTest::addRow("6.61_not_want_nor")
            << s << 6
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.62 — None of them wanted tea nor coffee → NO ERRORS (none в поддереве)
    // None/PRON[nsubj→wanted], of/ADP[case→them], them/PRON[nmod→None],
    // wanted/VERB[HEAD=0], tea/NOUN[obj→wanted], nor/CCONJ[cc→coffee],
    // coffee/NOUN[conj→tea]
    // C2=coffee (conj), C1=tea, root=wanted. Поддерево wanted содержит None.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("None of them wanted tea nor coffee"));
        RawToken none = makeRawToken(1, 1, "None", "PRON", 4, "nsubj");
        none.lemma = QStringLiteral("none");
        addToken(s, none);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 3, "case");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken them = makeRawToken(3, 3, "them", "PRON", 1, "nmod");
        them.lemma = QStringLiteral("they");
        addToken(s, them);
        RawToken wanted = makeRawToken(4, 4, "wanted", "VERB", 0, "root");
        wanted.lemma = QStringLiteral("want");
        addToken(s, wanted);
        RawToken tea = makeRawToken(5, 5, "tea", "NOUN", 4, "obj");
        tea.lemma = QStringLiteral("tea");
        addToken(s, tea);
        RawToken nor = makeRawToken(6, 6, "nor", "CCONJ", 7, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken coffee = makeRawToken(7, 7, "coffee", "NOUN", 5, "conj");
        coffee.lemma = QStringLiteral("coffee");
        addToken(s, coffee);
        QTest::addRow("6.62_none_nor")
            << s << 6
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }

    // 6.63 — neither John nor Mary → NO ERRORS (C2.DEPREL=root, не conj)
    // neither/CCONJ[cc:preconj→John], John/PROPN[conj→Mary],
    // nor/CCONJ[cc→Mary], Mary/PROPN[HEAD=0]
    // C2=Mary, C2.DEPREL=root (не conj) → правило не проверяется
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("neither John nor Mary"));
        RawToken neither = makeRawToken(1, 1, "neither", "CCONJ", 2, "cc:preconj");
        neither.lemma = QStringLiteral("neither");
        addToken(s, neither);
        RawToken john = makeRawToken(2, 2, "John", "PROPN", 4, "conj");
        john.lemma = QStringLiteral("John");
        addToken(s, john);
        RawToken nor = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken mary = makeRawToken(4, 4, "Mary", "PROPN", 0, "root");
        mary.lemma = QStringLiteral("Mary");
        addToken(s, mary);
        QTest::addRow("6.63_neither_nor")
            << s << 3
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_CONJ001::TestRule()
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
    Rule_CONJ001 rule;

    // Якорь — союз nor, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}