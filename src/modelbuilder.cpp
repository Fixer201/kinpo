/*!
* \file modelbuilder.cpp
* \brief Реализация модуля построения аналитической модели.
*
* Содержит buildSentenceModel и вспомогательные функции parseUpos,
* parseDeprel, parseFeats для преобразования строковых полей CoNLL-U
* в типизированные enum-значения.
*/

#include "modelbuilder.h"

#include <QHash>
#include <QStringList>
#include <vector>

// ========================================================================
// Преобразование строковых полей CoNLL-U в типизированные enum
// ========================================================================

/*!
* \brief Преобразует строковый UPOS-тег в enum.
* \param [in] upos Строка из входного файла (например, "NOUN").
* \return Соответствующее значение Upos; Upos::X для неизвестного тега.
*
* Использует статическую хеш-таблицу для O(1) поиска. Вызывается в
* buildSentenceModel при построении TokenNode из RawToken.
*/
Upos parseUpos(const QString& upos)
{
    static const QHash<QString, Upos> map = {
        {QStringLiteral("ADJ"),   Upos::ADJ},
        {QStringLiteral("ADP"),   Upos::ADP},
        {QStringLiteral("ADV"),   Upos::ADV},
        {QStringLiteral("AUX"),   Upos::AUX},
        {QStringLiteral("CCONJ"), Upos::CCONJ},
        {QStringLiteral("DET"),   Upos::DET},
        {QStringLiteral("INTJ"),  Upos::INTJ},
        {QStringLiteral("NOUN"),  Upos::NOUN},
        {QStringLiteral("NUM"),   Upos::NUM},
        {QStringLiteral("PART"),  Upos::PART},
        {QStringLiteral("PRON"),  Upos::PRON},
        {QStringLiteral("PROPN"), Upos::PROPN},
        {QStringLiteral("PUNCT"), Upos::PUNCT},
        {QStringLiteral("SCONJ"), Upos::SCONJ},
        {QStringLiteral("SYM"),   Upos::SYM},
        {QStringLiteral("VERB"),  Upos::VERB},
        {QStringLiteral("X"),     Upos::X},
    };
    return map.value(upos, Upos::X);
}

/*!
* \brief Преобразует строковый DEPREL-тег в enum.
* \param [in] deprel Строка из входного файла (например, "nsubj").
* \return Соответствующее значение Deprel; Deprel::Other для тегов,
*         не используемых правилами.
*
* Использует статическую хеш-таблицу для O(1) поиска. Подтипы (например,
* nsubj:pass) представлены отдельными значениями, а не приравниваются
* к базовому типу. Вызывается в buildSentenceModel при построении
* TokenNode из RawToken.
*/
Deprel parseDeprel(const QString& deprel)
{
    static const QHash<QString, Deprel> map = {
        {QStringLiteral("det"),         Deprel::Det},
        {QStringLiteral("amod"),        Deprel::Amod},
        {QStringLiteral("nsubj"),       Deprel::Nsubj},
        {QStringLiteral("nsubj:pass"),  Deprel::NsubjPass},
        {QStringLiteral("obj"),         Deprel::Obj},
        {QStringLiteral("obl"),         Deprel::Obl},
        {QStringLiteral("case"),        Deprel::Case},
        {QStringLiteral("nmod"),        Deprel::Nmod},
        {QStringLiteral("nmod:poss"),   Deprel::NmodPoss},
        {QStringLiteral("nmod:desc"),   Deprel::NmodDesc},
        {QStringLiteral("flat"),        Deprel::Flat},
        {QStringLiteral("flat:name"),   Deprel::FlatName},
        {QStringLiteral("compound"),    Deprel::Compound},
        {QStringLiteral("fixed"),       Deprel::Fixed},
        {QStringLiteral("root"),        Deprel::Root},
        {QStringLiteral("aux"),         Deprel::Aux},
        {QStringLiteral("aux:pass"),    Deprel::AuxPass},
        {QStringLiteral("cop"),         Deprel::Cop},
        {QStringLiteral("mark"),        Deprel::Mark},
        {QStringLiteral("punct"),       Deprel::Punct},
        {QStringLiteral("vocative"),    Deprel::Vocative},
        {QStringLiteral("advmod"),      Deprel::Advmod},
        {QStringLiteral("conj"),        Deprel::Conj},
        {QStringLiteral("cc"),          Deprel::Cc},
        {QStringLiteral("cc:preconj"),  Deprel::CcPreconj},
        {QStringLiteral("expl"),        Deprel::Expl},
        {QStringLiteral("xcomp"),       Deprel::Xcomp},
        {QStringLiteral("appos"),       Deprel::Appos},
        {QStringLiteral("advcl"),       Deprel::Advcl},
    };
    return map.value(deprel, Deprel::Other);
}

// ========================================================================
// Вспомогательные функции для buildSentenceModel
// ========================================================================

