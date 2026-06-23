/*!
* \file checkersystem.h
* \brief Функции системы грамматической проверки.
*
* Содержит resolveCandidate и checkSentence.
* Типы (CandidateError, Rule, CheckerRuntime и др.) находятся в datamodel.h.
*/

#ifndef CHECKERSYSTEM_H
#define CHECKERSYSTEM_H

#pragma once

#include "datamodel.h"

/*!
* \brief Разрешение конфликтов приоритетов между кандидатами.
* \param [in] candidate Новый кандидат ошибки.
* \param [in,out] zoneMap Текущая карта зон конфликтов.
* \param [in] sentence Предложение для проверки условий приоритета.
* \param [in] runtime Контекст с PriorityIndex.
*
* Инкрементально добавляет кандидата в зону, подавляя менее приоритетные
* кандидаты при необходимости.
*/
void resolveCandidate(CandidateError candidate,
                      ConflictZoneMap& zoneMap,
                      const SentenceModel& sentence,
                      const CheckerRuntime& runtime);

/*!
* \brief Проверка предложения всеми правилами и формирование финального набора ошибок.
* \param [in] sentence Предложение для анализа.
* \param [in] sentenceIndex Индекс предложения в document.sentences.
* \param [in] document Модель документа для навигации.
* \param [in] runtime Runtime-контекст (dispatch, priorityIndex).
* \return Финальный набор неподавленных кандидатов.
*/
QSet<CandidateError> checkSentence(const SentenceModel& sentence,
                                   int sentenceIndex,
                                   const DocumentModel& document,
                                   const CheckerRuntime& runtime);

// Цикл проверки всех предложений документа. Для каждого предложения
// вызывает checkSentence и накапливает найденные ошибки в общий набор.
QSet<CandidateError> runAnalysis(const DocumentModel& document,
                                 const CheckerRuntime& runtime);

#endif // CHECKERSYSTEM_H
