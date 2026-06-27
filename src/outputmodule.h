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

/*!
* \brief Применить edits к displayTokenIds и сформировать строку исправления.
* \param [in] ce Кандидат ошибки с edits и displayTokenIds.
* \param [in] sentence Предложение для доступа к форме токенов.
* \return Строка исправления (FORM через пробел).
*
* Конкатенирует FORM оставшихся и добавленных токенов через пробел.
*/
QString buildCorrection(const CandidateError& ce,
                         const SentenceModel& sentence);

/*!
* \brief Форматировать все найденные ошибки и записать выходной файл.
* \param [in] errors Набор найденных ошибок.
* \param [in] document Модель документа для доступа к предложениям.
* \param [in] runtime Runtime-контекст с путём к выходному файлу.
* \throws Diagnostic{OutputWriteError} при ошибке записи.
*
* При отсутствии ошибок пишет "NO ERRORS FOUND".
* Запись атомарная: временный файл, затем переименование.
*/
void writeOutput(const QSet<CandidateError>& errors,
                 const DocumentModel& document,
                 const CheckerRuntime& runtime);