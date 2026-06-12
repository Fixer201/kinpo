/*!
* \file TEST_CheckSentence.cpp
* \brief DDT тесты для checkSentence раздел 4 тестов_v3 md
*
* Проверяет диспетчеризацию правил по UPOS и разрешение конфликтов
* через PriorityIndex без полной реализации Rule системы
*/

#include <QtTest>
#include <QObject>
#include <QSet>
#include <QHash>

#include "TEST_CheckSentence.h"
#include "datamodel.h"
#include "inputmodule.h"
#include "auxiliaryfunctionsfortesting.h"

// ========================================================================
// Заглушки функций для тестов checkSentence
// ========================================================================

namespace {

// ART-001 — лишний артикль перед PROPN.
// Срабатывает на DET с формой a/an/the, когда родитель — PROPN.
class StubRule_ART001 : public Rule {
public:
    QString ruleId() const override { return QStringLiteral("ART-001"); }
    QSet<Upos> anchorUpos() const override { return {Upos::DET}; }
    bool canConflict() const override { return true; }

    QSet<CandidateError> check(const TokenNode& anchor, int /*sentenceIndex*/,
                               const DocumentModel& /*document*/,
                               const CheckerRuntime& /*runtime*/) const override
    {
        QSet<CandidateError> res;
        if (!anchor.parent) return res;
        if (anchor.upos != Upos::DET) return res;
        QStringList articles = {QStringLiteral("a"), QStringLiteral("an"), QStringLiteral("the")};
        if (!articles.contains(anchor.form.toLower())) return res;
        // Родитель должен быть PROPN для срабатывания ART-001.
        if (anchor.parent->upos == Upos::PROPN) {
            CandidateError ce;
            ce.ruleId = QStringLiteral("ART-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            res.insert(ce);
        }
        return res;
    }
};

// ART-006 — неверный a/an.
// Срабатывает на "an" перед согласной для проверки конфликтов с DET-001.
class StubRule_ART006 : public Rule {
public:
    QString ruleId() const override { return QStringLiteral("ART-006"); }
    QSet<Upos> anchorUpos() const override { return {Upos::DET}; }
    bool canConflict() const override { return true; }

    QSet<CandidateError> check(const TokenNode& anchor, int /*sentenceIndex*/,
                               const DocumentModel& /*document*/,
                               const CheckerRuntime& /*runtime*/) const override
    {
        QSet<CandidateError> res;
        if (anchor.upos != Upos::DET) return res;
        QString f = anchor.form.toLower();
        // Только "an" перед согласной — упрощённо для тестов.
        if (f == QStringLiteral("an") && anchor.nextToken) {
            QString next = anchor.nextToken->form.toLower();
            if (!next.isEmpty() && next[0].isLetter() && QStringLiteral("aeiou").indexOf(next[0]) < 0) {
                CandidateError ce;
                ce.ruleId = QStringLiteral("ART-006");
                ce.sentId = QStringLiteral("test");
                ce.displayTokenIds = {anchor.id};
                ce.conflictTokenIds = {anchor.id};
                res.insert(ce);
            }
        }
        return res;
    }
};

// DET-001 — несовместимость детерминатива с существительным.
// Проверяет "an" перед множественным числом и "this" перед неисчисляемым.
class StubRule_DET001 : public Rule {
public:
    QString ruleId() const override { return QStringLiteral("DET-001"); }
    QSet<Upos> anchorUpos() const override { return {Upos::DET}; }
    bool canConflict() const override { return true; }

