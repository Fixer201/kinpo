/*!
* \file climodule.cpp
* \brief Реализация разбора аргументов командной строки.
*
* Разбирает список аргументов в RunConfig или Diagnostic{CliUsageError}.
* Поддерживается формат: <input> <output> [--lists <dir>].
* Флаг --lists может стоять до или после позиционных аргументов.
*/

#include "climodule.h"

namespace {

/*! \brief Создать Diagnostic ошибки аргументов командной строки. */
Diagnostic makeCliError(const QString& message)
{
    Diagnostic d;
    d.kind = DiagnosticKind::CliUsageError;
    d.message = message;
    d.code = -1;
    return d;
}

} // namespace

std::variant<RunConfig, Diagnostic> parseCommandLine(const QStringList& args)
{
    QStringList positional;
    std::optional<QString> listsDir;

    // Обходим аргументы, разделяя позиционные и флаги
    for (int i = 0; i < args.size(); ++i) {
        const QString& arg = args[i];

        if (arg == QStringLiteral("--lists")) {
            // Флаг --lists требует значения — следующий аргумент
            if (i + 1 >= args.size())
                return makeCliError(QStringLiteral("Флаг --lists требует значение: путь к каталогу словарей."));
            listsDir = args[++i];
            continue;
        }

        // Неизвестный флаг (начинается с --)
        if (arg.startsWith(QStringLiteral("--")))
            return makeCliError(QStringLiteral("Неизвестный флаг: %1").arg(arg));

        // Позиционный аргумент
        positional.append(arg);
    }

    // Позиционных аргументов должно быть ровно два
    if (positional.size() < 2)
        return makeCliError(QStringLiteral("Недостаточно аргументов. Использование: checker <input.conllu> <output.txt> [--lists <dir>]."));
    if (positional.size() > 2)
        return makeCliError(QStringLiteral("Лишние позиционные аргументы. Использование: checker <input.conllu> <output.txt> [--lists <dir>]."));

    RunConfig config;
    config.inputPath = positional[0];
    config.outputPath = positional[1];
    config.listsDir = listsDir;

    return config;
}