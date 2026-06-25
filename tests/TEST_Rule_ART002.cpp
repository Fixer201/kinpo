/*!
* \file TEST_Rule_ART002.cpp
* \brief DDT-тесты для правил ART-002 и ART-002a (раздел 6.7-6.12 тесты_v3.md).
*
* Проверяет срабатывание и исключения правила «Пропущен the перед ADJ»:
*  — превосходная степень (Degree=Sup);
*  — порядковое числительное (NumType=Ord);
*  — аналитическая превосходная степень (most + ADJ), правило ART-002a;
*  — артикль уже присутствует (the/a/an);
*  — исключение: last/next + единица времени.
*
* Тест 6.9 использует якорь ADV и правило ART-002a.
* Остальные тесты используют якорь ADJ и правило ART-002.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_ART002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_art002.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Art002Expect
* \brief Точечные ожидания для тестов правил ART-002/ART-002a.
*/
struct Art002Expect {
    int anchorTokenId = -1;        ///< ID токена-якоря (ADJ или ADV). -1: не проверять.
    int expectedCount = -1;        ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;        ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds; ///< Ожидаемые displayTokenIds.
    QSet<int> expectedConflictIds;  ///< Ожидаемые conflictTokenIds.
};

Q_DECLARE_METATYPE(Art002Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_ART002::TEST_Rule_ART002() {}
TEST_Rule_ART002::~TEST_Rule_ART002() {}

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
        qDebug() << "[TEST_Rule_ART002]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.7-6.12)
// ------------------------------------------------------------------------

void TEST_Rule_ART002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Art002Expect>("expect");

    // === 6.7 ART-002: превосходная степень (Sup) ====================
    // Вход: best solution. Ожидается: добавить the перед best.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("best solution"));
        RawToken best = makeRawToken(1, 1, "best", "ADJ", 2, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(2, 2, "solution", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.7_best_solution") << s << e;
    }

    // === 6.7a ART-002 (negative): a перед превосходной ==============
    // Вход: a best solution. Ожидается: NO ERRORS (артикль уже есть).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("a best solution"));
        addToken(s, makeRawToken(1, 1, "a", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 0;

        QTest::addRow("6.7a_a_best_solution") << s << e;
    }

    // === 6.7b ART-002 (negative): an перед превосходной =============
    // Вход: an best solution. Ожидается: NO ERRORS (артикль уже есть).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("an best solution"));
        addToken(s, makeRawToken(1, 1, "an", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 0;

        QTest::addRow("6.7b_an_best_solution") << s << e;
    }

    // === 6.8 ART-002: порядковое числительное (Ord) =================
    // Вход: second chance. Ожидается: добавить the перед second.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("second chance"));
        RawToken second = makeRawToken(1, 1, "second", "ADJ", 2, "amod", "NumType=Ord");
        second.lemma = QStringLiteral("second");
        addToken(s, second);
        addToken(s, makeRawToken(2, 2, "chance", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-002");
        e.expectedDisplayIds = {1, 2};
        e.expectedConflictIds = {2};

        QTest::addRow("6.8_second_chance") << s << e;
    }

    // === 6.9 ART-002a: аналитическая превосходная степень ===========
    // Вход: most beautiful girl. Ожидается: добавить the перед most.
    // Якорь — ADV, вызывается Rule_ART002a.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("most beautiful girl"));
        RawToken most = makeRawToken(1, 1, "most", "ADV", 2, "advmod");
        most.lemma = QStringLiteral("most");
        addToken(s, most);
        addToken(s, makeRawToken(2, 2, "beautiful", "ADJ", 3, "amod"));
        addToken(s, makeRawToken(3, 3, "girl", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("ART-002a");
        e.expectedDisplayIds = {1, 2, 3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.9_most_beautiful_girl") << s << e;
    }

    // === 6.10 ART-002 (negative): артикль the уже есть =============
    // Вход: the best solution. Ожидается: NO ERRORS (the уже есть).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("the best solution"));
        addToken(s, makeRawToken(1, 1, "the", "DET", 3, "det"));
        RawToken best = makeRawToken(2, 2, "best", "ADJ", 3, "amod", "Degree=Sup");
        best.lemma = QStringLiteral("best");
        addToken(s, best);
        addToken(s, makeRawToken(3, 3, "solution", "NOUN", 0, "root"));

        Art002Expect e;
        e.anchorTokenId = 2;
        e.expectedCount = 0;

        QTest::addRow("6.10_the_best_solution") << s << e;
    }

    // === 6.11 ART-002 (исключение): last + time_unit ===============
    // Вход: last week. Ожидается: NO ERRORS (временной модификатор).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("last week"));
        RawToken last = makeRawToken(1, 1, "last", "ADJ", 2, "amod");
        last.lemma = QStringLiteral("last");
        addToken(s, last);
        RawToken week = makeRawToken(2, 2, "week", "NOUN", 0, "root");
        week.lemma = QStringLiteral("week");
        addToken(s, week);

        Art002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.11_last_week") << s << e;
    }

    // === 6.12 ART-002 (исключение): next + time_unit ===============
    // Вход: next year. Ожидается: NO ERRORS (временной модификатор).
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("next year"));
        RawToken next = makeRawToken(1, 1, "next", "ADJ", 2, "amod");
        next.lemma = QStringLiteral("next");
        addToken(s, next);
        RawToken year = makeRawToken(2, 2, "year", "NOUN", 0, "root");
        year.lemma = QStringLiteral("year");
        addToken(s, year);

        Art002Expect e;
        e.anchorTokenId = 1;
        e.expectedCount = 0;

        QTest::addRow("6.12_next_year") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_ART002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Art002Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(expect.anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr,
             qPrintable(QStringLiteral("[%1] anchor %2 не найден")
                        .arg(tag).arg(expect.anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();

    // ART-002a вызывается, если якорь — ADV. Иначе ART-002.
    QSet<CandidateError> result;
    if (anchor->upos == Upos::ADV) {
        Rule_ART002a rule;
        result = rule.check(*anchor, 0, DocumentModel(), runtime);
    } else {
        Rule_ART002 rule;
        result = rule.check(*anchor, 0, DocumentModel(), runtime);
    }

    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество кандидатов: ожидалось =" << expect.expectedCount
                     << "получено =" << actualCount;
            if (actualCount != 0) {
                const CandidateError& ce = *result.begin();
                qDebug() << "           неожиданное срабатывание:" << ce.ruleId
                         << "display:" << ce.displayTokenIds
                         << "conflict:" << ce.conflictTokenIds;
            }
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