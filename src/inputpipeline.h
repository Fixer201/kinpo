/*!
* \file inputpipeline.h
* \brief Интерфейс слоя ввода: чтение, парсинг, валидация, построение модели.
*
* Объявляет функции для последовательной обработки входного файла CoNLL-U:
*  readFile читает файл в список строк,
*  parseAndValidate разбивает на блоки и проверяет формат,
*  buildModel строит аналитическую модель документа,
*  runInput объединяет все три этапа.
*/

#pragma once

#include "datamodel.h"
#include <QStringList>

// Читает входной файл CoNLL-U в список строк.
// При ошибке доступа выбрасывает Diagnostic{InputFileError}.
QStringList readFile(const QString& path);

// Разбивает список строк на блоки предложений, парсит каждый блок
// и проверяет структуру. При ошибке выбрасывает Diagnostic{InputFormatError}.
RawDocument parseAndValidate(const QStringList& lines);

// Преобразует RawDocument в DocumentModel с деревьями зависимостей.
DocumentModel buildModel(const RawDocument& rawDoc);

/*!
* \brief Оркестратор слоя ввода: readFile, parseAndValidate, buildModel.
* \param [in] runtime Runtime-контекст с путём ко входному файлу.
* \return Построенная модель документа.
* \throws Diagnostic при ошибке любого этапа.
*/
DocumentModel runInput(const CheckerRuntime& runtime);