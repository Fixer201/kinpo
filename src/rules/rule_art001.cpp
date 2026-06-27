/*!
* \file rule_art001.cpp
* \brief Реализация правила ART-001: Лишний артикль перед PROPN.
*
* Срабатывает на DET с LEMMA {a, an, the}, DEPREL=det к токену N с UPOS=PROPN.
* Исключения для the: географические названия (geo_the.txt), титулы (positions.txt),
* фамилии во мн.ч., классификаторы (classifiers.txt), PROPN из propn_with_the.txt.
* Для a/an: применяется только исключение geo_the.
*/

#include "rule_art001.h"
#include <QSet>
#include <QStringList>
#include <algorithm>
#include <QDebug>

const Rule_ART001& Rule_ART001::instance()
{
    static Rule_ART001 inst;
    return inst;
}

QString Rule_ART001::ruleId() const
{
    return QStringLiteral("ART-001");
}

QSet<Upos> Rule_ART001::anchorUpos() const
{
    return {Upos::Det};
}

bool Rule_ART001::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Собрать LEMMA группы PROPN конкатенацией через пробел по возрастанию ID.
* \param [in] head Головной токен группы PROPN.
* \return Строка с конкатенированными формами зависимых с DEPREL ∈ {flat, flat:name, compound}
*         и формы самого головного токена, отсортированная по ID.
*
* Согласно спецификации Б.13, lookup в geo_the.txt выполняется по LEMMA группы PROPN.
* Группа включает головной PROPN и его зависимых с DEPREL flat, flat:name, compound.
*/
QString buildPropnGroupLemma(const TokenNode& head)
{
    // Собираем все токены группы: головной + зависимые с flat/flat:name/compound
    QList<const TokenNode*> group;
    group.append(&head);
    for (const TokenNode* child : head.children) {
        if (child->deprel == Deprel::Flat ||
            child->deprel == Deprel::FlatName ||
            child->deprel == Deprel::Compound) {
            group.append(child);
        }
    }

    // Сортируем по ID для детерминированного порядка конкатенации
    std::sort(group.begin(), group.end(),
              [](const TokenNode* a, const TokenNode* b) { return a->id < b->id; });

    // Конкатенируем формы через пробел
    QStringList forms;
    for (const TokenNode* node : group)
        forms.append(node->form);
    return forms.join(' ').toLower();
}

/*!
* \brief Проверить, есть ли среди прямых зависимых N или соседних токенов
*        токен с LEMMA из classifiers.txt.
* \param [in] head Головной токен группы PROPN.
* \param [in] classifiers Множество классификаторов (нижний регистр).
* \return true если классификатор найден.
*
* Согласно спецификации ART-001 исключение 6: проверяются прямые зависимые N
* и соседние токены в группе PROPN.
*/
bool hasClassifierInGroup(const TokenNode& head, const QSet<QString>& classifiers)
{
    // Проверяем прямых зависимых головного PROPN
    bool found = false;
    for (const TokenNode* child : head.children) {
        if (!found && classifiers.contains(child->form.toLower()))
            found = true;
    }

    // Проверяем соседние токены (previousToken, nextToken)
    if (!found && head.previousToken && classifiers.contains(head.previousToken->form.toLower()))
        found = true;
    if (!found && head.nextToken && classifiers.contains(head.nextToken->form.toLower()))
        found = true;

    return found;
}

/*!
* \brief Проверить, является ли группа PROPN составным с America.
* \param [in] head Головной токен группы PROPN.
* \return true если головной токен имеет lemma/form «america» и среди
*         зависимых с DEPREL=Compound есть lemma ∈ {north, south, central, latin}.
*
* Уточнение внешн. спецификации ART-001: составные с America
* не требуют артикля, поэтому исключения 5–7 к ним не применяются.
* Вызывающий код использует флаг для пропуска исключений 5–7,
* но сама ошибка ART-001 срабатывает.
*/
bool isAmericaCompound(const TokenNode& head)
{
    // Берём lemma головного PROPN. Если lemma пуста — используем form.
    // Оба значения приводятся к нижнему регистру для сравнения без учёта регистра.
    const QString headLower = head.lemma.isEmpty()
        ? head.form.toLower() : head.lemma.toLower();

    // Головной токен группы PROPN должен иметь lemma «america».
    // Без этого условия группа не является America-составным.
    if (headLower != QStringLiteral("america"))
        return false;

    // Составной модификатор направления: North, South, Central или Latin.
    // Перебираем прямых зависимых головного PROPN.
    // Интересуют только токены с DEPREL=Compound именно через эту связь
    // в UD-дереве оформляются составные географические названия.
    for (const TokenNode* child : head.children) {
        if (child->deprel == Deprel::Compound) {
            // Приводим lemma/form зависимого к нижнему регистру.
            const QString childLower = child->lemma.isEmpty()
                ? child->form.toLower() : child->lemma.toLower();

            // Если зависимый — одно из четырёх направлений,
            // группа PROPN является America-составным.
            if (childLower == QStringLiteral("north") ||
                childLower == QStringLiteral("south") ||
                childLower == QStringLiteral("central") ||
                childLower == QStringLiteral("latin"))
                return true;
        }
    }

    // Ни один Compound-зависимый не является модификатором направления.
    // Группа PROPN — не America-составное.
    return false;
}

