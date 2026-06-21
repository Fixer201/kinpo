/*!
* \file TEST_ResolveCandidate.cpp
* \brief DDT-тесты для resolveCandidate (раздел 5 тестов_v3.md).
*
* Проверяет инкрементальное разрешение конфликтов приоритетов:
*  — пустая зона (сохранение);
*  — приоритет выше (подавление);
*  — приоритет ниже (отбрасывание);
*  — отсутствие отношения (совмещение);
*  — множественные токены в зоне;
*  — условный приоритет Art003LanguageCase (true/false);
*  — независимые зоны;
*  — дублирование кандидатов;
*  — многотокеновое подавление.
*/

#include <QtTest>
#include <QObject>
#include <QSet>
#include <QHash>

#include "TEST_ResolveCandidate.h"
#include "datamodel.h"
#include "modelbuilder.h"
#include "checkersystem.h"
#include "auxiliaryfunctionsfortesting.h"

namespace {

/*!
* \brief Создаёт CandidateError с заданными ruleId и conflictTokenIds.
*
* Поля sentId и displayTokenIds заполняются минимально,
* чтобы CandidateError был сравним через operator== и qHash.
*/
CandidateError makeCandidate(const QString& ruleId, const QSet<int>& conflictTokenIds)
{
    CandidateError ce;
    ce.ruleId = ruleId;
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = conflictTokenIds.values();
    ce.conflictTokenIds = conflictTokenIds;
    return ce;
}

/*!
* \brief Добавляет в зону ConflictZoneMap фиктивные кандидаты с заданными ruleId.
*
* Используется для инициализации зоны перед вызовом resolveCandidate.
* У всех фиктивных кандидатов одинаковые conflictTokenIds = zoneKey.
*/
void addInitialToZone(ConflictZoneMap& zm,
                      const QSet<int>& zoneKey,
                      const QSet<QString>& ruleIds)
{
    for (const QString& rid : ruleIds) {
        CandidateError ce = makeCandidate(rid, zoneKey);
        zm.zones[zoneKey].insert(ce);
    }
}

} // namespace

TEST_ResolveCandidate::TEST_ResolveCandidate() {}
TEST_ResolveCandidate::~TEST_ResolveCandidate() {}

