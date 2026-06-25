/*!
* \file TEST_Rule_CONJ006.cpp
* \brief DDT-тесты для правила CONJ-006 (раздел 6.75-6.76 тесты_v3.md).
*
* Проверяет правило «Избыточный союз»:
*  — 6.75: because A, so B → so удаляется;
*  — 6.76: although A, but B → but удаляется.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ006.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj006.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj006Expect
* \brief Точечные ожидания для тестов правила CONJ-006.
*/
struct Conj006Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADV/CCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj006Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ006::TEST_Rule_CONJ006() {}
TEST_Rule_CONJ006::~TEST_Rule_CONJ006() {}

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
        qDebug() << "[TEST_Rule_CONJ006]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.75-6.76)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ006::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj006Expect>("expect");

    // === 6.75 CONJ-006: because A, so B → so удаляется =============
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("because A, so B"));
        RawToken because = makeRawToken(1, 1, "because", "SCONJ", 2, "mark");
        because.lemma = QStringLiteral("because");
        addToken(s, because);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "advcl");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken so = makeRawToken(3, 3, "so", "ADV", 4, "advmod");
        so.lemma = QStringLiteral("so");
        addToken(s, so);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);

        Conj006Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-006");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.75_because_so") << s << e;
    }

    // === 6.76 CONJ-006: although A, but B → but удаляется =========
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("although A, but B"));
        RawToken although = makeRawToken(1, 1, "although", "SCONJ", 2, "mark");
        although.lemma = QStringLiteral("although");
        addToken(s, although);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "advcl");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken but = makeRawToken(3, 3, "but", "CCONJ", 4, "cc");
        but.lemma = QStringLiteral("but");
        addToken(s, but);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);

        Conj006Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-006");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.76_although_but") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ006::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj006Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_CONJ006 rule;

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