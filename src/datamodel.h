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
#include <QSet>
#include <QMetaType>
#include <optional>
#include <memory>

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
};

inline uint qHash(Upos key, uint seed = 0) noexcept
{
    return qHash(static_cast<int>(key), seed);
}

// ------------------------------------------------------------------------
// Типы для системы грамматической проверки (checkersystem)
// ------------------------------------------------------------------------

/*!
* \struct CandidateError
* \brief Один найденный кандидат ошибки от правила.
*/
struct CandidateError {
    QString ruleId;            ///< Идентификатор правила (например "ART-001")
    QString sentId;            ///< Идентификатор предложения
    QList<int> displayTokenIds; ///< ID токенов, отображаемых в сообщении
    QSet<int> conflictTokenIds; ///< ID токенов, определяющих зону конфликта

    bool operator==(const CandidateError& other) const {
        return ruleId == other.ruleId &&
               sentId == other.sentId &&
               displayTokenIds == other.displayTokenIds &&
               conflictTokenIds == other.conflictTokenIds;
    }
};

inline uint qHash(const CandidateError& ce, uint seed = 0) noexcept
{
    uint h = qHash(ce.ruleId, seed);
    for (int id : ce.displayTokenIds)
        h ^= qHash(id, seed) + 0x9e3779b9;
    for (int id : ce.conflictTokenIds)
        h ^= qHash(id, seed) + 0x9e3779b9;
    return h;
}

Q_DECLARE_METATYPE(CandidateError)

/*!
* \enum PriorityConditionKind
* \brief Условия применения приоритета между правилами.
*/
enum class PriorityConditionKind {
    Always,             ///< Приоритет применяется всегда
    Art003LanguageCase  ///< Зависит от структуры предложения (ENGLISH + language)
};

Q_DECLARE_METATYPE(PriorityConditionKind)

/*!
* \struct PriorityIndex
* \brief Карта условных приоритетов между правилами.
*
* conditionsByHigherRule[higher][lower] = условие.
*/
struct PriorityIndex {
    QHash<QString, QHash<QString, PriorityConditionKind>> conditionsByHigherRule;
};

Q_DECLARE_METATYPE(PriorityIndex)

/*!
* \struct DocumentModel
* \brief Модель всего документа.
*
* Хранит предложения через unique_ptr для гарантии отсутствия
* копирования и стабильности адресов TokenNode*.
*/
struct DocumentModel {
    std::vector<std::unique_ptr<SentenceModel>> sentences; ///< Список предложений
    QHash<QString, SentenceModel*> sentById; ///< Индекс по sentId
};

// ------------------------------------------------------------------------
// Типы для ресурсов правил (RuleResources)
// ------------------------------------------------------------------------

/*!
* \enum NumberCondition
* \brief Требуемое число существительного для записи det_compat.
*/
enum class NumberCondition {
    Any,    ///< Любое число
    Sing,   ///< Единственное
    Plur    ///< Множественное
};

/*!
* \enum CountabilityCondition
* \brief Требуемая исчисляемость существительного для записи det_compat.
*/
enum class CountabilityCondition {
    Any,          ///< Любая исчисляемость
    Uncountable,  ///< Неисчисляемое
    Countable     ///< Исчисляемое
};

/*!
* \enum VerbPrepAction
* \brief Действие при неверном предлоге для записи verb_prep.
*/
enum class VerbPrepAction {
    ReplacePrep,  ///< Заменить предлог
    DeletePrep,   ///< Удалить предлог
    InsertPrep    ///< Добавить предлог
};

/*!
* \struct PastForms
* \brief Неправильные формы одного глагола.
*/
struct PastForms {
    QString pastSimple;      ///< Форма Past Simple
    QString pastParticiple;  ///< Форма Past Participle
};

/*!
* \struct DetCompatEntry
* \brief Одна запись таблицы совместимости детерминатива с существительным.
*/
struct DetCompatEntry {
    NumberCondition number;               ///< Требуемое число
    CountabilityCondition countability;   ///< Требуемая исчисляемость
    QString correction;                   ///< Исправленная форма детерминатива
};

