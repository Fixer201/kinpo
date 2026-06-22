/*!
* \file TEST_Rule_PREP005.cpp
* \brief Тесты для правила PREP-005 (раздел 6.56–6.59).
*
* Проверяет правило "Неверный предлог при глаголе":
*  — 6.56: discuss about the plan → PREP-005 (лишний about, удалить)
*  — 6.57: explain me → PREP-005 (пропущен to, вставить)
*  — 6.58: depend of it → PREP-005 (of неверный, заменить на on)
*  — 6.59: explain about it → NO ERRORS (about допустим, записи нет в словаре)
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_PREP005.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_prep005.h"

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* Словарь verb_prep.txt нужен для определения требуемого предлога и действия.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_PREP005]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_PREP005::TEST_Rule_PREP005() {}
TEST_Rule_PREP005::~TEST_Rule_PREP005() {}

void TEST_Rule_PREP005::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.56 — discuss about the plan → PREP-005 (лишний about, удалить)
    // discuss/VERB[HEAD=0], about/ADP[case→plan], the/DET[det→plan],
    // plan/NOUN[obl→discuss]
    // Запись словаря: discuss about - → DeletePrep, wrongPrep=about
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("discuss about the plan"));
        RawToken discuss = makeRawToken(1, 1, "discuss", "VERB", 0, "root");
        discuss.lemma = QStringLiteral("discuss");
        addToken(s, discuss);
        RawToken about = makeRawToken(2, 2, "about", "ADP", 4, "case");
        about.lemma = QStringLiteral("about");
        addToken(s, about);
        RawToken the = makeRawToken(3, 3, "the", "DET", 4, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken plan = makeRawToken(4, 4, "plan", "NOUN", 1, "obl");
        plan.lemma = QStringLiteral("plan");
        addToken(s, plan);
        QTest::addRow("6.56_discuss_about_plan")
            << s << 1
            << 1
            << QStringLiteral("PREP-005")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.57 — explain me → PREP-005 (пропущен to, вставить)
    // explain/VERB[HEAD=0], me/PRON[obj→explain]
    // Запись словаря: explain - to → InsertPrep, prep=to
    // У V есть obj=me без case-зависимого → вставить to
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("explain me"));
        RawToken explain = makeRawToken(1, 1, "explain", "VERB", 0, "root");
        explain.lemma = QStringLiteral("explain");
        addToken(s, explain);
        RawToken me = makeRawToken(2, 2, "me", "PRON", 1, "obj");
        me.lemma = QStringLiteral("I");
        addToken(s, me);
        QTest::addRow("6.57_explain_me")
            << s << 1
            << 1
            << QStringLiteral("PREP-005")
            << (QList<QList<int>>{QList<int>{1, 2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.58 — depend of it → PREP-005 (of неверный, заменить на on)
    // depend/VERB[HEAD=0], of/ADP[case→it], it/PRON[obl→depend]
    // Запись словаря: depend of on → ReplacePrep, wrongPrep=of, prep=on
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("depend of it"));
        RawToken depend = makeRawToken(1, 1, "depend", "VERB", 0, "root");
        depend.lemma = QStringLiteral("depend");
        addToken(s, depend);
        RawToken of = makeRawToken(2, 2, "of", "ADP", 3, "case");
        of.lemma = QStringLiteral("of");
        addToken(s, of);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 1, "obl");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        QTest::addRow("6.58_depend_of_it")
            << s << 1
            << 1
            << QStringLiteral("PREP-005")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.59 — explain about it → NO ERRORS (about допустим с explain)
    // explain/VERB[HEAD=0], about/ADP[case→it], it/PRON[obl→explain]
    // В словаре для explain только запись "explain - to" (InsertPrep).
    // У V есть obl=it с case=about, obj нет → InsertPrep не срабатывает.
    // Записи "explain about -" нет → about не ошибочный предлог.
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("explain about it"));
        RawToken explain = makeRawToken(1, 1, "explain", "VERB", 0, "root");
        explain.lemma = QStringLiteral("explain");
        addToken(s, explain);
        RawToken about = makeRawToken(2, 2, "about", "ADP", 3, "case");
        about.lemma = QStringLiteral("about");
        addToken(s, about);
        RawToken it = makeRawToken(3, 3, "it", "PRON", 1, "obl");
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        QTest::addRow("6.59_explain_about_it")
            << s << 1
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_PREP005::TestRule()
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
    Rule_PREP005 rule;

    // Якорь — глагол VERB, проверяем его напрямую
    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    // Если кандидатов нет, правило не сработало, проверка завершена
    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}