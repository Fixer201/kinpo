/*!
* \file auxiliaryfunctionsfortesting.h
* \brief Хелперы для создания тестовых данных и сравнения сложных структур в тестах.
*
* Предоставляет функции для сборки строк CoNLL-U, создания RawSentence
* вручную и сравнения результатов с детальным логированием.
*/

#ifndef AUXILIARYFUNCTIONSFORTESTING_H
#define AUXILIARYFUNCTIONSFORTESTING_H

#include <QString>
#include <QStringList>
#include <optional>
#include "datamodel.h"
#include "checkersystem.h"

// ========================================================================
// Хелперы создания строк CoNLL-U
// ========================================================================

/*!
* \brief Собирает строку CoNLL-U из заданных полей.
* \param [in] id       ID токена (целое число).
* \param [in] form     Словоформа (FORM), обязательное поле.
* \param [in] lemma    Лемма (LEMMA).
* \param [in] upos     Универсальная часть речи (UPOS).
* \param [in] head     ID головного токена (HEAD).
* \param [in] deprel   Синтаксическое отношение (DEPREL).
* \param [in] xpos     Языковоспецифичный тег (XPOS, по умолчанию "_").
* \param [in] feats    Признаки (FEATS, по умолчанию "_").
* \param [in] deps     DEPS (по умолчанию "_").
* \param [in] misc     MISC (по умолчанию "_").
* \return Строка с 10 колонками через табуляцию.
*/
QString makeTokenLine(int id,
                      const QString& form,
                      const QString& lemma,
                      const QString& upos,
                      int head,
                      const QString& deprel,
                      const QString& xpos = QStringLiteral("_"),
                      const QString& feats = QStringLiteral("_"),
                      const QString& deps = QStringLiteral("_"),
                      const QString& misc = QStringLiteral("_"));

/*!
* \brief Собирает строку MWT (Multi-Word Token) CoNLL-U.
* \param [in] rangeId ID диапазона в формате "N-M".
* \param [in] form    Исходная форма (FORM).
* \return Строка с 10 колонками: ID, FORM, и 8 символов "_".
*/
QString makeMwtLine(const QString& rangeId,
                    const QString& form);

/*!
* \brief Формирует строку комментария CoNLL-U.
* \param [in] content Содержимое комментария без ведущего "# ".
* \return Строка вида "# content".
*/
QString makeComment(const QString& content);

// ========================================================================
// Хелперы создания RawSentence вручную (изолированно от парсера)
// ========================================================================

/*!
* \brief Создаёт пустой RawSentence с заданными метаданными.
* \param [in] firstLine Номер первой строки блока.
* \param [in] sentId    Идентификатор предложения.
* \param [in] text      Исходный текст предложения.
* \return Объект RawSentence с пустыми списками tokens и mwtRecords.
*/
RawSentence makeRawSentence(int firstLine, const QString& sentId, const QString& text);

/*!
* \brief Создаёт RawToken с минимальным набором полей (остальные = "_").
* \param [in] lineNumber Номер строки во входном файле.
* \param [in] id         Целочисленный ID токена.
* \param [in] form       Словоформа (FORM).
* \param [in] upos       Универсальная часть речи (UPOS).
* \param [in] headId     ID головного токена (HEAD).
* \param [in] deprel     Синтаксическое отношение (DEPREL).
* \return Объект RawToken с заполненными полями.
*/
RawToken makeRawToken(int lineNumber, int id, const QString& form,
                      const QString& upos, int headId, const QString& deprel);

/*!
* \brief Создаёт RawToken с минимальным набором полей (остальные = "_").
* \param [in] lineNumber Номер строки во входном файле.
* \param [in] id         Целочисленный ID токена.
* \param [in] form       Словоформа (FORM).
* \param [in] upos       Универсальная часть речи (UPOS).
* \param [in] headId     ID головного токена (HEAD).
* \param [in] deprel     Синтаксическое отношение (DEPREL).
* \param [in] featsRaw   Синтаксические признаки (FEATS).
* \return Объект RawToken с заполненными полями.
*/
RawToken makeRawToken(int lineNumber, int id, const QString& form,
                      const QString& upos, int headId, const QString& deprel,
                      const QString& featsRaw);


/*!
* \brief Добавляет токен в список tokens предложения.
* \param [in,out] sentence Предложение, в которое добавляется токен.
* \param [in]     token    Токен для добавления.
*/
void addToken(RawSentence& sentence, const RawToken& token);

/*!
* \brief Добавляет MWT-запись в список mwtRecords предложения.
* \param [in,out] sentence Предложение, в которое добавляется MWT.
* \param [in] lineNumber   Номер строки MWT.
* \param [in] rangeStart   Начальный ID диапазона.
* \param [in] rangeEnd     Конечный ID диапазона.
* \param [in] form         Исходная форма.
*/
void addMwt(RawSentence& sentence, int lineNumber, int rangeStart, int rangeEnd, const QString& form);

// ========================================================================
// Хелперы сравнения (с детальным логированием при несовпадении)
// ========================================================================