namespace {

/*!
* \brief Парсит строку FEATS в типизированную структуру TokenFeatures.
* \param [in] featsRaw Строка FEATS из CoNLL-U (например, "Number=Sing|Person=3").
* \return TokenFeatures с заполненными полями.
*
 * Поддерживаемые признаки: Number, Tense, VerbForm, Degree, Case, Person,
 * Poss=Yes, Polarity=Neg, NumType=Ord, Voice=Pass.
 * Если featsRaw == "_" или пустая — возвращает структуру по умолчанию.
*/
TokenFeatures parseFeats(const QString& featsRaw)
{
    TokenFeatures f;
    if (featsRaw == QStringLiteral("_") || featsRaw.isEmpty())
        return f;

    QStringList pairs = featsRaw.split('|');
    for (const QString& pair : pairs) {
        QStringList kv = pair.split('=');
        if (kv.size() != 2)
            continue;
        QString key = kv[0].trimmed();
        QString value = kv[1].trimmed();

        if (key == QStringLiteral("Number")) {
            if (value == QStringLiteral("Sing"))      f.number = NumberValue::Sing;
            else if (value == QStringLiteral("Plur")) f.number = NumberValue::Plur;
        } else if (key == QStringLiteral("Tense")) {
            if (value == QStringLiteral("Past"))      f.tense = TenseValue::Past;
            else if (value == QStringLiteral("Pres"))  f.tense = TenseValue::Pres;
        } else if (key == QStringLiteral("VerbForm")) {
            if (value == QStringLiteral("Inf"))       f.verbForm = VerbFormValue::Inf;
            else if (value == QStringLiteral("Part"))  f.verbForm = VerbFormValue::Part;
        } else if (key == QStringLiteral("Degree")) {
            if (value == QStringLiteral("Pos"))       f.degree = DegreeValue::Pos;
            else if (value == QStringLiteral("Cmp"))   f.degree = DegreeValue::Cmp;
            else if (value == QStringLiteral("Sup"))   f.degree = DegreeValue::Sup;
        } else if (key == QStringLiteral("Case")) {
            if (value == QStringLiteral("Nom"))       f.caseValue = CaseValue::Nom;
            else if (value == QStringLiteral("Acc"))   f.caseValue = CaseValue::Acc;
        } else if (key == QStringLiteral("Person")) {
            f.person = value;
        } else if (key == QStringLiteral("Poss") && value == QStringLiteral("Yes")) {
            f.poss = true;
        } else if (key == QStringLiteral("Polarity") && value == QStringLiteral("Neg")) {
            f.polarityNeg = true;
        } else if (key == QStringLiteral("NumType") && value == QStringLiteral("Ord")) {
            f.numTypeOrd = true;
        } else if (key == QStringLiteral("Voice") && value == QStringLiteral("Pass")) {
            f.voicePass = true;
        }
    }
    return f;
}

} // namespace

// ========================================================================
// Построение аналитической модели
// ========================================================================

/*!
* \brief Построить SentenceModel из валидированного RawSentence.
* \param [in] rawSentence Сырое предложение после validateSentenceStructure.
* \return Аналитическая модель предложения с деревом зависимостей,
*         линейными связями, типизированными UPOS/DEPREL и парсенными FEATS.
*
* Алгоритм (см. внутреннюю спецификацию, раздел 4.2.1):
*  1. Создаёт TokenNode из каждого RawToken (конвертация UPOS, DEPREL, FEATS).
*  2. Заполняет линейный список tokens и хеш tokensById.
*  3. Связывает previousToken / nextToken.
*  4. Строит дерево parent / children по headId, находит rootToken.
*  5. Помечает isMwtFragment для токенов в диапазонах MWT.
*/
SentenceModel buildSentenceModel(const RawSentence& rawSentence)
{
    SentenceModel model;
    model.sentId = rawSentence.sentId;
    model.text = rawSentence.text;
    model.mwtRecords = rawSentence.mwtRecords;

    // Резервируем память, чтобы адреса nodeStorage не менялись
    model.nodeStorage.reserve(rawSentence.tokens.size());

    // Шаг 1: создаём TokenNode из RawToken
    for (const auto& rt : rawSentence.tokens) {
        TokenNode node;
        node.lineNumber = rt.lineNumber;
        node.id = rt.id;
        node.headId = rt.headId;
        node.form = rt.form;
        node.lemma = rt.lemma;
        node.xpos = rt.xpos;
        node.featsRaw = rt.featsRaw;
        node.depsRaw = rt.depsRaw;
        node.miscRaw = rt.miscRaw;
        node.upos = parseUpos(rt.upos);
        node.deprelRaw = rt.deprel;
        node.deprel = parseDeprel(rt.deprel);
        node.features = parseFeats(rt.featsRaw);

        model.nodeStorage.push_back(std::move(node));
    }

    // Шаг 2: заполняем tokens (линейный порядок) и tokensById
    for (auto& node : model.nodeStorage) {
        model.tokens.append(&node);
        model.tokensById[node.id] = &node;
    }

    // Шаг 3: линейные связи previousToken / nextToken
    for (int i = 0; i < model.tokens.size(); ++i) {
        if (i > 0)
            model.tokens[i]->previousToken = model.tokens[i - 1];
        if (i + 1 < model.tokens.size())
            model.tokens[i]->nextToken = model.tokens[i + 1];
    }

    // Шаг 4: дерево зависимостей parent / children, поиск корня
    for (auto& node : model.nodeStorage) {
        if (node.headId == 0) {
            node.parent = nullptr;
            model.rootToken = &node;
        } else {
            TokenNode* parent = model.tokensById.value(node.headId, nullptr);
            if (parent) {
                node.parent = parent;
                parent->children.append(&node);
            }
        }
    }

    // Шаг 5: помечаем MWT-фрагменты
    for (const auto& mwt : model.mwtRecords) {
        for (int id = mwt.rangeStart; id <= mwt.rangeEnd; ++id) {
            TokenNode* node = model.tokensById.value(id, nullptr);
            if (node)
                node->isMwtFragment = true;
        }
    }

    return model;
}