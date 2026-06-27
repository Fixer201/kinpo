/*!
* \file TEST_Rule_AUX004.cpp
* \brief DDT-тесты для правила AUX-004 (раздел 6.82 тесты_v3.md).
*
* Проверяет срабатывание правила при наличии двух модальных глаголов.
* Ожидается удаление второго модального. Паттерн: обход всех AUX.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_AUX004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_aux004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Aux004Expect
* \brief Точечные ожидания для тестов правила AUX-004.
*
* Структура не содержит anchorTokenId, так как правило использует
* паттерн обхода всех AUX-токенов в предложении.
*/
struct Aux004Expect {
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Aux004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_AUX004::TEST_Rule_AUX004() {}
TEST_Rule_AUX004::~TEST_Rule_AUX004() {}

// ------------------------------------------------------------------------
// Вспомогательная функция создания runtime с ресурсами
// ------------------------------------------------------------------------

namespace {
/*!
* \brief Создаёт CheckerRuntime с загруженными словарями.
*/
} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.82)
// ------------------------------------------------------------------------

void TEST_Rule_AUX004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Aux004Expect>("expect");

    // === 6.82 AUX-004: два модальных =================================
    // Вход: can will go. Ожидается: will→-, кандидат на токене 2.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("can will go"));
        RawToken c = makeRawToken(1, 1, "can", "AUX", 3, "aux");
        c.lemma = QStringLiteral("can");
        addToken(s, c);

        RawToken w = makeRawToken(2, 2, "will", "AUX", 3, "aux");
        w.lemma = QStringLiteral("will");
        addToken(s, w);

        RawToken g = makeRawToken(3, 3, "go", "VERB", 0, "root");
        g.lemma = QStringLiteral("go");
        addToken(s, g);

        Aux004Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("AUX-004");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.82_can_will_go") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_AUX004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Aux004Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_AUX004 rule;

    // Обходим все AUX-токены: правило проверяет пары модальных.
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::Aux) {
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

    if (!result.isEmpty()) {
        compareSingleCandidate(tag, *result.begin(),
                               expect.expectedRuleId,
                               expect.expectedDisplayIds,
                               expect.expectedConflictIds);
    }
}