    QSet<CandidateError> check(const TokenNode& anchor, int /*sentenceIndex*/,
                               const DocumentModel& /*document*/,
                               const CheckerRuntime& /*runtime*/) const override
    {
        QSet<CandidateError> res;
        if (anchor.upos != Upos::DET || !anchor.parent) return res;

        // "an" перед "books" — множественное число.
        if (anchor.form.toLower() == QStringLiteral("an") &&
            anchor.parent->upos == Upos::NOUN &&
            anchor.parent->form.toLower() == QStringLiteral("books"))
        {
            CandidateError ce;
            ce.ruleId = QStringLiteral("DET-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            res.insert(ce);
        }

        // "this" перед "informations" — неисчисляемое.
        if (anchor.form.toLower() == QStringLiteral("this") &&
            anchor.parent->upos == Upos::NOUN &&
            anchor.parent->form.toLower() == QStringLiteral("informations"))
        {
            CandidateError ce;
            ce.ruleId = QStringLiteral("DET-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            res.insert(ce);
        }

        return res;
    }
};

// PREP-001 — неверный временной предлог.
// Срабатывает на "at" перед "Monday".
class StubRule_PREP001 : public Rule {
public:
    QString ruleId() const override { return QStringLiteral("PREP-001"); }
    QSet<Upos> anchorUpos() const override { return {Upos::ADP}; }
    bool canConflict() const override { return true; }

    QSet<CandidateError> check(const TokenNode& anchor, int /*sentenceIndex*/,
                               const DocumentModel& /*document*/,
                               const CheckerRuntime& /*runtime*/) const override
    {
        QSet<CandidateError> res;
        if (anchor.upos != Upos::ADP) return res;

        // "at Monday" должно быть "on Monday".
        if (anchor.form.toLower() == QStringLiteral("at") &&
            anchor.parent && anchor.parent->form.toLower() == QStringLiteral("monday"))
        {
            CandidateError ce;
            ce.ruleId = QStringLiteral("PREP-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            res.insert(ce);
        }
        return res;
    }
};

// Сборка runtime с заглушками и приоритетом DET-001 выше ART-006.
CheckerRuntime makeTestRuntime()
{
    CheckerRuntime rt;
    static StubRule_ART001 art001;
    static StubRule_ART006 art006;
    static StubRule_DET001 det001;
    static StubRule_PREP001 prep001;

    rt.dispatch[Upos::DET].insert(&art001);
    rt.dispatch[Upos::DET].insert(&art006);
    rt.dispatch[Upos::DET].insert(&det001);
    rt.dispatch[Upos::ADP].insert(&prep001);

    rt.priorityIndex.conditionsByHigherRule[QStringLiteral("DET-001")]
        [QStringLiteral("ART-006")] = PriorityConditionKind::Always;

    return rt;
}

// Построение DocumentModel из одного RawSentence для тестов.
DocumentModel makeDocumentModel(const RawSentence& rawSentence)
{
    DocumentModel doc;
    SentenceModel sm = buildSentenceModel(rawSentence);
    doc.sentences.push_back(std::move(sm));
    for (const auto& s : std::as_const(doc.sentences)) {
        doc.sentById[s.sentId] = &s;
    }
    return doc;
}

} // namespace

// ========================================================================
// Тестовый класс
// ========================================================================

TEST_CheckSentence::TEST_CheckSentence() {}
TEST_CheckSentence::~TEST_CheckSentence() {}

void TEST_CheckSentence::TestCheckSentence_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QSet<QString>>("expectedRuleIds");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictZones");
    QTest::addColumn<QString>("expectedSuppressedRuleId");

    // 4.1 — корректное предложение без ошибок.
    // Проверяет, что на валидных данных правила не срабатывают.
    {
        RawSentence s = makeRawSentence(1, "test", "The cat sits.");
        addToken(s, makeRawToken(1, 1, "The",  "DET",  3, "det"));
        addToken(s, makeRawToken(2, 2, "cat",  "NOUN", 3, "nsubj"));
        addToken(s, makeRawToken(3, 3, "sits", "VERB", 0, "root"));
        addToken(s, makeRawToken(4, 4, ".",    "PUNCT",3, "punct"));
        QTest::addRow("4.1_no_errors")
            << s << 0 << QSet<QString>() << QList<QSet<int>>() << QString();
    }

    // 4.2 — одна ошибка ART-001.
    // Проверяет, что "a Europe" порождает ровно один кандидат ART-001.
    {
        RawSentence s = makeRawSentence(1, "test", "a Europe");
        addToken(s, makeRawToken(1, 1, "a",      "DET",  2, "det"));
        addToken(s, makeRawToken(2, 2, "Europe", "PROPN",0, "root"));
        QTest::addRow("4.2_art001")
            << s << 1
            << (QSet<QString>{QStringLiteral("ART-001")})
            << (QList<QSet<int>>{QSet<int>{1}})
            << QString();
    }

    // 4.3 — несколько независимых ошибок ART-001 + ART-006.
    // Проверяет, что разные зоны конфликтов не мешают друг другу.
    {
        RawSentence s = makeRawSentence(1, "test", "a Europe and an cat");
        addToken(s, makeRawToken(1, 1, "a",      "DET",   2, "det"));
        addToken(s, makeRawToken(2, 2, "Europe", "PROPN", 0, "root"));
        addToken(s, makeRawToken(3, 3, "and",    "CCONJ", 2, "cc"));
        addToken(s, makeRawToken(4, 4, "an",     "DET",   5, "det"));
        addToken(s, makeRawToken(5, 5, "cat",    "NOUN",  2, "conj"));
        QTest::addRow("4.3_independent")
            << s << 2
            << (QSet<QString>{QStringLiteral("ART-001"), QStringLiteral("ART-006")})
            << (QList<QSet<int>>{QSet<int>{1}, QSet<int>{4}})
            << QString();
    }

    // 4.4 — ошибки с конфликтом приоритетов: DET-001 выше ART-006.
    // Проверяет, что resolveCandidate подавляет ART-006, оставляя DET-001.
    {
        RawSentence s = makeRawSentence(1, "test", "an books");
        addToken(s, makeRawToken(1, 1, "an",    "DET",  2, "det"));
        addToken(s, makeRawToken(2, 2, "books", "NOUN", 0, "root"));
        QTest::addRow("4.4_priority")
            << s << 1
            << (QSet<QString>{QStringLiteral("DET-001")})
            << (QList<QSet<int>>{QSet<int>{1}})
            << QStringLiteral("ART-006");
    }

    // 4.5 — токен с UPOS без правил в dispatch: INTJ.
    // Проверяет, что INTJ не порождает вызовов check и не падает.
    {
        RawSentence s = makeRawSentence(1, "test", "Wow! The cat sits.");
        addToken(s, makeRawToken(1, 1, "Wow",  "INTJ",  5, "vocative"));
        addToken(s, makeRawToken(2, 2, "!",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(3, 3, "The",  "DET",   5, "det"));
        addToken(s, makeRawToken(4, 4, "cat",  "NOUN",  5, "nsubj"));
        addToken(s, makeRawToken(5, 5, "sits", "VERB",  0, "root"));
        addToken(s, makeRawToken(6, 6, ".",    "PUNCT", 5, "punct"));
        QTest::addRow("4.5_no_dispatch")
            << s << 0 << QSet<QString>() << QList<QSet<int>>() << QString();
    }

    // 4.6 — зависимые и независимые ошибки в одном предложении.
    // Проверяет одновременную работу разных зон и подавление ART-006 зоной DET-001.
    {
        RawSentence s = makeRawSentence(1, "test", "a Europe and an books and this informations and at Monday");
        addToken(s, makeRawToken(1,  1,  "a",            "DET",   2,  "det"));
        addToken(s, makeRawToken(2,  2,  "Europe",       "PROPN", 0,  "root"));
        addToken(s, makeRawToken(3,  3,  "and",          "CCONJ", 2,  "cc"));
        addToken(s, makeRawToken(4,  4,  "an",           "DET",   5,  "det"));
        addToken(s, makeRawToken(5,  5,  "books",        "NOUN",  2,  "conj"));
        addToken(s, makeRawToken(6,  6,  "and",          "CCONJ", 5,  "cc"));
        addToken(s, makeRawToken(7,  7,  "this",         "DET",   8,  "det"));
        addToken(s, makeRawToken(8,  8,  "informations", "NOUN",  5,  "conj"));
        addToken(s, makeRawToken(9,  9,  "and",          "CCONJ", 8,  "cc"));
        addToken(s, makeRawToken(10, 10, "at",           "ADP",   11, "case"));
        addToken(s, makeRawToken(11, 11, "Monday",       "PROPN", 5,  "conj"));
        QTest::addRow("4.6_mixed")
            << s << 4
            << (QSet<QString>{QStringLiteral("ART-001"),
                              QStringLiteral("DET-001"),
                              QStringLiteral("PREP-001")})
            << (QList<QSet<int>>{
                QSet<int>{1},   // ART-001
                QSet<int>{4},   // DET-001 (an books)
                QSet<int>{7},   // DET-001 (this informations)
                QSet<int>{10}  // PREP-001
            })
            << QStringLiteral("ART-006");
    }
}

void TEST_CheckSentence::TestCheckSentence()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedCount);
    QFETCH(QSet<QString>, expectedRuleIds);
    QFETCH(QList<QSet<int>>, expectedConflictZones);
    QFETCH(QString, expectedSuppressedRuleId);

