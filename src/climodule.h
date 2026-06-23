/*!
* \file climodule.h
* \brief Интерфейс модуля разбора аргументов командной строки.
*
* Объявляет функцию parseCommandLine, преобразующую список аргументов
* в структуру RunConfig или Diagnostic{CliUsageError} при ошибке.
*/

#pragma once

#include "datamodel.h"
#include <QStringList>
#include <variant>

/*!
* \brief Разобрать аргументы командной строки в RunConfig.
* \param [in] args Список аргументов без имени программы (argv[0]).
* \return RunConfig при успехе, Diagnostic{CliUsageError} при ошибке.
*
* Ожидаемый формат: <input> <output> [--lists <dir>].
* Флаг --lists может стоять до или после позиционных аргументов.
* Позиционных аргументов должно быть ровно два: путь ко входному файлу
* и путь к выходному файлу.
*/
std::variant<RunConfig, Diagnostic> parseCommandLine(const QStringList& args);

std::variant<CheckerRuntime, Diagnostic> runSetup(const QStringList& args);