/*!
* \file TEST_Rule_OTHER004.cpp
* \brief DDT-тесты для правила OTHER-004 (раздел 6.93-6.95 тесты_v3.md).
*
* Проверяет правило «Неверный падеж местоимения»:
*  — 6.93: Me saw → Me→I (nsubj, должен быть именительный);
*  — 6.94: saw he → he→him (obj, должен быть винительный);
*  — 6.95: It is I → без ошибок (именительный после copula допустим).
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_OTHER004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_other004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Other004Expect
* \brief Точечные ожидания для тестов правила OTHER-004.
*/
struct Other004Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (PRON). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Other004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_OTHER004::TEST_Rule_OTHER004() {}
TEST_Rule_OTHER004::~TEST_Rule_OTHER004() {}

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
        qDebug() << "[TEST_Rule_OTHER004]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.93-6.95)
// ------------------------------------------------------------------------

void TEST_Rule_OTHER004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Other004Expect>("expect");

    // === 6.93 OTHER-004: Me → I (подлежащее) =======================
    // Me saw him. Me — PRON как nsubj, должен быть I.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("Me saw him"));
        RawToken me = makeRawToken(1, 1, "Me", "PRON", 2, "nsubj");
        me.lemma = QStringLiteral("I");
        addToken(s, me);
        RawToken saw = makeRawToken(2, 2, "saw", "VERB", 0, "root");
        saw.lemma = QStringLiteral("see");
        addToken(s, saw);
        RawToken him = makeRawToken(3, 3, "him", "PRON", 2, "obj");
        him.lemma = QStringLiteral("he");
        addToken(s, him);

        Other004Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-004");
        e.expectedDisplayIds = {1};
        e.expectedConflictIds = {1};

        QTest::addRow("6.93_me_saw") << s << e;
    }

    // === 6.94 OTHER-004: he → him (дополнение) =====================
    // saw he. he — PRON как obj, должен быть him.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("saw he"));
        RawToken saw = makeRawToken(1, 1, "saw", "VERB", 0, "root");
        saw.lemma = QStringLiteral("see");
        addToken(s, saw);
        RawToken he = makeRawToken(2, 2, "he", "PRON", 1, "obj");
        he.lemma = QStringLiteral("he");
        addToken(s, he);

        Other004Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("OTHER-004");
        e.expectedDisplayIds = {2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.94_saw_he") << s << e;
    }

    // === 6.95 OTHER-004 (исключение): It is I — без ошибок =========
    // Именительный после copula допустим.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("It is I"));
        RawToken it = makeRawToken(1, 1, "It", "PRON", 2, "nsubj");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken is = makeRawToken(2, 2, "is", "VERB", 0, "root");
        is.lemma = QStringLiteral("be");
        addToken(s, is);
        RawToken i = makeRawToken(3, 3, "I", "PRON", 2, "comp");
        i.lemma = QStringLiteral("I");
        addToken(s, i);

        Other004Expect e;
        e.anchorTokenId = 3;
        e.expectedCount = 0;

        QTest::addRow("6.95_it_is_I") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_OTHER004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Other004Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_OTHER004 rule;

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