    const QString tag = QString(QTest::currentDataTag());

    // строим модель документа и runtime для теста
    DocumentModel document = makeDocumentModel(rawSentence);
    CheckerRuntime runtime = makeTestRuntime();

    const SentenceModel& sentence = document.sentences[0];

    // вызываем тестируемую функцию
    QSet<CandidateError> result = checkSentence(sentence, 0, document, runtime);

    // проверяем общее число кандидатов после разрешения конфликтов
    QCOMPARE(result.size(), expectedCount);

    // собираем фактические ruleId и сравниваем с ожидаемыми
    QSet<QString> actualRuleIds;
    for (const auto& ce : result) {
        actualRuleIds.insert(ce.ruleId);
    }
    QCOMPARE(actualRuleIds, expectedRuleIds);

    // проверяем что каждая ожидаемая зона конфликтов присутствует в результате
    for (const QSet<int>& expectedZone : expectedConflictZones) {
        bool found = false;
        for (const auto& ce : result) {
            if (ce.conflictTokenIds == expectedZone) {
                found = true;
                break;
            }
        }
        QVERIFY2(found, qPrintable(QString("[%1] Не найден кандидат с conflictTokenIds=%2")
                                   .arg(tag)
                                   .arg([](const QSet<int>& s){
                                       QStringList parts;
                                       for (int v : s) parts.append(QString::number(v));
                                       return parts.isEmpty() ? QStringLiteral("∅") : parts.join(',');
                                   }(expectedZone))));
    }

    // проверяем что подавленное правило не присутствует в результате
    if (!expectedSuppressedRuleId.isEmpty()) {
        for (const auto& ce : result) {
            QVERIFY2(ce.ruleId != expectedSuppressedRuleId,
                     qPrintable(QString("[%1] Подавленное правило %2 присутствует в результате")
                                .arg(tag, expectedSuppressedRuleId)));
        }
    }
}
