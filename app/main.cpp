/*!
* \file main.cpp
* \brief Точка входа консольного приложения грамматического анализатора.
*
* Выполняет полный конвейер:
*   1. runSetup — разбор аргументов CLI, инициализация runtime.
*   2. runInput — чтение файла, парсинг CoNLL-U, построение модели.
*   3. runAnalysis — проверка всех предложений правилами.
*   4. writeOutput — форматирование и запись результата.
*
* При возникновении Diagnostic на любом этапе печатает сообщение
* в stderr и завершается с кодом 1.
*/

#include "climodule.h"
#include "inputpipeline.h"
#include "checkersystem.h"
#include "outputmodule.h"
#include <QCoreApplication>
#include <QTextStream>
#include <iostream>

/*!
* \brief Вывести Diagnostic в stderr.
*/
static void printDiagnostic(const Diagnostic& d)
{
    QTextStream err(stderr);
    err << QStringLiteral("[") << diagnosticKindToString(d.kind)
        << QStringLiteral("] ");
    if (d.lineNumber.has_value())
        err << QStringLiteral("строка ") << d.lineNumber.value() << QStringLiteral(": ");
    if (d.sentId.has_value())
        err << QStringLiteral("предложение ") << d.sentId.value() << QStringLiteral(": ");
    err << d.message << Qt::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Настройка: аргументы CLI + инициализация runtime
    auto setupResult = runSetup(app.arguments().mid(1));
    if (std::holds_alternative<Diagnostic>(setupResult)) {
        printDiagnostic(std::get<Diagnostic>(setupResult));
        return 1;
    }
    const CheckerRuntime& runtime = std::get<CheckerRuntime>(setupResult);

    // Ввод: чтение файла + парсинг + построение модели
    auto inputResult = runInput(runtime);
    if (std::holds_alternative<Diagnostic>(inputResult)) {
        printDiagnostic(std::get<Diagnostic>(inputResult));
        return 1;
    }
    const DocumentModel& document = std::get<DocumentModel>(inputResult);

    // Анализ: проверка всех предложений правилами
    QSet<CandidateError> errors = runAnalysis(document, runtime);

    // Вывод: форматирование и запись результата
    auto writeResult = writeOutput(errors, document, runtime);
    if (writeResult.has_value()) {
        printDiagnostic(writeResult.value());
        return 1;
    }

    return 0;
}
