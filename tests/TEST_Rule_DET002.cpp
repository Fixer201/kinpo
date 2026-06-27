/*!
* \file TEST_Rule_DET002.cpp
* \brief DDT-тесты для правила DET-002 (тесты 6.34–6.36).
*
* Проверяет правило «Два центральных детерминатива»:
*  — артикль + притяжательное (the my friend → the);
*  — притяжательное + артикль (my the friend → the);
*  — исключение: предетерминатив (all the people — без ошибок).
*
* Обход всех NOUN: правило проверяет совместимость детерминативов
* при одном NOUN. Для multi-candidate используется expectedConflictZones.
*/

#include <QtTest>
#include <QObject>
#include <QSet>

#include "TEST_Rule_DET002.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "wordlists.h"
#include "auxiliaryfunctionsfortesting.h"
#include "rule_det002.h"

/*!
* \struct Det002Expect
* \brief Ожидания для одного теста правила DET-002.
*
* Заполняются только те поля, которые проверяет конкретный тест.
* Значения по умолчанию (-1, пустые списки) означают «не проверять».
*/
struct Det002Expect {
    int expectedCount = -1;
    QString expectedRuleId;
    QList<QSet<int>> expectedConflictZones;
};
Q_DECLARE_METATYPE(Det002Expect)

TEST_Rule_DET002::TEST_Rule_DET002() {}
TEST_Rule_DET002::~TEST_Rule_DET002() {}

void TEST_Rule_DET002::TestRule_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<Det002Expect>("expect");

    // 6.34 — the my friend → the (артикль + притяжательное)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("the my friend"));
        RawToken the = makeRawToken(1, 1, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken my = makeRawToken(2, 2, "my", "DET", 3, "det");
        my.lemma = QStringLiteral("my");
        addToken(s, my);
        RawToken friendTok = makeRawToken(3, 3, "friend", "NOUN", 0, "root");
        friendTok.lemma = QStringLiteral("friend");
        addToken(s, friendTok);
        Det002Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-002");
        e.expectedConflictZones = {QSet<int>{1}};
        QTest::addRow("6.34_the_my_friend") << s << e;
    }
    // 6.35 — my the friend → the (притяжательное + артикль)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("my the friend"));
        RawToken my = makeRawToken(1, 1, "my", "DET", 3, "det");
        my.lemma = QStringLiteral("my");
        addToken(s, my);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken friendTok = makeRawToken(3, 3, "friend", "NOUN", 0, "root");
        friendTok.lemma = QStringLiteral("friend");
        addToken(s, friendTok);
        Det002Expect e;
        e.expectedCount = 1;
        e.expectedRuleId = QStringLiteral("DET-002");
        e.expectedConflictZones = {QSet<int>{2}};
        QTest::addRow("6.35_my_the_friend") << s << e;
    }
    // 6.36 — all the people → без ошибок (all — предетерминатив)
    {
        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("all the people"));
        RawToken all = makeRawToken(1, 1, "all", "DET", 3, "det");
        all.lemma = QStringLiteral("all");
        addToken(s, all);
        RawToken the = makeRawToken(2, 2, "the", "DET", 3, "det");
        the.lemma = QStringLiteral("the");
        addToken(s, the);
        RawToken people = makeRawToken(3, 3, "people", "NOUN", 0, "root");
        people.lemma = QStringLiteral("people");
        addToken(s, people);
        Det002Expect e;
        e.expectedCount = 0;
        QTest::addRow("6.36_all_the_people") << s << e;
    }
}

void TEST_Rule_DET002::TestRule()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(Det002Expect, expect);
    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime = makeRuntimeWithResources();
    Rule_DET002 rule;

    // Обходим все NOUN: правило проверяет детерминативы при одном NOUN
    QSet<CandidateError> result;
    for (TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::Noun)
            continue;
        QSet<CandidateError> found = rule.check(*token, 0, DocumentModel(), runtime);
        for (const CandidateError& ce : found)
            result.insert(ce);
    }

    if (expect.expectedCount != -1) {
        int actualCount = static_cast<int>(result.size());
        if (actualCount != expect.expectedCount) {
            qDebug() << "[TEST FAIL]" << tag << "- количество кандидатов не совпадает:"
                     << "ожидалось =" << expect.expectedCount << ", получено  =" << actualCount;
        }
        QCOMPARE(actualCount, expect.expectedCount);
    }
    if (expect.expectedCount == 0) return;

    if (!expect.expectedConflictZones.isEmpty()) {
        compareConflictZones(tag, result, expect.expectedConflictZones);
    }
    if (!expect.expectedRuleId.isEmpty()) {
        for (const CandidateError& ce : result) {
            if (ce.ruleId != expect.expectedRuleId) {
                qDebug() << "[TEST FAIL]" << tag << "- ruleId не совпадает:"
                         << "ожидался =" << expect.expectedRuleId << ", получено  =" << ce.ruleId;
            }
            QCOMPARE(ce.ruleId, expect.expectedRuleId);
        }
    }
}