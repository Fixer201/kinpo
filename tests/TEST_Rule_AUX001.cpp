/*!
* \file TEST_Rule_AUX001.cpp
* \brief DDT-тесты для правила AUX-001 (раздел 6.77-6.78 тесты_v3.md).
*
* Проверяет срабатывание правила при наличии do/did перед модальным
* вспомогательным глаголом. Ожидается удаление лишнего do/did.
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

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux001Expect
* \brief Точечные ожидания для тестов правила AUX-001.
*
* Все поля имеют значения по умолчанию, означающие «не проверять».
*/
struct Aux001Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (AUX). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux001Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX001::TEST_Rule_AUX001() {}
TEST_Rule_AUX001::~TEST_Rule_AUX001() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {

/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
* \return Runtime с заполненными ресурсами.
*
* Загружает словари из директории lists, обнаруженной через findListsDir.
* Предупреждения о загрузке выводятся в qDebug с тегом теста.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns) {
        qDebug() << "[TEST_Rule_AUX001]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.77-6.78)
// ------------------------------------------------------------------------

void TEST_Rule_AUX001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux001Expect>("expect");

    // === 6.77 AUX-001: do + модальный ================================
    // Вход: do can go. Ожидается: do→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("do can go"));
        RawToken d = makeRawToken(1, 1, "do", "AUX", 3, "aux");
        d.lemma = QStringLiteral("do");
        addToken(s, d);

        RawToken c = makeRawToken(2, 2, "can", "AUX", 3, "aux");
        c.lemma = QStringLiteral("can");
        addToken(s, c);

        RawToken g = makeRawToken(3, 3, "go", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        addToken(s, g);

        Aux001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.77_do_can") << s << e;
    }

    // === 6.78 AUX-001 (вариант): did + модальный =====================
    // Вход: did can go. Ожидается: did→-, кандидат на токене 1.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("did can go"));
        RawToken d = makeRawToken(1, 1, "did", "AUX", 3, "aux");
        d.lemma = QStringLiteral("do");
        addToken(s, d);

        RawToken c = makeRawToken(2, 2, "can", "AUX", 3, "aux");
        c.lemma = QStringLiteral("can");
        addToken(s, c);

        RawToken g = makeRawToken(3, 3, "go", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        addToken(s, g);

        Aux001Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-001");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.78_did_can") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux001Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    // Строим модель предложения из сырых данных.
    SentenceModel sentence = buildSentenceModel(rawSentence);

    // Достаём токен-якорь по anchorTokenId.
    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    // Создаём runtime с ресурсами и экземпляр правила.
    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_AUX001 rule;

    // Вызываем проверяемое правило.
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    // Проверяем количество кандидатов, если задано.
    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество кандидатов: ожидалось =" << expect.expectedCount
                     << "получено =" << actualCount;
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }

    // Если кандидатов не ожидается — выходим.
    if (expect.expectedCount == 0) {
        return;
    }

    // Проверяем единственного кандидата через хелпер.
    if (!result.isEmpty()) {
        compareSingleCandidate(tag, *result.begin(),
                               expect.expectedRuleId,
                               expect.expectedDisplayIds,
                               expect.expectedConflictIds);
    }
}