/*!
* \brief Сравнивает структуру RawSentence с ожидаемыми значениями.
* \param [in] testName                Имя текущего теста (для логирования).
* \param [in] actual                  Фактический результат.
* \param [in] expectedSentId          Ожидаемый sentId.
* \param [in] expectedText            Ожидаемый text.
* \param [in] expectedTokenCount      Ожидаемое число токенов.
* \param [in] expectedFirstTokenForm  Ожидаемая FORM первого токена.
* \param [in] expectedLastTokenUpos   Ожидаемый UPOS последнего токена.
*
* При несовпадении выводит в лог имя теста и расхождение.
*/
void compareRawSentence(const QString& /*testName*/,
                        const RawSentence& actual,
                        const QString& expectedSentId,
                        const QString& expectedText,
                        int expectedTokenCount,
                        const QString& expectedFirstTokenForm,
                        const QString& expectedLastTokenUpos);

/*!
* \brief Сравнивает Diagnostic с ожидаемыми значениями.
* \param [in] testName       Имя текущего теста (для логирования).
* \param [in] actual         Фактический Diagnostic.
* \param [in] expectedKind   Ожидаемый diagnosticKindToString(...) (например, "InputFormatError").
* \param [in] expectedMessage Ожидаемое message.
*
* При несовпадении выводит в лог имя теста и расхождение.
*/
void compareDiagnostic(const QString& /*testName*/,
                       const Diagnostic& actual,
                       const QString& expectedKind,
                       const QString& expectedMessage);

/*!
* \brief Сравнивает std::optional<Diagnostic> с ожидаемым результатом.
* \param [in] testName       Имя текущего теста (для логирования).
* \param [in] actual         Фактический результат (nullopt или Diagnostic).
* \param [in] expectValid    true если ожидается std::nullopt, false если ожидается ошибка.
* \param [in] expectedMessage Ожидаемое сообщение об ошибке (игнорируется при expectValid=true).
*
* При ожидании успеха, но получении ошибки выводит сообщение ошибки в qDebug.
* При ожидании ошибки проверяет наличие значения и совпадение сообщения.
*/
void compareOptionalDiagnostic(const QString& /*testName*/,
                                const std::optional<Diagnostic>& actual,
                                bool expectValid,
                                const QString& expectedMessage);

// =====================================================================
// Хелперы сравнения CandidateError / QSet<CandidateError>
// =====================================================================

/*!
* \brief Сравнивает набор CandidateError с ожидаемыми ruleId.
* \param [in] testName      Имя текущего теста.
* \param [in] actual        Фактический набор кандидатов.
* \param [in] expectedRuleIds Множество ожидаемых идентификаторов правил.
*
* При несовпадении выводит в лог имя теста и расхождение.
*/
void compareCandidateSet(const QString& testName,
                         const QSet<CandidateError>& actual,
                         const QSet<QString>& expectedRuleIds);

/*!
* \brief Сравнивает наборы conflictTokenIds.
* \param [in] testName        Имя текущего теста.
* \param [in] actual            Фактический набор кандидатов.
* \param [in] expectedZones     Список ожидаемых conflictTokenIds.
*
* Каждый QSet<int> из expectedZones должен присутствовать у ровно одного кандидата.
* При несовпадении выводит в лог имя теста и расхождение.
*/
void compareConflictZones(const QString& testName,
                          const QSet<CandidateError>& actual,
                          const QList<QSet<int>>& expectedZones);

/*!
* \brief Сравнивает содержимое конкретной зоны ConflictZoneMap.
* \param [in] testName        Имя текущего теста.
* \param [in] zoneMap         Фактическая карта зон.
* \param [in] zoneKey         Ключ зоны (conflictTokenIds).
* \param [in] expectedCount   Ожидаемое число кандидатов в зоне.
* \param [in] expectedRuleIds Ожидаемые ruleId в зоне.
*
* При несовпадении выводит в лог имя теста и расхождение.
*/
void compareZoneCandidates(const QString& testName,
                           const ConflictZoneMap& zoneMap,
                           const QSet<int>& zoneKey,
                           int expectedCount,
                           const QSet<QString>& expectedRuleIds);

/*!
* \brief Сравнивает поля одного CandidateError с ожидаемыми значениями.
* \param [in] testName           Имя текущего теста.
* \param [in] actual             Фактический CandidateError.
* \param [in] expectedRuleId     Ожидаемый ruleId.
* \param [in] expectedDisplayIds Ожидаемые displayTokenIds.
* \param [in] expectedConflictIds Ожидаемые conflictTokenIds.
*
* При несовпадении любого поля выводит в лог имя теста и расхождение.
*/
void compareSingleCandidate(const QString& testName,
                             const CandidateError& actual,
                             const QString& expectedRuleId,
                             const QList<int>& expectedDisplayIds,
                             const QSet<int>& expectedConflictIds);

/*!
* \brief Сравнивает множество кандидатов со списком ожидаемых пар displayIds/conflictIds.
* \param [in] testName               Имя текущего теста.
* \param [in] actual                 Фактический набор кандидатов.
* \param [in] expectedRuleId         Ожидаемый ruleId (одинаковый для всех кандидатов).
* \param [in] expectedDisplayIdsList Список ожидаемых displayTokenIds.
* \param [in] expectedConflictIdsList Список ожидаемых conflictTokenIds.
*
* Каждый фактический кандидат должен совпасть ровно с одной ожидаемой парой
* (displayIds, conflictIds). При несовпадении выводит в лог имя теста
* и расхождение по каждому несовпавшему кандидату.
*/
void compareMultiCandidate(const QString& testName,
                            const QSet<CandidateError>& actual,
                            const QString& expectedRuleId,
                            const QList<QList<int>>& expectedDisplayIdsList,
                            const QList<QSet<int>>& expectedConflictIdsList);

#endif // AUXILIARYFUNCTIONSFORTESTING_H
