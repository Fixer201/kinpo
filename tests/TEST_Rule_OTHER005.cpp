/*!
* \file TEST_Rule_OTHER005.cpp
* \brief DDT-тесты для правила OTHER-005 (раздел 6.96-6.97 тесты_v3.md).
*
* Проверяет правило «Двойное отрицание»:
*  — 6.96: He does not know nothing → nothing→anything;
*  — 6.97: He does not know nobody → nobody→anybody.
*
* Якорь — VERB (know), правило проверяет obj-зависимый с Polarity=Neg.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other005.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other005Expect
* \brief Точечные ожидания для тестов правила OTHER-005.
*/
struct Other005Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (VERB). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other005Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER005::TEST_Rule_OTHER005() {}
TEST_Rule_OTHER005::~TEST_Rule_OTHER005() {}

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
        qDebug() << "[TEST_Rule_OTHER005]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.96-6.97)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other005Expect>("expect");

    // === 6.96 OTHER-005: двойное отрицание (nothing) ===============
    // He does not know nothing → nothing→anything.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He does not know nothing"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 4, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken does = makeRawToken(2, 2, "does", "AUX", 4, "aux");
        does.lemma = QStringLiteral("do");
        addToken(s, does);
        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod");
        notTok.lemma = QStringLiteral("not");
        notTok.featsRaw = QStringLiteral("Polarity=Neg");
        addToken(s, notTok);
        RawToken know = makeRawToken(4, 4, "know", "VERB", 0, "root");
        know.lemma = QStringLiteral("know");
        addToken(s, know);
        RawToken nothing = makeRawToken(5, 5, "nothing", "PRON", 4, "obj");
        nothing.lemma = QStringLiteral("nothing");
        nothing.featsRaw = QStringLiteral("PronType=Neg");
        addToken(s, nothing);

        Other005Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-005");
        e.expectedDisplayIds = {5};
        e.expectedConflictIds = {5};

        QTest::addRow("6.96_not_know_nothing") << s << e;
    }

    // === 6.97 OTHER-005: двойное отрицание (nobody) ================
    // He does not know nobody → nobody→anybody.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("He does not know nobody"));
        RawToken he = makeRawToken(1, 1, "He", "PRON", 4, "nsubj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        RawToken does = makeRawToken(2, 2, "does", "AUX", 4, "aux");
        does.lemma = QStringLiteral("do");
        addToken(s, does);
        RawToken notTok = makeRawToken(3, 3, "not", "PART", 4, "advmod");
        notTok.lemma = QStringLiteral("not");
        notTok.featsRaw = QStringLiteral("Polarity=Neg");
        addToken(s, notTok);
        RawToken know = makeRawToken(4, 4, "know", "VERB", 0, "root");
        know.lemma = QStringLiteral("know");
        addToken(s, know);
        RawToken nobody = makeRawToken(5, 5, "nobody", "PRON", 4, "obj");
        nobody.lemma = QStringLiteral("nobody");
        nobody.featsRaw = QStringLiteral("PronType=Neg");
        addToken(s, nobody);

        Other005Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-005");
        e.expectedDisplayIds = {5};
        e.expectedConflictIds = {5};

        QTest::addRow("6.97_not_know_nobody") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER005::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other005Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_OTHER005 rule;

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