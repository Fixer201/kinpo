/*!
* \file TEST_ParseCommandLine.cpp
* \brief Тесты функции parseCommandLine.
*
* Раздел 2.4.1 внешней спецификации:
*   checker <input.conllu> <output.txt> [--lists <lists_dir>]
*
* Функция принимает список аргументов без имени программы (argv[0])
* и возвращает RunConfig при успехе или Diagnostic{CliUsageError} при ошибке.
*
* Проверяемые кейсы:
*  - корректный базовый вызов (2 позиционных, без --lists);
*  - корректный вызов с --lists (2 позиционных + флаг + путь);
*  - недостаточно аргументов (0 или 1 позиционный);
*  - --lists без значения;
*  - лишние позиционные аргументы (3 и более);
*  - неизвестный флаг.
*/

#include <QtTest>
#include <QObject>

#include "TEST_ParseCommandLine.h"
#include "datamodel.h"
#include "climodule.h"

TEST_ParseCommandLine::TEST_ParseCommandLine() {}
TEST_ParseCommandLine::~TEST_ParseCommandLine() {}

void TEST_ParseCommandLine::TestParseCommandLine_data()
{
    QTest::addColumn<QStringList>("args");
    QTest::addColumn<bool>("expectOk");
    QTest::addColumn<QString>("expectedInputPath");
    QTest::addColumn<QString>("expectedOutputPath");
    QTest::addColumn<bool>("expectListsDir");
    QTest::addColumn<QString>("expectedListsDir");

    // Корректный базовый вызов: input output
    QTest::addRow("ok_basic")
        << QStringList{QStringLiteral("input.conllu"), QStringLiteral("output.txt")}
        << true
        << QStringLiteral("input.conllu")
        << QStringLiteral("output.txt")
        << false
        << QString();

    // Корректный вызов с --lists
    QTest::addRow("ok_with_lists")
        << QStringList{QStringLiteral("input.conllu"), QStringLiteral("output.txt"),
                        QStringLiteral("--lists"), QStringLiteral("/path/to/lists")}
        << true
        << QStringLiteral("input.conllu")
        << QStringLiteral("output.txt")
        << true
        << QStringLiteral("/path/to/lists");

    // --lists перед позиционными — тоже допустимо
    QTest::addRow("ok_lists_first")
        << QStringList{QStringLiteral("--lists"), QStringLiteral("/custom/lists"),
                        QStringLiteral("in.conllu"), QStringLiteral("out.txt")}
        << true
        << QStringLiteral("in.conllu")
        << QStringLiteral("out.txt")
        << true
        << QStringLiteral("/custom/lists");

    // Недостаточно аргументов: 0
    QTest::addRow("err_no_args")
        << QStringList{}
        << false
        << QString() << QString() << false << QString();

    // Недостаточно аргументов: 1
    QTest::addRow("err_one_arg")
        << QStringList{QStringLiteral("only_input.conllu")}
        << false
        << QString() << QString() << false << QString();

    // --lists без значения
    QTest::addRow("err_lists_no_value")
        << QStringList{QStringLiteral("input.conllu"), QStringLiteral("output.txt"),
                        QStringLiteral("--lists")}
        << false
        << QString() << QString() << false << QString();

    // Лишний позиционный аргумент
    QTest::addRow("err_extra_positional")
        << QStringList{QStringLiteral("a.conllu"), QStringLiteral("b.txt"),
                        QStringLiteral("extra")}
        << false
        << QString() << QString() << false << QString();

    // Неизвестный флаг
    QTest::addRow("err_unknown_flag")
        << QStringList{QStringLiteral("a.conllu"), QStringLiteral("b.txt"),
                        QStringLiteral("--foo"), QStringLiteral("bar")}
        << false
        << QString() << QString() << false << QString();
}

void TEST_ParseCommandLine::TestParseCommandLine()
{
    QFETCH(QStringList, args);
    QFETCH(bool, expectOk);
    QFETCH(QString, expectedInputPath);
    QFETCH(QString, expectedOutputPath);
    QFETCH(bool, expectListsDir);
    QFETCH(QString, expectedListsDir);

    const QString tag = QString(QTest::currentDataTag());

    if (expectOk) {
        RunConfig cfg;
        try {
            cfg = parseCommandLine(args);
        } catch (const Diagnostic& d) {
            qDebug() << "[" << tag << "] неожиданная ошибка:" << d.message;
            QFAIL("parseCommandLine выбросил Diagnostic при корректных аргументах");
        }
        QCOMPARE(cfg.inputPath, expectedInputPath);
        QCOMPARE(cfg.outputPath, expectedOutputPath);
        QCOMPARE(cfg.listsDir.has_value(), expectListsDir);
        if (expectListsDir)
            QCOMPARE(cfg.listsDir.value(), expectedListsDir);
    } else {
        bool caught = false;
        try {
            parseCommandLine(args);
        } catch (const Diagnostic& d) {
            caught = true;
            QCOMPARE(d.kind, DiagnosticKind::CliUsageError);
        }
        QVERIFY2(caught, qPrintable(QStringLiteral("[%1] ожидалось исключение Diagnostic").arg(tag)));
    }
}