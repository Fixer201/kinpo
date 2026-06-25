/*!
* \file TEST_Rule_ART004.cpp
* \brief DDT-тесты для правила ART-004 (раздел 6.17 тесты_v3.md).
*
* Проверяет правило «Пропущен the перед географическим названием»:
*  — PROPN-группа из geo_the.txt без артикля.
*
* Обход всех PROPN: правило может сработать на нескольких группах
* в одном предложении.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art004Expect
* \brief Точечные ожидания для тестов правила ART-004.
*
* Структура не содержит anchorTokenId, так как правило использует
* паттерн обхода всех PROPN-токенов в предложении.
*/
struct Art004Expect {
    int expectedCount = -1;            ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;            ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<QSet<int>> expectedConflictZones; ///< Ожидаемые зоны конфликтов.
};

Q_DECLARE_METATYPE(Art004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART004::TEST_Rule_ART004() {}
TEST_Rule_ART004::~TEST_Rule_ART004() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {

/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns) {
        qDebug() << "[TEST_Rule_ART004]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.17)
// ------------------------------------------------------------------------

void TEST_Rule_ART004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art004Expect>("expect");

    // === 6.17 ART-004: пропущен the перед географическим названием ===
    // Вход: Pacific Ocean. Ожидается: Pacific Ocean→the Pacific Ocean.
    // Головной PROPN — Ocean (HEAD=0), Pacific — зависимый с flat:name.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("Pacific Ocean"));
        RawToken pacific = makeRawToken(1, 1, "Pacific", "PROPN", 2, "flat:name");
        pacific.lemma = QStringLiteral("Pacific");
        addToken(s, pacific);
        RawToken ocean = makeRawToken(2, 2, "Ocean", "PROPN", 0, "root");
        ocean.lemma = QStringLiteral("Ocean");
        addToken(s, ocean);

        Art004Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-004");
        e.expectedConflictZones = {QSet<int>{1, 2}};

        QTest::addRow("6.17_Pacific_Ocean") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art004Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_ART004 rule;

    // Обходим все PROPN: правило может сработать на нескольких группах.
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::PROPN) {
            continue;
        }
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found) {
            result.insert(ce);
        }
    }

    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество кандидатов: ожидалось =" << expect.expectedCount
                     << "получено =" << actualCount;
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }

    if (expect.expectedCount == 0) {
        return;
    }

    if (!expect.expectedConflictZones.isEmpty()) {
        compareConflictZones(tag, result, expect.expectedConflictZones);
    }

    if (!expect.expectedRuleId.isEmpty()) {
        for (const CandidateError& ce : result) {
            if (ce.ruleId != expect.expectedRuleId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "ruleId: ожидался =" << expect.expectedRuleId
                         << "получено =" << ce.ruleId;
            }
            QCOMPARE(ce.ruleId, expect.expectedRuleId);
        }
    }
}