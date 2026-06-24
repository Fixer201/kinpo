/*!
* \file runtimemodule.h
* \brief Инициализация runtime-контекста проверки.
*
* Объявляет функцию initializeRuntime, которая по параметрам запуска
* строит CheckerRuntime с диспетчеризацией правил и загруженными словарями.
*/

#pragma once

#include "datamodel.h"
#include <QString>
#include <variant>

/*!
* \brief Построить CheckerRuntime по параметрам запуска.
* \param [in] config Параметры запуска с путями к файлам и каталогу словарей.
* \return CheckerRuntime при успехе, Diagnostic при ошибке загрузки словарей.
*
* Заполняет диспетчеризацию правил по UPOS, индекс приоритетов и ресурсы.
*/
std::variant<CheckerRuntime, Diagnostic> initializeRuntime(const RunConfig& config);