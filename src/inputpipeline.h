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
#include <variant>

// Читает входной файл CoNLL-U в список строк.
// Возвращает QStringList при успехе или Diagnostic{InputFileError} при ошибке доступа.
std::variant<QStringList, Diagnostic> readFile(const QString& path);

// Разбивает список строк на блоки предложений, парсит каждый блок
// и проверяет структуру. Возвращает RawDocument или Diagnostic{InputFormatError}.
std::variant<RawDocument, Diagnostic> parseAndValidate(const QStringList& lines);

// Преобразует RawDocument в DocumentModel с деревьями зависимостей.
DocumentModel buildModel(const RawDocument& rawDoc);

// Оркестратор слоя ввода: readFile, parseAndValidate, buildModel.
// Возвращает DocumentModel при успехе или Diagnostic при ошибке любого этапа.
std::variant<DocumentModel, Diagnostic> runInput(const CheckerRuntime& runtime);