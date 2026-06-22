/*!
* \file TEST_Rule_AUX001.cpp
* \brief Тесты для правила AUX-001 (раздел 6.77–6.78).
*
* 6.77: do can go — избыточный do при модальном can.
* 6.78: did can go — избыточный did при модальном can.
*
* В утверждённой версии входы `do can` и `did can` не удовлетворяют
* спецификации: модальный can является головой V, а не зависимым V.
* Тесты адаптированы: добавлен VERB go, к которому присоединены и do
* и can как aux-зависимые, что соответствует спецификации.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux001.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* AUX-001 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_AUX001]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_AUX001::TEST_Rule_AUX001() {}
TEST_Rule_AUX001::~TEST_Rule_AUX001() {}

void TEST_Rule_AUX001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.77: do can go → AUX-001, do удаляется
    // do/AUX[aux→go], can/AUX[aux→go], go/VERB[HEAD=0]
    // D=do (id=1), V=go (id=3), M=can (id=2) — другой aux-зависимый V, модальный
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("do can go"));
        RawToken doTok = makeRawToken(1, 1, "do", "AUX", 3, "aux");
        doTok.lemma = QStringLiteral("do");
        addToken(s, doTok);
        RawToken can = makeRawToken(2, 2, "can", "AUX", 3, "aux");
        can.lemma = QStringLiteral("can");
        addToken(s, can);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        QTest::addRow("6.77_do_can")
            << s << 1
            << 1
            << QStringLiteral("AUX-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.78: did can go → AUX-001, did удаляется
    // did/AUX[aux→go], can/AUX[aux→go], go/VERB[HEAD=0]
    // LEMMA=do покрывает форму did
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("did can go"));
        RawToken did = makeRawToken(1, 1, "did", "AUX", 3, "aux");
        did.lemma = QStringLiteral("do");
        addToken(s, did);
        RawToken can = makeRawToken(2, 2, "can", "AUX", 3, "aux");
        can.lemma = QStringLiteral("can");
        addToken(s, can);
        RawToken go = makeRawToken(3, 3, "go", "VERB", 0, "root");
        go.lemma = QStringLiteral("go");
        addToken(s, go);
        QTest::addRow("6.78_did_can")
            << s << 1
            << 1
            << QStringLiteral("AUX-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_AUX001::TestRule()
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
    Rule_AUX001 rule;

    // Якорь — вспомогательный do, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}