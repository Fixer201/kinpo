/*!
* \file TEST_Rule_CONJ005.cpp
* \brief Тесты для правила CONJ-005 (раздел 6.73–6.74).
*
* 6.73: if to go or not — if заменяется на whether.
* 6.74: whether to go or not — конструкция корректна, ошибок нет.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj005.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-005 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ005]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ005::TEST_Rule_CONJ005() {}
TEST_Rule_CONJ005::~TEST_Rule_CONJ005() {}

void TEST_Rule_CONJ005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.73: if to go or not → CONJ-005, if→whether
    // if — маркер клаузы go, в поддереве go есть or, за or следует not (conj)
    // Все условия правила выполнены, if заменяется на whether
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("if to go or not"));
        RawToken ifTok = makeRawToken(1, 1, "if", "SCONJ", 3, "mark");
        ifTok.lemma = QStringLiteral("if");
        addToken(s, ifTok);
        RawToken to = makeRawToken(2, 2, "to", "PART", 3, "mark");
        to.lemma = QStringLiteral("to");
        addToken(s, to);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        RawToken orTok = makeRawToken(4, 4, "or", "CCONJ", 5, "cc");
        orTok.lemma = QStringLiteral("or");
        addToken(s, orTok);
        RawToken notTok = makeRawToken(5, 5, "not", "PART", 3, "conj");
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);
        QTest::addRow("6.73_if_or_not")
            << s << 1
            << 1
            << QStringLiteral("CONJ-005")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.74: whether to go or not → ошибок нет
    // whether корректен для данной конструкции, правило не должно срабатывать
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("whether to go or not"));
        RawToken whether = makeRawToken(1, 1, "whether", "SCONJ", 3, "mark");
        whether.lemma = QStringLiteral("whether");
        addToken(s, whether);
        RawToken to = makeRawToken(2, 2, "to", "PART", 3, "mark");
        to.lemma = QStringLiteral("to");
        addToken(s, to);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        RawToken orTok = makeRawToken(4, 4, "or", "CCONJ", 5, "cc");
        orTok.lemma = QStringLiteral("or");
        addToken(s, orTok);
        RawToken notTok = makeRawToken(5, 5, "not", "PART", 3, "conj");
        notTok.lemma = QStringLiteral("not");
        addToken(s, notTok);
        QTest::addRow("6.74_whether_or_not")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_CONJ005::TestRule()
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
    Rule_CONJ005 rule;

    // Якорь — союз if/whether, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}