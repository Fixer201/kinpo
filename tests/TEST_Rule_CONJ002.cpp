/*!
* \file TEST_Rule_CONJ002.cpp
* \brief DDT-тесты для правила CONJ-002 (раздел 6.64-6.65 тесты_v3.md).
*
* Проверяет правило «because ↔ because of»:
*  — because перед именной группой (the rain) → because of;
*  — because of перед клаузой (it rains) → because.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj002Expect
* \brief Точечные ожидания для тестов правила CONJ-002.
*/
struct Conj002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (SCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ002::TEST_Rule_CONJ002() {}
TEST_Rule_CONJ002::~TEST_Rule_CONJ002() {}

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
        qDebug() << "[TEST_Rule_CONJ002]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.64-6.65)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj002Expect>("expect");

    // === 6.64 CONJ-002: because the rain → because of (именная группа)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("because the rain"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 3, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken rain = makeRawToken(3, 3, "rain", "NOUN", 0, "root");
        rain.lemma = QStringLiteral("rain");
        addToken(s, rain);

        Conj002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-002");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.64_because_the_rain") << s << e;
    }

    // === 6.65 CONJ-002: because of it rains → because (клауза) =====
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("because of it rains"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 4, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 1, "fixed");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 4, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken rains = makeRawToken(4, 4, "rains", "VERB", 0, "root");
        rains.lemma = QStringLiteral("rain");
        addToken(s, rains);

        Conj002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {1, 2};

        QTest::addRow("6.65_because_of_it_rains") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj002Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_CONJ002 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

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