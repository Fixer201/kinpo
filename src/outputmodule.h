/*!
* \file outputmodule.h
* \brief Интерфейс слоя вывода: построение строк результата и запись файла.
*
* Объявляет buildCorrection для формирования строки «Исправление»
* и writeOutput для форматирования всех ошибок и атомарной записи файла.
*/

#pragma once

#include "datamodel.h"
#include <QSet>
#include <QStringList>
#include <optional>
#include <variant>

// Применяет edits к displayTokenIds и возвращает строку исправления.
// Конкатенирует FORM оставшихся и добавленных токенов через пробел.
QString buildCorrection(const CandidateError& ce,
                         const SentenceModel& sentence);

// Форматирует все найденные ошибки в строки и записывает выходной файл.
// При отсутствии ошибок пишет "NO ERRORS FOUND".
// Возвращает nullopt при успехе или Diagnostic{OutputWriteError} при ошибке записи.
std::optional<Diagnostic> writeOutput(const QSet<CandidateError>& errors,
                                      const DocumentModel& document,
                                      const CheckerRuntime& runtime);