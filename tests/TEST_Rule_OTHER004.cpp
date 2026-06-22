/*!
* \file TEST_Rule_OTHER004.cpp
* \brief Тесты правила OTHER-004: неверный падеж местоимения.
*
* Раздел 6.93–6.95 утверждённой версии тестов (тесты_v3.docx).
* Якорём служит местоимение (PRON). Проверяются:
*  6.93 — объектная форма "Me" в позиции подлежащего, ожидается "I";
*  6.94 — именительная форма "he" в позиции дополнения, ожидается "him";
*  6.95 — "It is I": именительный после copula допустим, ошибки нет
*         (deprel=comp не входит в список nsubj/obj/obl, поэтому правило
*         естественно не срабатывает).
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

namespace {

/*!
* \brief Создать runtime с загруженными словарями.
* \return CheckerRuntime с заполненными resources.
*
* OTHER-004 не использует словари, но загрузка сохраняет совместимость
* с другими тестами правил.
*/
CheckerRuntime makeRuntimeWithResources()
{
    CheckerRuntime runtime;
    QString listsDir = findListsDir();
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qDebug() << "[TEST_Rule_OTHER004]" << w;
    runtime.resources = std::move(res);
    return runtime;
}

} // namespace

TEST_Rule_OTHER004::TEST_Rule_OTHER004() {}
TEST_Rule_OTHER004::~TEST_Rule_OTHER004() {}

void TEST_Rule_OTHER004::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("anchorTokenId");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("expectedRuleId");
    QTest::addColumn<QList<QList<int>>>("expectedDisplayIdsList");
    QTest::addColumn<QList<QSet<int>>>("expectedConflictIdsList");

    // 6.93: Me saw. Объектная форма в позиции подлежащего, Me -> I.
    // Me/PRON[nsubj->saw], saw/VERB[HEAD=0]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.93"), QStringLiteral("Me saw"));
        RawToken me = makeRawToken(1, 1, "Me", "PRON", 2, "nsubj",
                                   QStringLiteral("Case=Acc|Number=Sing|Person=1|PronType=Prs"));
        me.lemma = QStringLiteral("I");
        addToken(s, me);
        RawToken saw = makeRawToken(2, 2, "saw", "VERB", 0, "root",
                                    QStringLiteral("Mood=Ind|Number=Sing|Person=3|Tense=Past|VerbForm=Fin"));
        saw.lemma = QStringLiteral("see");
        addToken(s, saw);
        QTest::addRow("6.93_me_saw")
            << s << 1
            << 1
            << QStringLiteral("OTHER-004")
            << (QList<QList<int>>{QList<int>{1}})
            << (QList<QSet<int>>{QSet<int>{1}});
    }

    // 6.94: saw he. Именительная форма в позиции дополнения, he -> him.
    // saw/VERB[HEAD=0], he/PRON[obj->saw]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.94"), QStringLiteral("saw he"));
        RawToken saw = makeRawToken(1, 1, "saw", "VERB", 0, "root",
                                    QStringLiteral("Mood=Ind|Number=Sing|Person=3|Tense=Past|VerbForm=Fin"));
        saw.lemma = QStringLiteral("see");
        addToken(s, saw);
        RawToken he = makeRawToken(2, 2, "he", "PRON", 1, "obj",
                                   QStringLiteral("Case=Nom|Gender=Masc|Number=Sing|Person=3|PronType=Prs"));
        he.lemma = QStringLiteral("he");
        addToken(s, he);
        QTest::addRow("6.94_saw_he")
            << s << 2
            << 1
            << QStringLiteral("OTHER-004")
            << (QList<QList<int>>{QList<int>{2}})
            << (QList<QSet<int>>{QSet<int>{2}});
    }

    // 6.95: It is I. Именительный после copula допустим, ошибки нет.
    // It/PRON[nsubj->is], is/VERB[HEAD=0], I/PRON[comp->is]
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("6.95"), QStringLiteral("It is I"));
        RawToken it = makeRawToken(1, 1, "It", "PRON", 2, "nsubj",
                                   QStringLiteral("Case=Nom|Number=Sing|Person=3|PronType=Prs"));
        it.lemma = QStringLiteral("it");
        addToken(s, it);
        RawToken is = makeRawToken(2, 2, "is", "VERB", 0, "root",
                                   QStringLiteral("Mood=Ind|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin"));
        is.lemma = QStringLiteral("be");
        addToken(s, is);
        RawToken I = makeRawToken(3, 3, "I", "PRON", 2, "comp",
                                  QStringLiteral("Case=Nom|Number=Sing|Person=1|PronType=Prs"));
        I.lemma = QStringLiteral("I");
        addToken(s, I);
        QTest::addRow("6.95_it_is_I")
            << s << 3
            << 0
            << QString()
            << QList<QList<int>>()
            << QList<QSet<int>>();
    }
}

void TEST_Rule_OTHER004::TestRule()
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
    Rule_OTHER004 rule;

    QSet<CandidateError> result = rule.check(*anchor, 0, DocumentModel(), runtime);

    QCOMPARE(result.size(), expectedCount);

    if (expectedCount == 0)
        return;

    compareMultiCandidate(tag, result, expectedRuleId,
                           expectedDisplayIdsList, expectedConflictIdsList);
}