/*!
* \brief Проверить, является ли форма одной из особых (Reverend, Honorable).
* \param [in] form Форма токена (приводится к нижнему регистру).
* \return true если форма — reverend или honorable.
*/
bool isSpecialTitle(const QString& form)
{
    const QString lower = form.toLower();
    return lower == QStringLiteral("reverend") || lower == QStringLiteral("honorable");
}

} // namespace

QSet<CandidateError> Rule_ART001::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Условие срабатывания: DET с form ∈ {a, an, the}, DEPREL=det к PROPN
    if (anchor.upos != Upos::Det)
        return res;

    const QString formLower = anchor.form.toLower();
    if (formLower != QStringLiteral("a") &&
        formLower != QStringLiteral("an") &&
        formLower != QStringLiteral("the"))
        return res;

    if (!anchor.parent)
        return res;
    if (anchor.parent->upos != Upos::Prop)
        return res;

    const TokenNode& propn = *anchor.parent;
    const QString propnFormLower = propn.form.toLower();
    const bool isThe = (formLower == QStringLiteral("the"));

    // Исключение 1: N входит в группу PROPN из geo_the.txt (применяется для a/an/the)
    const QString groupLemma = buildPropnGroupLemma(propn);
    if (runtime.resources.geoThe.contains(groupLemma))
        return res;
    // Также проверяем форму одиночного PROPN (для случая без группы)
    if (runtime.resources.geoThe.contains(propnFormLower))
        return res;

    // Для a/an: только исключение 1 (geo_the), остальные исключения не применяются
    if (!isThe)
        return produceError(anchor, res);

    // Для the: проверяем исключения 2–7

    // Исключение 3: D.LEMMA=the и N.LEMMA из positions.txt
    if (runtime.resources.positions.contains(propnFormLower))
        return res;

    // Исключение 4: D.LEMMA=the и N.LEMMA ∈ {Reverend, Honorable}
    if (isSpecialTitle(propn.form))
        return res;

    // Составные с America не требуют артикля — исключения 5–7
    // к ним не применяются. Флаг isAmerica используется ниже
    // для пропуска этих исключений.
    const bool isAmerica = isAmericaCompound(propn);

    // PROPN-язык в позиции подлежащего = народ/нация.
    // «The English are polite» — the здесь корректен.
    // Зеркалирует исключение ART-003 для nsubj (строка 129 rule_art003.cpp).
    if (runtime.resources.languages.contains(propn.lemma.toLower()) &&
        propn.deprel == Deprel::Nsubj)
        return res;

    // Исключение 5: N.Number=Plur и D.LEMMA=the
    if (!isAmerica && propn.features.number == NumberValue::Plur)
        return res;

    // Исключение 6: классификатор в группе PROPN
    if (!isAmerica && hasClassifierInGroup(propn, runtime.resources.classifiers))
        return res;

    // Исключение 7: N.LEMMA из propn_with_the.txt
    if (!isAmerica && runtime.resources.propnThe.contains(propnFormLower))
        return res;

    // Ошибка подтверждена
    return produceError(anchor, res);
}

QSet<CandidateError> Rule_ART001::produceError(const TokenNode& anchor, QSet<CandidateError> res) const
{
    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id, anchor.parent->id};
    ce.conflictTokenIds = {anchor.id};
    AtomicEdit edit;
    edit.type = AtomicEditType::DeleteTokens;
    edit.targetTokenIds = {anchor.id};
    ce.edits.append(edit);
    ce.description = QStringLiteral("Артикль не используется перед именем собственным.");
    res.insert(ce);
    return res;
}