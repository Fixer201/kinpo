/*!
* \file TEST_Rule_DET001.cpp
* \brief тесты для правила DET-001 (раздел 6.27–6.33).
*
* Проверяет правило "Несовместимость det/quant + NOUN":
*  — much + мн.ч. исчисляемое → many
*  — many + ед.ч. неисчисляемое → much
*  — a + мн.ч. → -
*  — this + мн.ч. → these
*  — few + неисчисляемое → little
*  — little + мн.ч. исчисляемое → few
*  — a + ед.ч. неисчисляемое → -
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET001.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det001.h"

namespace {

CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_DET001]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_DET001::TEST_Rule_DET001() {}
TEST_Rule_DET001::~TEST_Rule_DET001() {}

void TEST_Rule_DET001::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.27 — much books → many (much несовместим с Plur+исчисляемое)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("much books"));
        RawToken much = makeRawToken(1, 1, "much", "DET", 2, "det");
        much.lemma = QStringLiteral("much");
        addToken(s, much);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                       QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);
        QTest::addRow("6.27_much_books")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.28 — many water → much (many несовместим с неисчисляемым)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("many water"));
        RawToken many = makeRawToken(1, 1, "many", "DET", 2, "det");
        many.lemma = QStringLiteral("many");
        addToken(s, many);
        RawToken water = makeRawToken(2, 2, "water", "NOUN", 0, "root",
                                       QStringLiteral("Number=Sing"));
        water.lemma = QStringLiteral("water");
        addToken(s, water);
        QTest::addRow("6.28_many_water")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.29 — a books → - (a несовместим с Plur)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a books"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                       QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);
        QTest::addRow("6.29_a_books")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.30 — this books → these (this несовместим с Plur)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("this books"));
        RawToken thisTok = makeRawToken(1, 1, "this", "DET", 2, "det");
        thisTok.lemma = QStringLiteral("this");
        addToken(s, thisTok);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                       QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);
        QTest::addRow("6.30_this_books")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.31 — few water → little (few несовместим с неисчисляемым)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("few water"));
        RawToken few = makeRawToken(1, 1, "few", "DET", 2, "det");
        few.lemma = QStringLiteral("few");
        addToken(s, few);
        RawToken water = makeRawToken(2, 2, "water", "NOUN", 0, "root",
                                       QStringLiteral("Number=Sing"));
        water.lemma = QStringLiteral("water");
        addToken(s, water);
        QTest::addRow("6.31_few_water")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.32 — little books → few (little несовместим с Plur+исчисляемое)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("little books"));
        RawToken little = makeRawToken(1, 1, "little", "DET", 2, "det");
        little.lemma = QStringLiteral("little");
        addToken(s, little);
        RawToken books = makeRawToken(2, 2, "books", "NOUN", 0, "root",
                                       QStringLiteral("Number=Plur"));
        books.lemma = QStringLiteral("book");
        addToken(s, books);
        QTest::addRow("6.32_little_books")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.33 — a furniture → - (a несовместим с неисчисляемым)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("a furniture"));
        RawToken a = makeRawToken(1, 1, "a", "DET", 2, "det");
        a.lemma = QStringLiteral("a");
        addToken(s, a);
        RawToken furniture = makeRawToken(2, 2, "furniture", "NOUN", 0, "root",
                                           QStringLiteral("Number=Sing"));
        furniture.lemma = QStringLiteral("furniture");
        addToken(s, furniture);
        QTest::addRow("6.33_a_furniture")
            << s << 1
            << 1
            << QStringLiteral("DET-001")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }
}

void TEST_Rule_DET001::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, anchorTokenId);
    QFETCH(int, expectedCount);
    QFETCH(QString, expectedRuleId);
    QFETCH(QList<QList<int>>, expectedDisplayIdsList);
    QFETCH(QList<QSet<int>>, expectedConflictIdsList);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);

    TokenNode* anchor = sentence.tokensById.value(anchorTokenId, nullptr);
    QVERIFY2(anchor != nullptr, qPrintable(QString("[%1] Якорный токен %2 не найден").arg(tag).arg(anchorTokenId)));

    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET001 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет — правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}