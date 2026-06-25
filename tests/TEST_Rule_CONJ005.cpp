/*!
* \file TEST_Rule_CONJ005.cpp
* \brief DDT-тесты для правила CONJ-005 (раздел 6.73-6.74 тесты_v3.md).
*
* Проверяет правило «if ↔ whether»:
*  — 6.73: if to go or not → if→whether;
*  — 6.74: whether to go or not → без ошибок (корректный whether).
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

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj005Expect
* \brief Точечные ожидания для тестов правила CONJ-005.
*/
struct Conj005Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (SCONJ). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Conj005Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ005::TEST_Rule_CONJ005() {}
TEST_Rule_CONJ005::~TEST_Rule_CONJ005() {}

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
        qDebug() << "[TEST_Rule_CONJ005]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.73-6.74)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj005Expect>("expect");

    // === 6.73 CONJ-005: if to go or not → if→whether ================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("if to go or not"));
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

        Conj005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-005");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.73_if_or_not") << s << e;
    }

    // === 6.74 CONJ-005 (положительный): whether to go or not =======
    // Корректный whether, без ошибок.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("whether to go or not"));
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

        Conj005Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.74_whether_or_not") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ005::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj005Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_CONJ005 rule;

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