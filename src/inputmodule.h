/*!
* \file inputmodule.h
* \brief Интерфейс модуля ввода для чтения CoNLL-U блоков.
*
* Определяет функции парсинга CoNLL-U: parseSentenceBlock — разбор одного
* блока строк (одного предложения) в структуру RawSentence с локальной
* валидацией каждой строки; validateSentenceStructure — проверка
* структурных ограничений, требующих знания всех токенов сразу.
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
*  - формат MWT (N-M) и проверку непустых колонок.
*/
std::variant<RawSentence, Diagnostic> parseSentenceBlock(
    const QStringList& block, int firstLineNumber);

/*!
* \brief Проверить корректность структуры предложения по условиям, для которых нужно знать все токены сразу.
* \param [in] sentence Валидированное предложение (после parseSentenceBlock).
* \return std::nullopt если ошибок нет; Diagnostic{kind=InputFormatError} если структура нарушена.
*
* Проверяет:
*  - число токенов <= 200;
*  - для каждого MwtRecord: rangeStart/rangeEnd в диапазоне ID токенов, все ID в диапазоне присутствуют и образуют непрерывную последовательность;
*  - для каждого токена: HEAD в {ID токенов} ∪ {0}, HEAD != ID токена;
*  - ровно один токен имеет HEAD=0;
*  - нет циклов в дереве зависимостей (обход цепочки HEAD с маркировкой "в пути" / "завершён").
*/
std::optional<Diagnostic> validateSentenceStructure(const RawSentence& sentence);