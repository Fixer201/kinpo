/*!
* \file TEST_Rule_DET005.cpp
* \brief Тесты для правила DET-005 (раздел 6.42).
*
* Проверяет правило "Множественное число неисчисляемого":
*  — informations → information (NOUN, Number=Plur, LEMMA=information ∈ uncountable.txt)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det005.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* Словарь uncountable.txt нужен для проверки LEMMA. Остальные словари
* загружаются для совместимости с остальными правилами.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_DET005]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_DET005::TEST_Rule_DET005() {}
TEST_Rule_DET005::~TEST_Rule_DET005() {}

void TEST_Rule_DET005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.42 — informations → information
    // NOUN во множественном числе, LEMMA=information есть в uncountable.txt
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("informations"));
        RawToken informations = makeRawToken(1, 1, "informations", "NOUN", 0, "root",
                                               QStringLiteral("Number=Plur"));
        // LEMMA — словарная форма единственного числа
        informations.lemma = QStringLiteral("information");
        addToken(s, informations);
        QTest::addRow("6.42_informations")
            << s << 1
            << 1
            << QStringLiteral("DET-005")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_DET005::TestRule()
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
    Rule_DET005 rule;

    // Якорь — сам NOUN, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}