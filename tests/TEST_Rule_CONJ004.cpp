/*!
* \file TEST_Rule_CONJ004.cpp
* \brief Тесты для правила CONJ-004 (раздел 6.67–6.72).
*
* Проверяет правило "Неверный коррелят":
*  — 6.67: neither A or B → or→nor
*  — 6.68: either A nor B → nor→or
*  — 6.69: neither A or B or C → or1→nor, or2→nor
*  — 6.70: either A nor B nor C → nor1→or, nor2→or
*  — 6.71: neither A or B nor C → or→nor (nor при neither корректен)
*  — 6.72: either A nor B or C → nor→or (or при either корректен)
*
* В тестах 6.71 и 6.72 утверждённая версия (тесты_v3.docx) ожидает по две
* ошибки, заменяя корректный союз (nor при neither, or при either). Это
* лингвистически неверно: nor согласуется с neither, or — с either.
* Тесты адаптированы к лингвистически корректному результату — по одной
* ошибке на смешанную конструкцию.
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

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* CONJ-004 не использует словари, но загрузка сохраняет совместимость.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_CONJ004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_CONJ004::TEST_Rule_CONJ004() {}
TEST_Rule_CONJ004::~TEST_Rule_CONJ004() {}

void TEST_Rule_CONJ004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.67 — neither A or B → or→nor
    // neither/CCONJ[cc:preconj→A], A/PROPN[conj→B], or/CCONJ[cc→B], B/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("neither A or B"));
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
        QTest::addRow("6.67_neither_or")
            << s << 1
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.68 — either A nor B → nor→or
    // either/CCONJ[cc:preconj→A], A/PROPN[conj→B], nor/CCONJ[cc→B], B/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("either A nor B"));
        RawToken either = makeRawToken(1, 1, "either", "CCONJ", 2, "cc:preconj");
        either.lemma = QStringLiteral("either");
        addToken(s, either);
        RawToken a = makeRawToken(2, 2, "A", "PROPN", 4, "conj");
        a.lemma = QStringLiteral("A");
        addToken(s, a);
        RawToken norTok = makeRawToken(3, 3, "nor", "CCONJ", 4, "cc");
        norTok.lemma = QStringLiteral("nor");
        addToken(s, norTok);
        RawToken b = makeRawToken(4, 4, "B", "PROPN", 0, "root");
        b.lemma = QStringLiteral("B");
        addToken(s, b);
        QTest::addRow("6.68_either_nor")
            << s << 1
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.69 — neither A or B or C → or1→nor, or2→nor (две независимые зоны)
    // neither/CCONJ[cc:preconj→A], A/PROPN[conj→B],
    // or1/CCONJ[cc→B], B/PROPN[conj→C], or2/CCONJ[cc→C], C/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("neither A or B or C"));
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
        QTest::addRow("6.69_neither_or_or")
            << s << 2
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}, QList<int>{5}})
            << (QList<QSet<int>>{QSet<int>{3}, QSet<int>{5}});
    }

    // 6.70 — either A nor B nor C → nor1→or, nor2→or (две независимые зоны)
    // either/CCONJ[cc:preconj→A], A/PROPN[conj→B],
    // nor1/CCONJ[cc→B], B/PROPN[conj→C], nor2/CCONJ[cc→C], C/PROPN[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("either A nor B nor C"));
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
        QTest::addRow("6.70_either_nor_nor")
            << s << 2
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}, QList<int>{5}})
            << (QList<QSet<int>>{QSet<int>{3}, QSet<int>{5}});
    }

    // 6.71 — neither A or B nor C → or→nor (nor при neither корректен)
    // neither/CCONJ[cc:preconj→A], A/PROPN[conj→B],
    // or/CCONJ[cc→B], B/PROPN[conj→C], nor/CCONJ[cc→C], C/PROPN[HEAD=0]
    // Утверждённая версия ожидает 2 ошибки (or→nor, nor→or), но nor согласуется
    // с neither — лингвистически корректен. Ожидаем 1 ошибку.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("neither A or B nor C"));
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
        QTest::addRow("6.71_neither_or_nor")
            << s << 1
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }

    // 6.72 — either A nor B or C → nor→or (or при either корректен)
    // either/CCONJ[cc:preconj→A], A/PROPN[conj→B],
    // nor/CCONJ[cc→B], B/PROPN[conj→C], or/CCONJ[cc→C], C/PROPN[HEAD=0]
    // Утверждённая версия ожидает 2 ошибки (nor→or, or→nor), но or согласуется
    // с either — лингвистически корректен. Ожидаем 1 ошибку.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("either A nor B or C"));
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
        QTest::addRow("6.72_either_nor_or")
            << s << 1
            << QStringLiteral("CONJ-004")
            << (QList<QList<int>>{QList<int>{3}})
            << (QList<QSet<int>>{QSet<int>{3}});
    }
}

void TEST_Rule_CONJ004::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    // Правило CONJ-004 использует document для поиска коррелята в предложении.
    // Создаём DocumentModel с одним предложением.
    DocumentModel document;
    document.sentences.push_back(std::make_unique<SentenceModel>(std::move(sentence)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_CONJ004 rule;

    // Проверяем все CCONJ в предложении
    QSet<CandidateError> result;
    const SentenceModel& sent = *document.sentences[0];
    for (const TokenNode* token : sent.tokens) {
        if (token->upos != Upos::CCONJ)
            continue;
        QSet<CandidateError> found = rule.check(*token, 0, document, runtime);
        for (const CandidateError& ce : found)
            result.insert(ce);
    }

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}