void TEST_ResolveCandidate::TestResolveCandidate_data()
{
    QTest::addColumn<CandidateError>("candidate");   ///\u003c Новый кандидат
    QTest::addColumn<QSet<int>>("zone");               ///\u003c Зона для проверки
    QTest::addColumn<QSet<QString>>("initialRuleIds");///\u003c RuleId уже в зоне (или пусто)
    QTest::addColumn<PriorityIndex>("priorityIndex");///\u003c Индекс приоритетов
    QTest::addColumn<RawSentence>("rawSentence");     ///\u003c Предложение для SentenceModel
    QTest::addColumn<int>("repeatCount");            ///\u003c Сколько раз вызвать resolveCandidate
    QTest::addColumn<int>("expectedCount");          ///\u003c Ожидаемое число кандидатов в зоне
    QTest::addColumn<QSet<QString>>("expectedRuleIds");///\u003c Ожидаемые ruleId в зоне

    // ====================================================================
    // 5.1 — Пустая зона: простое сохранение
    // ====================================================================
    {
        QTest::addRow("5.1_empty_zone")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")};
    }

    // ====================================================================
    // 5.2 — Приоритет выше: DET-001 подавляет ART-006
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("DET-001")]
            [QStringLiteral("ART-006")] = PriorityConditionKind::Always;
        QTest::addRow("5.2_higher_priority")
            << makeCandidate(QStringLiteral("DET-001"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>{QStringLiteral("ART-006")}
            << pi
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("DET-001")};
    }

    // ====================================================================
    // 5.3 — Приоритет ниже: ART-006 отбрасывается DET-001
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("DET-001")]
            [QStringLiteral("ART-006")] = PriorityConditionKind::Always;
        QTest::addRow("5.3_lower_priority")
            << makeCandidate(QStringLiteral("ART-006"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>{QStringLiteral("DET-001")}
            << pi
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("DET-001")};
    }

    // ====================================================================
    // 5.4 — Нет отношения приоритета: оба остаются
    // ====================================================================
    {
        QTest::addRow("5.4_no_relation")
            << makeCandidate(QStringLiteral("PREP-001"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>{QStringLiteral("ART-001")}
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 2
            << QSet<QString>{QStringLiteral("ART-001"), QStringLiteral("PREP-001")};
    }

    // ====================================================================
    // 5.5 — Множественные conflictTokenIds
    // ====================================================================
    {
        QTest::addRow("5.5_multi_token_zone")
            << makeCandidate(QStringLiteral("CONJ-001"), QSet<int>{3, 4})
            << QSet<int>{3, 4}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("CONJ-001")};
    }

    // ====================================================================
    // 5.6 — Условный приоритет Art003LanguageCase (true)
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("ART-003")]
            [QStringLiteral("ART-001")] = PriorityConditionKind::Art003LanguageCase;

        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("English"));
        RawToken t = makeRawToken(1, 1, QStringLiteral("English"), QStringLiteral("PROPN"), 0, QStringLiteral("root"));
        t.lemma = QStringLiteral("English");
        addToken(s, t);

        QTest::addRow("5.6_conditional_true")
            << makeCandidate(QStringLiteral("ART-003"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>{QStringLiteral("ART-001")}
            << pi
            << s
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-003")};
    }

    // ====================================================================
    // 5.7 — Условный приоритет Art003LanguageCase (false)
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("ART-003")]
            [QStringLiteral("ART-001")] = PriorityConditionKind::Art003LanguageCase;

        RawSentence s = makeRawSentence(1, QStringLiteral("test"), QStringLiteral("English language"));
        RawToken t1 = makeRawToken(1, 1, QStringLiteral("English"), QStringLiteral("PROPN"), 0, QStringLiteral("root"));
        t1.lemma = QStringLiteral("English");
        addToken(s, t1);
        addToken(s, makeRawToken(2, 2, QStringLiteral("language"), QStringLiteral("NOUN"), 1, QStringLiteral("compound")));

        QTest::addRow("5.7_conditional_false")
            << makeCandidate(QStringLiteral("ART-003"), QSet<int>{1})
            << QSet<int>{1}
            << QSet<QString>{QStringLiteral("ART-001")}
            << pi
            << s
            << 1
            << 2
            << QSet<QString>{QStringLiteral("ART-001"), QStringLiteral("ART-003")};
    }

    // ====================================================================
    // 5.8 — Независимые зоны: новая зона {5} не трогает существующую {2}
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("DET-001")]
            [QStringLiteral("ART-006")] = PriorityConditionKind::Always;
        QTest::addRow("5.8_independent_zones")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{5})
            << QSet<int>{5}
            << QSet<QString>()
            << pi
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")};
    }

    // ====================================================================
    // 5.9 — Дубли: три одинаковых кандидата, в зоне остаётся один
    // ====================================================================
    {
        QTest::addRow("5.9_duplicates")
            << makeCandidate(QStringLiteral("CONJ-001"), QSet<int>{3})
            << QSet<int>{3}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 3
            << 1
            << QSet<QString>{QStringLiteral("CONJ-001")};
    }

    // ====================================================================
    // 5.10 — Множественные токены в ключе зоны, подавление
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("ART-001")]
            [QStringLiteral("ART-002")] = PriorityConditionKind::Always;
        QTest::addRow("5.10_multi_token_suppress")
            << makeCandidate(QStringLiteral("ART-002"), QSet<int>{2, 3})
            << QSet<int>{2, 3}
            << QSet<QString>{QStringLiteral("ART-001")}
            << pi
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")};
    }
}

void TEST_ResolveCandidate::TestResolveCandidate()
{
    // Получаем входные данные
    QFETCH(CandidateError, candidate);
    QFETCH(QSet<int>, zone);
    QFETCH(QSet<QString>, initialRuleIds);
    QFETCH(PriorityIndex, priorityIndex);
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, repeatCount);
    QFETCH(int, expectedCount);
    QFETCH(QSet<QString>, expectedRuleIds);

    const QString tag = QString(QTest::currentDataTag());

    // Строим SentenceModel и CheckerRuntime для вызова resolveCandidate
    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime;
    runtime.priorityIndex = priorityIndex;

    // Инициализируем ConflictZoneMap фиктивными кандидатами
    ConflictZoneMap zoneMap;
    if (!initialRuleIds.isEmpty()) {
        addInitialToZone(zoneMap, zone, initialRuleIds);
    }

    // Для 5.8 создаём дополнительную зону {2}→DET-001 до вызова resolveCandidate
    if (tag == QStringLiteral("5.8_independent_zones")) {
        QSet<int> otherZone{2};
        addInitialToZone(zoneMap, otherZone, QSet<QString>{QStringLiteral("DET-001")});
    }

    // Вызов resolveCandidate repeatCount раз
    for (int i = 0; i < repeatCount; ++i) {
        resolveCandidate(candidate, zoneMap, sentence, runtime);
    }

    // Проверяем целевую зону через хелпер с подробным логированием
    compareZoneCandidates(tag, zoneMap, zone, expectedCount, expectedRuleIds);

    // Для 5.8 дополнительно проверяем что зона {2} осталась нетронутой
    if (tag == QStringLiteral("5.8_independent_zones")) {
        QSet<int> otherZone{2};
        compareZoneCandidates(tag + QStringLiteral("_other"), zoneMap, otherZone, 1,
                              QSet<QString>{QStringLiteral("DET-001")});
    }
}
