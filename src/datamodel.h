/*!
* \file datamodel.h
* \brief Определения базовых структур данных CoNLL-U парсера.
*
* Содержит перечисление видов диагностических ошибок и структуры Diagnostic,
* RawToken, MwtRecord, RawSentence, используемые на всех этапах обработки.
*/

#pragma once

#include <QString>
#include <QList>
#include <QHash>
#include <optional>

/*!
* \brief Перечисление категорий ошибок, прерывающих конвейер.
*
* Используется в Diagnostic для классификации проблемы на этапах
* парсинга, валидации, загрузки ресурсов и т.д.
*/
enum class DiagnosticKind {
    CliUsageError,      ///< Неверные аргументы командной строки
    ResourceLoadError,  ///< Ошибка загрузки словарей или таблиц
    InputFileError,     ///< Проблема доступа к входному файлу
    InputFormatError,   ///< Нарушение формата CoNLL-U или структурных ограничений
    OutputWriteError,   ///< Ошибка записи выходного файла
    InternalError       ///< Внутренняя ошибка программы
};

/*!
* \brief Преобразует DiagnosticKind в строковое представление.
* \param [in] kind Значение перечисления.
* \return Локализованное строковое имя (например, "InputFormatError").
*/
QString diagnosticKindToString(DiagnosticKind kind);

/*!
* \struct Diagnostic
* \brief Единообразное описание ошибки, прерывающей конвейер.
*
* Хранит категорию (kind), номер строки входного файла, идентификатор
* предложения, человекочитаемое сообщение и внутренний код ошибки.
*/
struct Diagnostic {
    DiagnosticKind kind;            ///< Категория ошибки
    std::optional<int> lineNumber;   ///< Номер строки во входном файле, если применимо
    std::optional<QString> sentId;   ///< Идентификатор предложения, если применимо
    QString message;                ///< Готовое сообщение для stderr
    int code;                       ///< Внутренний код ошибки
};

/*!
* \struct RawToken
* \brief Представление одного токена CoNLL-U (строка 10 колонок).
*
* Поля соответствуют стандартной спецификации CoNLL-U:
* ID, FORM, LEMMA, UPOS, XPOS, FEATS, HEAD, DEPREL, DEPS, MISC.
* Все значения хранятся как QString без лингвистической интерпретации.
*/
struct RawToken {
    int lineNumber;   ///< Номер строки во входном файле
    int id;           ///< Целочисленный ID токена (1..N)
    QString form;     ///< Словоформа (FORM)
    QString lemma;    ///< Лемма (LEMMA)
    QString upos;     ///< Универсальная часть речи (UPOS)
    QString xpos;     ///< Языковоспецифичный тег (XPOS)
    QString featsRaw; ///< Сырые признаки (FEATS)
    int headId;       ///< ID головного токена (HEAD)
    QString deprel;   ///< Синтаксическое отношение (DEPREL)
    QString depsRaw;  ///< Расширенные зависимости (DEPS)
    QString miscRaw;  ///< Прочие аннотации (MISC)
};

/*!
* \struct MwtRecord
* \brief Запись Multi-Word Token (ID вида N-M в CoNLL-U).
*
* MWT-строка содержит исходную форму слова перед разбиением,
* например, "don't" → do + n't. Сама запись не имеет UPOS/DEPREL
* и не участвует в дереве зависимостей.
*/
struct MwtRecord {
    int lineNumber;  ///< Номер строки во входном файле
    int rangeStart;  ///< Начальный ID диапазона (N)
    int rangeEnd;    ///< Конечный ID диапазона (M)
    QString form;    ///< Исходная форма (FORM)
};

/*!
* \struct RawSentence
* \brief Внутреннее представление одного предложения до преобразования в модель.
*
* Содержит метаданные (sentId, text) и список токенов с MWT-записями.
* Используется на этапах парсинга и структурной валидации.
*/
struct RawSentence {
    int firstLineNumber;          ///< Номер первой строки блока
    QString sentId;               ///< Идентификатор предложения
    QString text;                 ///< Исходный текст предложения
    QList<RawToken> tokens;       ///< Обычные токены с целочисленным ID
    QList<MwtRecord> mwtRecords;  ///< MWT-записи (ID вида N-M)
};
