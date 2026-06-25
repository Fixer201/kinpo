/*!
* \file TEST_Rule_CONJ004.cpp
* \brief DDT-тесты для правила CONJ-004 (раздел 6.67-6.72 тесты_v3.md).
*
* Проверяет правило «Неверный коррелят neither/either»:
*  — 6.67: neither A or B → or→nor;
*  — 6.68: either A nor B → nor→or;
*  — 6.69: neither A or B or C → 2 кандидата (or→nor, or→nor);
*  — 6.70: either A nor B nor C → 2 кандидата (nor→or, nor→or);
*  — 6.71: neither A or B nor C → 2 кандидата (or→nor, nor→or);
*  — 6.72: either A nor B or C → 2 кандидата (nor→or, or→nor).
*
* Обход всех CCONJ с использованием DocumentModel для навигации
* по коррелятам neither/either. Для multi-candidate используется
* expectedConflictZones.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_CONJ004.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_conj004.h"

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct Conj004Expect
* \brief Точечные ожидания для тестов правила CONJ-004.
*
* Структура не содержит anchorTokenId, так как правило использует
* паттерн обхода всех CCONJ-токенов в предложении.
*/
struct Conj004Expect {
    int expectedCount = -1;            ///< Ожидаемое число кандидатов. -1: не проверять.
    QString expectedRuleId;            ///< Ожидаемый ruleId. Пусто: не проверять.
    QList<int> expectedDisplayIds;      ///< Ожидаемые displayTokenIds (для single).
    QSet<int> expectedConflictIds;       ///< Ожидаемые conflictTokenIds (для single).
    QList<QSet<int>> expectedConflictZones; ///< Ожидаемые зоны конфликтов (для multi).
};

Q_DECLARE_METATYPE(Conj004Expect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_Rule_CONJ004::TEST_Rule_CONJ004() {}
TEST_Rule_CONJ004::~TEST_Rule_CONJ004() {}

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
        qDebug() << "[TEST_Rule_CONJ004]" << w;
    }
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

// ------------------------------------------------------------------------
// Данные тестов (6.67-6.72)
// ------------------------------------------------------------------------

