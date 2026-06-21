/*!
* \file modelbuilder.h
* \brief Интерфейс модуля построения аналитической модели.
*
* Содержит функции построения SentenceModel из валидированного
* RawSentence, а также преобразования строковых UPOS/DEPREL/FEATS
* в типизированные enum-значения.
*/

#pragma once

#include "datamodel.h"

/*!
* \brief Преобразует строковый UPOS-тег в enum.
* \param [in] upos Строка из входного файла (например, "NOUN").
* \return Соответствующее значение Upos; Upos::X для неизвестного тега.
* \note Вызывается в buildSentenceModel при построении TokenNode из RawToken.
*/
Upos parseUpos(const QString& upos);

/*!
* \brief Преобразует строковый DEPREL-тег в enum.
* \param [in] deprel Строка из входного файла (например, "nsubj").
* \return Соответствующее значение Deprel; Deprel::Other для тегов,
*         не используемых правилами.
* \note Вызывается в buildSentenceModel при построении TokenNode из RawToken.
*/
Deprel parseDeprel(const QString& deprel);

/*!
* \brief Построить SentenceModel из валидированного RawSentence.
* \param [in] rawSentence Сырое предложение после validateSentenceStructure.
* \return Внутреннее представление предложения с деревом зависимостей,
*         линейными связями и типизированными полями.
*
* Алгоритм (см. внутреннюю спецификацию, раздел 4.2.1):
*  1. Создаёт TokenNode из каждого RawToken (конвертация UPOS, DEPREL, FEATS).
*  2. Заполняет линейный список tokens и хеш tokensById.
*  3. Связывает previousToken / nextToken.
*  4. Строит дерево parent / children по headId, находит rootToken.
*  5. Помечает isMwtFragment для токенов в диапазонах MWT.
*/
SentenceModel buildSentenceModel(const RawSentence& rawSentence);