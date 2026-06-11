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
#include <QMetaType>
#include <optional>

/*!
* \brief Типизированное представление синтаксических отношений (DEPREL).
*
* Соответствует спецификации CoNLL-U и расширениям UD English EWT.
* Подтипы (например, nsubj:pass) представлены отдельными значениями,
* а не приравниваются к базовому типу. Исходная строка сохраняется
* в TokenNode::deprelRaw.
*/
enum class Deprel {
    Det,        ///< Определитель (det)
    Amod,       ///< Атрибутивное прилагательное (amod)
    Nsubj,      ///< Подлежащее (nsubj)
    NsubjPass,  ///< Подлежащее пассива (nsubj:pass)
    Obj,        ///< Прямое дополнение (obj)
    Obl,        ///< Косвенное дополнение (obl)
    Case,       ///< Предложный падеж (case)
    Nmod,       ///< Именное модификатор (nmod)
    NmodPoss,   ///< Притяжательный модификатор (nmod:poss)
    NmodDesc,   ///< Описательный модификатор — титул + имя (nmod:desc)
    Flat,       ///< Плоский составной (flat)
    FlatName,   ///< Плоский составной — имя (flat:name)
    Compound,   ///< Составное (compound)
    Fixed,      ///< Фиксированное выражение (fixed)
    Root,       ///< Корневое отношение (root)
    Aux,        ///< Вспомогательный глагол (aux)
    AuxPass,    ///< Вспомогательный глагол пассива (aux:pass)
    Cop,        ///< Связка (cop)
    Mark,       ///< Маркер подчинённой клаузы (mark)
    Punct,      ///< Пунктуация (punct)
    Vocative,   ///< Обращение (vocative)
    Advmod,     ///< Наречие-модификатор (advmod)
    Conj,       ///< Сочинительный элемент (conj)
    Cc,         ///< Союз (cc)
    CcPreconj,  ///< Предсоюз (cc:preconj)
    Expl,       ///< Эксплетив (expl)
    Xcomp,      ///< Открытый клаузальный комплемент (xcomp)
    Appos,      ///< Аппозиция (appos)
    Advcl,      ///< Наречная клауза (advcl)
    Other       ///< Любое значение, не используемое правилами
};
Q_DECLARE_METATYPE(Deprel)

/*!
* \brief Типизированное представление универсальных частей речи (UPOS).
*
* Соответствует спецификации CoNLL-U: каждому строковому тегу UPOS
* сопоставлено значение перечисления. Преобразование выполняется
* в buildSentenceModel при построении TokenNode из RawToken.
*/
enum class Upos {
    ADJ,      ///< Прилагательное (Adjective)
    ADP,      ///< Адпозиция (Adposition)
    ADV,      ///< Наречие (Adverb)
    AUX,      ///< Вспомогательный глагол (Auxiliary)
    CCONJ,    ///< Сочинительный союз (Coordinating conjunction)
    DET,      ///< Детерминатив (Determiner)
    INTJ,     ///< Междометие (Interjection)
    NOUN,     ///< Существительное (Noun)
    NUM,      ///< Числительное (Numeral)
    PART,     ///< Частица (Particle)
    PRON,     ///< Местоимение (Pronoun)
    PROPN,    ///< Имя собственное (Proper noun)
    PUNCT,    ///< Пунктуация (Punctuation)
    SCONJ,    ///< Подчинительный союз (Subordinating conjunction)
    SYM,      ///< Символ (Symbol)
    VERB,     ///< Глагол (Verb)
    X         ///< Прочее (Other)
};
Q_DECLARE_METATYPE(Upos)

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

Q_DECLARE_METATYPE(RawSentence)

// ------------------------------------------------------------------------
// Типы для аналитической модели (buildSentenceModel)
// ------------------------------------------------------------------------

/*!
* \brief Значение числа (Number) в FEATS.
*/
enum class NumberValue { Sing, Plur };

