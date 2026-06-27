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
/*!
* \brief Точка входа консольного приложения.
* \param [in] argc Количество аргументов командной строки.
* \param [in] argv Массив аргументов командной строки.
* \return 0 при успехе, 1 при ошибке.
*
* Выполняет полный конвейер: разбор аргументов, чтение входного файла,
* анализ правилами и запись результата. Любая ошибка выводится в stderr.
*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    try {
        const CheckerRuntime runtime = runSetup(app.arguments().mid(1));
        const DocumentModel document = runInput(runtime);
        const QSet<CandidateError> errors = runAnalysis(document, runtime);
        writeOutput(errors, document, runtime);
    } catch (const Diagnostic& d) {
        printDiagnostic(d);
        return 1;
    }

    return 0;
}
