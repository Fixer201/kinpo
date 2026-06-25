/*!
* \file TEST_Rule_DET003.cpp
* \brief DDT-тесты для правила DET-003 (раздел 6.37-6.39 тесты_v3.md).
*
* Проверяет правило «Притяжательный омофон как подлежащее»:
*  — its → it's (Poss=Yes, nsubj, HEAD=VERB);
*  — their → they're (Poss=Yes, nsubj, HEAD=VERB);
*  — исключение: nmod:poss (a bird in its nest — без ошибок).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET003.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det003.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Det003Expect
* \brief Точечные ожидания для тестов правила DET-003.
*/
struct Det003Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (PRON/DET). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Det003Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_DET003::TEST_Rule_DET003() {}
TEST_Rule_DET003::~TEST_Rule_DET003() {}

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
        qDebug() << "[TEST_Rule_DET003]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.37-6.39)
// ------------------------------------------------------------------------

void TEST_Rule_DET003::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Det003Expect>("expect");

    // === 6.37 DET-003: Its → It's ==================================
    // its PRON, Poss=Yes, nsubj, HEAD raining VERB.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("Its raining"));
        RawToken its = makeRawToken(1, 1, "Its", "PRON", 2, "nsubj",
                                   QStringLiteral("Poss=Yes"));
        its.lemma = QStringLiteral("its");
        addToken(s, its);
        RawToken raining = makeRawToken(2, 2, "raining", "VERB", 0, "root");
        raining.lemma = QStringLiteral("rain");
        addToken(s, raining);

        Det003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.37_Its_raining") << s << e;
    }

    // === 6.38 DET-003: Their → They're =============================
    // their PRON, Poss=Yes, nsubj, HEAD coming VERB.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("Their coming"));
        RawToken their = makeRawToken(1, 1, "Their", "PRON", 2, "nsubj",
                                      QStringLiteral("Poss=Yes"));
        their.lemma = QStringLiteral("their");
        addToken(s, their);
        RawToken coming = makeRawToken(2, 2, "coming", "VERB", 0, "root");
        coming.lemma = QStringLiteral("come");
        addToken(s, coming);

        Det003Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-003");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.38_Their_coming") << s << e;
    }

    // === 6.39 DET-003 (исключение): nmod:poss =====================
    // a bird in its nest — its nmod:poss, без ошибок.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a bird in its nest"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken bird = makeRawToken(2, 2, "bird", "NOUN", 0, "root");
        bird.lemma = QStringLiteral("bird");
        addToken(s, bird);
        RawToken in = makeRawToken(3, 3, "in", "ADP", 5, "case");
        in.lemma = QStringLiteral("in");
        addToken(s, in);
        RawToken its = makeRawToken(4, 4, "its", "PRON", 5, "nmod:poss",
                                    QStringLiteral("Poss=Yes"));
        its.lemma = QStringLiteral("its");
        addToken(s, its);
        RawToken nest = makeRawToken(5, 5, "nest", "NOUN", 2, "nmod");
        nest.lemma = QStringLiteral("nest");
        addToken(s, nest);

        Det003Expect e;
        e.anchorTokenId = 4;
        e.expectedCount = 0;

        QTest::addRow("6.39_a_bird_in_its_nest") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_DET003::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Det003Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET003 rule;

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