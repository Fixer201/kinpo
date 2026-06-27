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

/*!
* \brief Читать входной файл CoNLL-U в список строк.
* \param [in] path Путь ко входному файлу.
* \return Список строк файла.
* \throws Diagnostic{InputFileError} при ошибке доступа к файлу.
*/
QStringList readFile(const QString& path);

/*!
* \brief Разбить строки на блоки предложений, распарсить и проверить.
* \param [in] lines Строки входного файла.
* \return Валидированное сырое представление документа.
* \throws Diagnostic{InputFormatError} при ошибке формата.
*/
RawDocument parseAndValidate(const QStringList& lines);

/*!
* \brief Преобразовать RawDocument в DocumentModel с деревьями зависимостей.
* \param [in] rawDoc Валидированное сырое представление.
* \return Аналитическая модель документа с индексом sentById.
*/
DocumentModel buildModel(const RawDocument& rawDoc);

/*!
* \brief Оркестратор слоя ввода: readFile, parseAndValidate, buildModel.
* \param [in] runtime Runtime-контекст с путём ко входному файлу.
* \return Построенная модель документа.
* \throws Diagnostic при ошибке любого этапа.
*/
DocumentModel runInput(const CheckerRuntime& runtime);