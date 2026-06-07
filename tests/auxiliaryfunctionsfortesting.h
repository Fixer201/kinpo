/*!
* \file auxiliaryfunctionsfortesting.h
* \brief Хелперы для создания тестовых данных и сравнения сложных структур в тестах.
*
* Предоставляет функции для сборки строк CoNLL-U и сравнения результатов
* с детальным логированием (вместо стандартных QCOMPARE, которые не пишут
* что именно не совпало).
*/

#ifndef AUXILIARYFUNCTIONSFORTESTING_H
#define AUXILIARYFUNCTIONSFORTESTING_H

#include <QString>
#include <QStringList>
#include "datamodel.h"

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

#endif // AUXILIARYFUNCTIONSFORTESTING_H
