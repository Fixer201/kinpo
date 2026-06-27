#include <QtTest>
#include <QObject>

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
        << 4;

    QTest::addRow("err_nonexistent_lists")
        << QStringLiteral("/nonexistent/path")
        << false
        << 4;
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

    if (shouldSucceed) {
        CheckerRuntime rt;
        try {
            rt = initializeRuntime(config);
        } catch (const Diagnostic& d) {
            qDebug() << "[TEST FAIL] initializeRuntime выбросил исключение:" << d.message;
            QFAIL("initializeRuntime должен был успешно инициализировать runtime");
        }

        // dispatch должен быть непустым
        QVERIFY(rt.dispatch.size() > 0);

        // PriorityIndex должен содержать 4 записи приоритетов между правилами
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
        bool caught = false;
        try {
            initializeRuntime(config);
        } catch (const Diagnostic& d) {
            caught = true;
            QCOMPARE(d.kind, DiagnosticKind::ResourceLoadError);
        }
        QVERIFY(caught);
    }
}