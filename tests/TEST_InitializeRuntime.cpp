#include <QtTest>
#include <QObject>
#include <variant>

#include "TEST_InitializeRuntime.h"
#include "datamodel.h"
#include "runtimemodule.h"
#include "auxiliaryfunctionsfortesting.h"

TEST_InitializeRuntime::TEST_InitializeRuntime() {}
TEST_InitializeRuntime::~TEST_InitializeRuntime() {}

void TEST_InitializeRuntime::TestInitializeRuntime_data()
{
    QTest::addColumn<QString>("listsDir");
    QTest::addColumn<bool>("shouldSucceed");
    QTest::addColumn<int>("expectedPriorityEntries");

    QTest::addRow("ok_builtin_lists")
        << findListsDir()
        << true
        << 3;

    QTest::addRow("err_nonexistent_lists")
        << QStringLiteral("/nonexistent/path")
        << false
        << 3;
}

void TEST_InitializeRuntime::TestInitializeRuntime()
{
    QFETCH(QString, listsDir);
    QFETCH(bool, shouldSucceed);
    QFETCH(int, expectedPriorityEntries);

    RunConfig config;
    config.inputPath = QStringLiteral("input.conllu");
    config.outputPath = QStringLiteral("output.txt");
    config.listsDir = listsDir;

    std::variant<CheckerRuntime, Diagnostic> result = initializeRuntime(config);

    if (shouldSucceed) {
        QVERIFY(std::holds_alternative<CheckerRuntime>(result));
        const CheckerRuntime& rt = std::get<CheckerRuntime>(result);

        // dispatch должен быть непустым
        QVERIFY(rt.dispatch.size() > 0);

        // PriorityIndex должен содержать 3 записи из раздела 2.4.1
        int priorityCount = 0;
        for (auto it = rt.priorityIndex.conditionsByHigherRule.begin();
             it != rt.priorityIndex.conditionsByHigherRule.end(); ++it) {
            priorityCount += it.value().size();
        }
        QCOMPARE(priorityCount, expectedPriorityEntries);

        // config должен быть сохранён в runtime
        QCOMPARE(rt.config.inputPath, config.inputPath);
        QCOMPARE(rt.config.outputPath, config.outputPath);
    } else {
        // Ожидаем Diagnostic — словари не загрузились
        QVERIFY(std::holds_alternative<Diagnostic>(result));
        const Diagnostic& d = std::get<Diagnostic>(result);
        QCOMPARE(d.kind, DiagnosticKind::ResourceLoadError);
    }
}