void TEST_Rule_CONJ004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Conj004Expect>("expect");

    // === 6.67 CONJ-004: neither A or B → 1 (or→nor) ================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("neither A or B"));
        RawToken neither = makeRawToken(1, 1, "neither", "CCONJ", 2, "cc:preconj");
        neither.lemma = QStringLiteral("neither");
        addToken(s, neither);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken orTok = makeRawToken(3, 3, "or", "CCONJ", 4, "cc");
        orTok.lemma = QStringLiteral("or");
        addToken(s, orTok);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);

        Conj004Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.67_neither_or") << s << e;
    }

    // === 6.68 CONJ-004: either A nor B → 1 (nor→or) ================
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("either A nor B"));
        RawToken either = makeRawToken(1, 1, "either", "CCONJ", 2, "cc:preconj");
        either.lemma = QStringLiteral("either");
        addToken(s, either);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken nor = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        nor.lemma = QStringLiteral("nor");
        addToken(s, nor);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);

        Conj004Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedDisplayIds = {3};
        e.expectedConflictIds = {3};

        QTest::addRow("6.68_either_nor") << s << e;
    }

    // === 6.69 CONJ-004: neither A or B or C → 2 (or→nor, or→nor) ===
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("neither A or B or C"));
        RawToken neither = makeRawToken(1, 1, "neither", "CCONJ", 2, "cc:preconj");
        neither.lemma = QStringLiteral("neither");
        addToken(s, neither);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken or1 = makeRawToken(3, 3, "or", "CCONJ", 4, "cc");
        or1.lemma = QStringLiteral("or");
        addToken(s, or1);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 6, "conj");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        RawToken or2 = makeRawToken(5, 5, "or", "CCONJ", 6, "cc");
        or2.lemma = QStringLiteral("or");
        addToken(s, or2);
        RawToken c = makeRawToken(6, 6, "C", "PROPN", 0, "root");
        c.lemma = QStringLiteral("C");
        addToken(s, c);

        Conj004Expect e;
        e.expectedCount = 2;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedConflictZones = {QSet<int>{3}, QSet<int>{5}};

        QTest::addRow("6.69_neither_or_or") << s << e;
    }

    // === 6.70 CONJ-004: either A nor B nor C → 2 (nor→or, nor→or) ==
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("either A nor B nor C"));
        RawToken either = makeRawToken(1, 1, "either", "CCONJ", 2, "cc:preconj");
        either.lemma = QStringLiteral("either");
        addToken(s, either);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken nor1 = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        nor1.lemma = QStringLiteral("nor");
        addToken(s, nor1);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 6, "conj");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        RawToken nor2 = makeRawToken(5, 5, "nor", "CCONJ", 6, "cc");
        nor2.lemma = QStringLiteral("nor");
        addToken(s, nor2);
        RawToken c = makeRawToken(6, 6, "C", "PROPN", 0, "root");
        c.lemma = QStringLiteral("C");
        addToken(s, c);

        Conj004Expect e;
        e.expectedCount = 2;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedConflictZones = {QSet<int>{3}, QSet<int>{5}};

        QTest::addRow("6.70_either_nor_nor") << s << e;
    }

    // === 6.71 CONJ-004: neither A or B nor C → 2 (or→nor, nor→or) =
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("neither A or B nor C"));
        RawToken neither = makeRawToken(1, 1, "neither", "CCONJ", 2, "cc:preconj");
        neither.lemma = QStringLiteral("neither");
        addToken(s, neither);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken orTok = makeRawToken(3, 3, "or", "CCONJ", 4, "cc");
        orTok.lemma = QStringLiteral("or");
        addToken(s, orTok);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 6, "conj");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        RawToken norTok = makeRawToken(5, 5, "nor", "CCONJ", 6, "cc");
        norTok.lemma = QStringLiteral("nor");
        addToken(s, norTok);
        RawToken c = makeRawToken(6, 6, "C", "PROPN", 0, "root");
        c.lemma = QStringLiteral("C");
        addToken(s, c);

        Conj004Expect e;
        e.expectedCount = 2;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedConflictZones = {QSet<int>{3}, QSet<int>{5}};

        QTest::addRow("6.71_neither_or_nor") << s << e;
    }

    // === 6.72 CONJ-004: either A nor B or C → 2 (nor→or, or→nor) ==
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"),
                                        QStringLiteral("either A nor B or C"));
        RawToken either = makeRawToken(1, 1, "either", "CCONJ", 2, "cc:preconj");
        either.lemma = QStringLiteral("either");
        addToken(s, either);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken norTok = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        norTok.lemma = QStringLiteral("nor");
        addToken(s, norTok);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 6, "conj");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        RawToken orTok = makeRawToken(5, 5, "or", "CCONJ", 6, "cc");
        orTok.lemma = QStringLiteral("or");
        addToken(s, orTok);
        RawToken c = makeRawToken(6, 6, "C", "PROPN", 0, "root");
        c.lemma = QStringLiteral("C");
        addToken(s, c);

        Conj004Expect e;
        e.expectedCount = 2;
        e.expectedRuleId = QStringLiteral("CONJ-004");
        e.expectedConflictZones = {QSet<int>{3}, QSet<int>{5}};

        QTest::addRow("6.72_either_nor_or") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_Rule_CONJ004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Conj004Expect, expect);

    const QString tag = QString(QTest::currentDataTag());

    // CONJ-004 требует DocumentModel для навигации по коррелятам.
    SentenceModel sentence = buildSentenceModel(rawSentence);
    DocumentModel document;
    document.sentences.push_back(std::make_unique<SentenceModel>(std::move(sentence)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_CONJ004 rule;

    // Обходим все CCONJ: правило проверяет пары коррелятов.
    QSet<CandidateError> result;
    for (TokenNode* token : document.sentences[0]->tokens) {
        if (token->upos != Upos::CCONJ) {
            continue;
        }
        QSet<CandidateError> found = rule.check(*token, 0, document, runtime);
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
            for (const auto& ce : result) {
                qDebug() << "  " << ce.ruleId << ce.displayTokenIds << ce.conflictTokenIds;
            }
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }

    if (expect.expectedCount == 0) {
        return;
    }

    // Для multi-candidate проверяем зоны конфликтов.
    if (!expect.expectedConflictZones.isEmpty()) {
        for (const QSet<int>& z : expect.expectedConflictZones) {
            bool found = false;
            for (const auto& ce : result) {
                if (ce.conflictTokenIds == z) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                qDebug() << "[TEST FAIL]" << tag << "zone not found" << z;
            }
            QVERIFY2(found, qPrintable(QStringLiteral("%1: zone not found").arg(tag)));
        }
        if (!expect.expectedRuleId.isEmpty()) {
            QSet<QString> ids;
            for (const auto& ce : result) {
                ids.insert(ce.ruleId);
            }
            QCOMPARE(ids, QSet<QString>{expect.expectedRuleId});
        }
    } else if (!result.isEmpty()) {
        // Для single-candidate — обычная проверка.
        compareSingleCandidate(tag, *result.begin(),
                               expect.expectedRuleId,
                               expect.expectedDisplayIds,
                               expect.expectedConflictIds);
    }
}