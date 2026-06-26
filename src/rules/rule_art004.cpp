/*!
* \file rule_art004.cpp
* \brief Реализация правила ART-004: Пропущен the перед географическим названием.
*
* Срабатывает на головном токене PROPN-группы, если:
*  — группа (LEMMA головного + зависимых с flat/flat:name/compound) присутствует
*    в geo_the.txt, ИЛИ рядом есть классификатор из classifiers.txt (fallback);
*  — у головного PROPN нет прямого зависимого с DEPREL=det.
*/

#include "rule_art004.h"
#include <QSet>
#include <QStringList>
#include <algorithm>

const Rule_ART004& Rule_ART004::instance()
{
    static Rule_ART004 inst;
    return inst;
}

QString Rule_ART004::ruleId() const
{
    return QStringLiteral("ART-004");
}

QSet<Upos> Rule_ART004::anchorUpos() const
{
    return {Upos::PROPN};
}

bool Rule_ART004::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Собрать LEMMA группы PROPN конкатенацией через пробел по возрастанию ID.
* \param [in] head Головной токен группы PROPN.
* \return Строка с конкатенированными формами головного токена и его зависимых
*         с DEPREL ∈ {flat, flat:name, compound}, нижний регистр.
*/
QString buildPropnGroupLemma(const TokenNode& head)
{
    QList<const TokenNode*> group;
    group.append(&head);
    for (const TokenNode* child : head.children) {
        if (child->deprel == Deprel::Flat ||
            child->deprel == Deprel::FlatName ||
            child->deprel == Deprel::Compound) {
            group.append(child);
        }
    }
    std::sort(group.begin(), group.end(),
              [](const TokenNode* a, const TokenNode* b) { return a->id < b->id; });
    QStringList forms;
    for (const TokenNode* node : group)
        forms.append(node->form);
    return forms.join(' ').toLower();
}

/*!
* \brief Собрать отсортированный список ID токенов группы PROPN.
* \param [in] head Головной токен группы.
* \return Список ID головного токена и его зависимых с flat/flat:name/compound,
*         отсортированный по возрастанию ID.
*/
QList<int> collectGroupIds(const TokenNode& head)
{
    QList<int> ids;
    ids.append(head.id);
    for (const TokenNode* child : head.children) {
        if (child->deprel == Deprel::Flat ||
            child->deprel == Deprel::FlatName ||
            child->deprel == Deprel::Compound) {
            ids.append(child->id);
        }
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

/*!
* \brief Проверить, является ли токен головным в группе PROPN.
* \param [in] anchor Проверяемый токен PROPN.
* \return true если anchor — головной (HEAD указывает на внешний узел).
*
* Головной токен группы — PROPN, у которого HEAD либо отсутствует (root),
* либо указывает на не-PROPN, либо связан с PROPN через DEPREL,
* не входящий в {flat, flat:name, compound}.
*/
bool isPropnGroupHead(const TokenNode& anchor)
{
    if (!anchor.parent)
        return true;
    if (anchor.parent->upos != Upos::PROPN)
        return true;
    if (anchor.deprel == Deprel::Flat ||
        anchor.deprel == Deprel::FlatName ||
        anchor.deprel == Deprel::Compound)
        return false;
    return true;
}

/*!
* \brief Проверить, есть ли у головного PROPN зависимый с DEPREL=det.
* \param [in] head Головной токен группы.
* \return true если найден зависимый с DEPREL=det.
*/
bool hasDetChild(const TokenNode& head)
{
    for (const TokenNode* child : head.children)
        if (child->deprel == Deprel::Det)
            return true;
    return false;
}

/*!
* \brief Проверить наличие классификатора рядом с головным PROPN.
* \param [in] head Головной токен группы PROPN.
* \param [in] classifiers Множество классификаторов (нижний регистр).
* \return true если среди прямых зависимых head (через compound/flat/appos)
*         или соседних токенов есть классификатор.
*
* Fallback по спецификации ART-004: проверяются прямые зависимые головного PROPN
* и соседние токены через DEPREL ∈ {compound, flat, appos} в обоих направлениях.
*/
bool hasClassifierNearby(const TokenNode& head, const QSet<QString>& classifiers)
{
    // Прямые зависимые с DEPREL ∈ {compound, flat, appos}
    for (const TokenNode* child : head.children) {
        if ((child->deprel == Deprel::Compound ||
             child->deprel == Deprel::Flat ||
             child->deprel == Deprel::Appos) &&
            classifiers.contains(child->form.toLower()))
            return true;
    }
    // Соседние токены
    if (head.previousToken && classifiers.contains(head.previousToken->form.toLower()))
        return true;
    if (head.nextToken && classifiers.contains(head.nextToken->form.toLower()))
        return true;
    return false;
}

} // namespace

QSet<CandidateError> Rule_ART004::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::PROPN)
        return res;

    if (!isPropnGroupHead(anchor))
        return res;

    // Если у головного PROPN есть артикль — ошибки нет
    if (hasDetChild(anchor))
        return res;

    // Проверка по geo_the.txt: LEMMA группы или одиночный LEMMA
    const QString groupLemma = buildPropnGroupLemma(anchor);
    const bool inGeoThe = runtime.resources.geoThe.contains(groupLemma) ||
                          runtime.resources.geoThe.contains(anchor.form.toLower());

    // Fallback: классификаторы рядом
    const bool hasClassifier = hasClassifierNearby(anchor, runtime.resources.classifiers);

    if (!inGeoThe && !hasClassifier)
        return res;

    // Ошибка: добавить «the»
    const QList<int> ids = collectGroupIds(anchor);

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-004");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = ids;
    ce.conflictTokenIds = QSet<int>(ids.begin(), ids.end());
    AtomicEdit edit;
    edit.type = AtomicEditType::InsertBefore;
    edit.referenceTokenId = anchor.id;
    edit.newTokens = {QStringLiteral("the")};
    ce.edits.append(edit);
    ce.description = QStringLiteral("Географическое название требует артикля «the».");
    res.insert(ce);
    return res;
}