/*!
* \file inputmodule.h
* \brief Интерфейс модуля ввода для чтения CoNLL-U блоков.
*
* Определяет функцию parseSentenceBlock, выполняющую разбор одного блока строк
* (одного предложения в формате CoNLL-U) в структуру RawSentence с локальной
* валидацией каждой строки.
*/

#pragma once

#include "datamodel.h"
#include <QStringList>
#include <variant>

/*!
* \brief Разобрать один блок строк CoNLL-U в RawSentence.
* \param [in] block Список строк одного предложения (без пустых-разделительных строк).
* \param [in] firstLineNumber Номер первой строки блока во входном файле.
* \return RawSentence при успехе, или Diagnostic{kind=InputFormatError} при ошибке.
*
* Выполняет локальную валидацию:
*  - наличие комментариев # sent_id и # text;
*  - формат комментариев (# space content);
*  - количество колонок (ровно 10);
*  - корректность ID, HEAD, UPOS, FORM;
*  - строгий порядок ID (1, 2, 3...);
*  - формат MWT (N-M) и проверка непустых колонок.
*/
std::variant<RawSentence, Diagnostic> parseSentenceBlock(
    const QStringList& block, int firstLineNumber);
