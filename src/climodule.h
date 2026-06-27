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

/*!
* \brief Разобрать аргументы командной строки в RunConfig.
* \param [in] args Список аргументов без имени программы (argv[0]).
* \return Заполненная структура RunConfig.
* \throws Diagnostic{CliUsageError} при ошибке разбора.
*
* Ожидаемый формат: <input> <output> [--lists <dir>].
* Флаг --lists может стоять до или после позиционных аргументов.
* Позиционных аргументов должно быть ровно два: путь ко входному файлу
* и путь к выходному файлу.
*/
RunConfig parseCommandLine(const QStringList& args);

/*!
* \brief Оркестратор слоя настройки: разбор аргументов и инициализация runtime.
* \param [in] args Список аргументов без имени программы.
* \return Заполненный контекст CheckerRuntime.
* \throws Diagnostic при ошибке разбора аргументов или инициализации ресурсов.
*/
CheckerRuntime runSetup(const QStringList& args);