/*!
* \brief Значение времени (Tense) в FEATS.
*/
enum class TenseValue { Past, Pres };

/*!
* \brief Значение формы глагола (VerbForm) в FEATS.
*/
enum class VerbFormValue { Inf, Part };

/*!
* \brief Значение степени сравнения (Degree) в FEATS.
*/
enum class DegreeValue { Pos, Cmp, Sup };

/*!
* \brief Значение падежа (Case) в FEATS.
*/
enum class CaseValue { Nom, Acc };

/*!
* \brief Типизированное представление FEATS, используемых правилами.
*
* Парсится из строки FEATS в buildSentenceModel. Неиспользуемые
* признаки (например, Person) не хранятся.
*/
struct TokenFeatures {
    std::optional<NumberValue> number;      ///< Number=Sing/Plur
    std::optional<TenseValue> tense;        ///< Tense=Past/Pres
    std::optional<VerbFormValue> verbForm;  ///< VerbForm=Inf/Part
    std::optional<DegreeValue> degree;      ///< Degree=Pos/Cmp/Sup
    std::optional<CaseValue> caseValue;     ///< Case=Nom/Acc
    bool poss = false;                      ///< Poss=Yes
    bool polarityNeg = false;               ///< Polarity=Neg
    bool numTypeOrd = false;                ///< NumType=Ord
    bool voicePass = false;                 ///< Voice=Pass
};

/*!
* \brief Один токен в аналитической модели.
*
* Содержит типизированные поля (Upos, Deprel, TokenFeatures),
* связи дерева зависимостей (parent/children) и линейные связи.
*/
struct TokenNode {
    int lineNumber = 0;                ///< Номер строки во входном файле
    int id = 0;                        ///< Целочисленный ID токена
    int headId = 0;                    ///< ID родителя (0 для корня)
    QString form;                      ///< FORM
    QString lemma;                     ///< LEMMA
    Upos upos = Upos::X;               ///< Типизированный UPOS
    QString xpos;                      ///< XPOS
    QString deprelRaw;                 ///< DEPREL как строка
    Deprel deprel = Deprel::Other;     ///< Типизированный DEPREL
    QString featsRaw;                  ///< FEATS строка
    TokenFeatures features;            ///< Типизированные FEATS
    QString depsRaw;                   ///< DEPS
    QString miscRaw;                   ///< MISC

    TokenNode* parent = nullptr;              ///< Родительский токен
    QList<TokenNode*> children;                 ///< Прямые зависимые
    TokenNode* previousToken = nullptr;         ///< Предыдущий токен
    TokenNode* nextToken = nullptr;             ///< Следующий токен
    bool isMwtFragment = false;               ///< Входит в MWT-диапазон

    /*! \brief Следующий токен, пропуская PUNCT и MWT. */
    const TokenNode* nextNonPunct() const;
    /*! \brief Предыдущий токен, пропуская PUNCT и MWT. */
    const TokenNode* previousNonPunct() const;
};

/*!
* \brief Внутреннее представление одного предложения.
*
* Хранит TokenNode в nodeStorage (по значению); tokens и tokensById
* — адреса тех же объектов. Копирование запрещено.
*/
struct SentenceModel {
    std::vector<TokenNode> nodeStorage;    ///< Хранилище токенов по значению
    QString sentId;                        ///< Идентификатор предложения
    QString text;                          ///< Исходный текст
    QList<TokenNode*> tokens;              ///< Линейный порядок
    QHash<int, TokenNode*> tokensById;     ///< Доступ по ID
    TokenNode* rootToken = nullptr;        ///< Корневой токен
    QList<MwtRecord> mwtRecords;           ///< MWT-записи

    SentenceModel() = default;
    SentenceModel(const SentenceModel&) = delete;
    SentenceModel& operator=(const SentenceModel&) = delete;
    SentenceModel(SentenceModel&&) = default;
    SentenceModel& operator=(SentenceModel&&) = default;
};