/*!
* \struct VerbPrepEntry
* \brief Одна запись таблицы глагольного управления.
*/
struct VerbPrepEntry {
    std::optional<QString> wrongPrep;     ///< Ошибочный предлог (nullopt для InsertPrep)
    VerbPrepAction action;                ///< Действие
    std::optional<QString> prep;          ///< Правильный предлог (nullopt для DeletePrep)

    bool operator==(const VerbPrepEntry& other) const {
        return wrongPrep == other.wrongPrep &&
               action == other.action &&
               prep == other.prep;
    }
};

inline uint qHash(const VerbPrepEntry& vpe, uint seed = 0) noexcept
{
    uint h = qHash(static_cast<int>(vpe.action), seed);
    if (vpe.wrongPrep) h ^= qHash(*vpe.wrongPrep, seed) + 0x9e3779b9;
    if (vpe.prep) h ^= qHash(*vpe.prep, seed) + 0x9e3779b9;
    return h;
}

/*!
* \struct RuleResources
* \brief Словари и таблицы, загружаемые один раз при инициализации.
*
* Простые списки слов хранятся в нижнем регистре для case-insensitive
* сравнения. Lookup выполняется через contains(form.toLower()).
*/
struct RuleResources {
    QSet<QString> geoThe;          ///< Географические названия, требующие "the" (geo_the.txt)
    QSet<QString> adjRequiresThe;  ///< ADJ, требующие "the" (adj_requires_the.txt)
    QSet<QString> languages;       ///< Названия языков (languages.txt)
    QSet<QString> sports;          ///< Виды спорта (sports.txt)
    QSet<QString> meals;           ///< Приёмы пищи (meals.txt)
    QSet<QString> titles;          ///< Титулы (titles.txt)
    QSet<QString> uncountable;     ///< Неисчисляемые существительные (uncountable.txt)
    QSet<QString> propnThe;        ///< PROPN, требующие "the" (propn_with_the.txt)
    QSet<QString> classifiers;     ///< Слова-классификаторы (classifiers.txt)
    QSet<QString> timeUnits;       ///< Единицы времени (time_units.txt)
    QSet<QString> activityVerbs;   ///< Глаголы активности (activity_verbs.txt)
    QSet<QString> positions;       ///< Названия должностей (positions.txt)
    QSet<QString> durations;       ///< Единицы длительности (durations.txt)
    QHash<QString, PastForms> pastForms;          ///< Неправильные глаголы (past_forms.txt)
    QHash<QString, QList<DetCompatEntry>> detCompat;  ///< Совместимость det+noun (det_compat.txt)
    QHash<QString, QSet<VerbPrepEntry>> verbPrep;     ///< Глагольное управление (verb_prep.txt)
    QHash<QString, QStringList> cmudict;              ///< Фонетический словарь CMUdict (cmudict.txt)
};

// Forward declarations
class Rule;

/*!
* \struct CheckerRuntime
* \brief Runtime-контекст с индексами и ресурсами.
*/
struct CheckerRuntime {
    QHash<Upos, QSet<const Rule*>> dispatch; ///< Диспетчеризация по UPOS
    PriorityIndex priorityIndex; ///< Приоритеты между правилами
    RuleResources resources; ///< Словари и таблицы для правил
};

/*!
* \class Rule
* \brief Базовый класс для всех правил грамматической проверки.
*/
class Rule {
public:
    virtual ~Rule() = default;

    virtual QString ruleId() const = 0;
    virtual QSet<Upos> anchorUpos() const = 0;
    virtual bool canConflict() const = 0;

    virtual QSet<CandidateError> check(const TokenNode& anchor,
                                       int sentenceIndex,
                                       const DocumentModel& document,
                                       const CheckerRuntime& runtime) const = 0;
};

/*!
* \struct ConflictZoneMap
* \brief Карта зон конфликтов: каждой группе conflictTokenIds
*        соответствует набор кандидатов, конкурирующих за одну ошибку.
*/
struct ConflictZoneMap {
    QHash<QSet<int>, QSet<CandidateError>> zones;
};
