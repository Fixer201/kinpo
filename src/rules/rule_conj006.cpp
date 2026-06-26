/*!
* \file rule_conj006.cpp
* \brief Реализация правила CONJ-006: избыточный координатор.
*
* Якорь — токен C с LEMMA ∈ {so, therefore, but}. V2 — голова C (прямой
* родитель). Среди прямых зависимых V2 ищется V1 с DEPREL=advcl. Среди
* прямых зависимых V1 ищется S с DEPREL=mark, UPOS=SCONJ. Для so/therefore
* лемма S должна быть в множестве {because, since, as}. Для but — в
* множестве {although, though}. При выполнении условий координатор C
* избыточен и удаляется.
*/

#include "rule_conj006.h"
#include <QSet>

const Rule_CONJ006& Rule_CONJ006::instance()
{
    static Rule_CONJ006 inst;
    return inst;
}

QString Rule_CONJ006::ruleId() const
{
    return QStringLiteral("CONJ-006");
}

QSet<Upos> Rule_CONJ006::anchorUpos() const
{
    // so/therefore обычно ADV, but обычно CCONJ
    return {Upos::CCONJ, Upos::ADV};
}

bool Rule_CONJ006::canConflict() const
{
    return true;
}

namespace {

// Леммы подчинительных союзов для пары because/since/as → so/therefore
const QSet<QString> causativeSubordinators = {
    QStringLiteral("because"), QStringLiteral("since"), QStringLiteral("as")
};

// Леммы подчинительных союзов для пары although/though → but
const QSet<QString> concessiveSubordinators = {
    QStringLiteral("although"), QStringLiteral("though")
};

// Леммы координаторов, проверяемых правилом
const QSet<QString> coordinatorLemmas = {
    QStringLiteral("so"), QStringLiteral("therefore"), QStringLiteral("but")
};

/*!
* \brief Найти подчинительный союз S среди прямых зависимых V1.
* \param [in] v1 Токен V1 (advcl-зависимый V2).
* \param [in] allowedLemmas Множество допустимых лемм S.
* \return Указатель на S или nullptr, если не найден.
*
* S должен иметь DEPREL=mark, UPOS=SCONJ и лемму из allowedLemmas.
*/
const TokenNode* findSubordinator(const TokenNode& v1,
                                  const QSet<QString>& allowedLemmas)
{
    for (const TokenNode* child : v1.children) {
        if (child->deprel != Deprel::Mark)
            continue;
        if (child->upos != Upos::SCONJ)
            continue;
        if (allowedLemmas.contains(child->lemma.toLower()))
            return child;
    }
    return nullptr;
}

/*!
* \brief Найти V1 (advcl-зависимый V2) с подходящим подчинительным союзом.
* \param [in] v2 Токен V2 (голова координатора C).
* \param [in] coordinatorLemma Лемма координатора (so/therefore/but).
* \return Указатель на V1 или nullptr, если не найден.
*
* Перебирает прямых зависимых V2 с DEPREL=advcl. Для каждого проверяет,
* есть ли среди его зависимых подчинительный союз, соответствующий
* координатору: so/therefore → because/since/as, but → although/though.
*/
const TokenNode* findAdvclWithSubordinator(const TokenNode& v2,
                                           const QString& coordinatorLemma)
{
    // Выбираем множество лемм S в зависимости от координатора
    const QSet<QString>& allowedLemmas =
        (coordinatorLemma == QStringLiteral("but"))
            ? concessiveSubordinators
            : causativeSubordinators;

    for (const TokenNode* child : v2.children) {
        if (child->deprel != Deprel::Advcl)
            continue;
        if (findSubordinator(*child, allowedLemmas) != nullptr)
            return child;
    }
    return nullptr;
}

} // namespace

QSet<CandidateError> Rule_CONJ006::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь должен быть координатором из проверяемого множества
    const QString lemma = anchor.lemma.toLower();
    if (!coordinatorLemmas.contains(lemma))
        return res;

    // V2 — голова координатора, строго прямой родитель
    const TokenNode* v2 = anchor.parent;
    if (v2 == nullptr)
        return res;

    // Ищем V1 (advcl-зависимый V2) с подходящим подчинительным союзом
    const TokenNode* v1 = findAdvclWithSubordinator(*v2, lemma);
    if (v1 == nullptr)
        return res;

    // Все условия выполнены: координатор избыточен, удаляем
    CandidateError ce;
    ce.ruleId = QStringLiteral("CONJ-006");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::DeleteTokens;
        edit.targetTokenIds = {anchor.id};
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("Избыточный координатор «%1» при подчинительном союзе.").arg(anchor.form);
    res.insert(ce);
    return res;
}