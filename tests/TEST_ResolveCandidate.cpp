/*!
* \file TEST_ResolveCandidate.cpp
* \brief DDT-тесты для resolveCandidate (раздел 5 тестов_v3.md).
*
* Проверяет инкрементальное разрешение конфликтов приоритетов:
*  — пустая зона;
*  — приоритет выше/ниже;
*  — отсутствие отношения приоритета;
*  — множественные токены в зоне;
*  — условный приоритет Art003LanguageCase;
*  — независимые зоны;
*  — дублирование кандидатов;
*  — частичное пересечение зон с разными ключами.
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
    QTest::addColumn<CandidateError>("candidate");
    QTest::addColumn<QSet<int>>("zone");
    QTest::addColumn<QSet<QString>>("initialRuleIds");
    QTest::addColumn<PriorityIndex>("priorityIndex");
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("repeatCount");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QSet<QString>>("expectedRuleIds");
    QTest::addColumn<QSet<int>>("existingZoneKey");
    QTest::addColumn<QSet<QString>>("existingZoneRuleIds");

    // ====================================================================
    // 5.1 — Пустая зона
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
            << QSet<QString>{QStringLiteral("ART-001")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("DET-001")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("DET-001")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("ART-001"), QStringLiteral("PREP-001")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("CONJ-001")}
            << QSet<int>()
            << QSet<QString>();
    }

    // ====================================================================
    // 5.5a — Подавление по приоритету в мульти-токенной зоне
    // ====================================================================
    {
        PriorityIndex pi;
        pi.conditionsByHigherRule[QStringLiteral("ART-001")]
            [QStringLiteral("ART-002")] = PriorityConditionKind::Always;
        QTest::addRow("5.5a_multi_token_suppress")
            << makeCandidate(QStringLiteral("ART-002"), QSet<int>{2, 3})
            << QSet<int>{2, 3}
            << QSet<QString>{QStringLiteral("ART-001")}
            << pi
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("ART-003")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("ART-001"), QStringLiteral("ART-003")}
            << QSet<int>()
            << QSet<QString>();
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
            << QSet<QString>{QStringLiteral("ART-001")}
            << (QSet<int>{2})
            << QSet<QString>{QStringLiteral("DET-001")};
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
            << QSet<QString>{QStringLiteral("CONJ-001")}
            << QSet<int>()
            << QSet<QString>();
    }

    // ====================================================================
    // 5.10 — Частичное пересечение: новый {3,4}, старый {2,3}
    // ====================================================================
    {
        QTest::addRow("5.10_partial_overlap_34_vs_23")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{3, 4})
            << QSet<int>{3, 4}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")}
            << (QSet<int>{2, 3})
            << QSet<QString>{QStringLiteral("DET-001")};
    }

    // ====================================================================
    // 5.11 — Частичное пересечение: новый {3,4}, старый {3}
    // ====================================================================
    {
        QTest::addRow("5.11_partial_overlap_34_vs_3")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{3, 4})
            << QSet<int>{3, 4}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")}
            << (QSet<int>{3})
            << QSet<QString>{QStringLiteral("DET-001")};
    }

    // ====================================================================
    // 5.12 — Частичное пересечение: новый {4}, старый {3,4}
    // ====================================================================
    {
        QTest::addRow("5.12_partial_overlap_4_vs_34")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{4})
            << QSet<int>{4}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")}
            << (QSet<int>{3, 4})
            << QSet<QString>{QStringLiteral("DET-001")};
    }

    // ====================================================================
    // 5.13 — Частичное пересечение: новый {3}, старый {3,4}
    // ====================================================================
    {
        QTest::addRow("5.13_partial_overlap_3_vs_34")
            << makeCandidate(QStringLiteral("ART-001"), QSet<int>{3})
            << QSet<int>{3}
            << QSet<QString>()
            << PriorityIndex()
            << makeRawSentence(1, QStringLiteral("test"), QStringLiteral("x"))
            << 1
            << 1
            << QSet<QString>{QStringLiteral("ART-001")}
            << (QSet<int>{3, 4})
            << QSet<QString>{QStringLiteral("DET-001")};
    }
}

void TEST_ResolveCandidate::TestResolveCandidate()
{
    QFETCH(CandidateError, candidate);
    QFETCH(QSet<int>, zone);
    QFETCH(QSet<QString>, initialRuleIds);
    QFETCH(PriorityIndex, priorityIndex);
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, repeatCount);
    QFETCH(int, expectedCount);
    QFETCH(QSet<QString>, expectedRuleIds);
    QFETCH(QSet<int>, existingZoneKey);
    QFETCH(QSet<QString>, existingZoneRuleIds);

    const QString tag = QString(QTest::currentDataTag());

    SentenceModel sentence = buildSentenceModel(rawSentence);
    CheckerRuntime runtime;
    runtime.priorityIndex = priorityIndex;

    ConflictZoneMap zoneMap;

    // Инициализируем целевую зону кандидатами из initialRuleIds
    if (!initialRuleIds.isEmpty()) {
        addInitialToZone(zoneMap, zone, initialRuleIds);
    }

    // Инициализируем существующую зону, если она задана
    if (!existingZoneKey.isEmpty()) {
        addInitialToZone(zoneMap, existingZoneKey, existingZoneRuleIds);
    }

    // Вызываем resolveCandidate repeatCount раз
    for (int i = 0; i < repeatCount; ++i) {
        resolveCandidate(candidate, zoneMap, sentence, runtime);
    }

    // Проверяем целевую зону
    compareZoneCandidates(tag, zoneMap, zone, expectedCount, expectedRuleIds);

    // Проверяем, что существующая зона осталась нетронутой
    if (!existingZoneKey.isEmpty()) {
        compareZoneCandidates(tag + QStringLiteral("_existing"), zoneMap,
                              existingZoneKey, existingZoneRuleIds.size(), existingZoneRuleIds);
    }
}
