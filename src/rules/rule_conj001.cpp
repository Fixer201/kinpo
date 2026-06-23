/*!
* \file rule_conj001.cpp
* \brief Реализация правила CONJ-001: nor без отрицания.
*
* Срабатывает на CCONJ с LEMMA=nor, DEPREL=cc. HEAD(nor)=C2, C2.DEPREL
* должен быть conj (иначе не проверять — sentence-initial, discourse).
* HEAD(C2)=C1. У C1 не должно быть зависимого cc:preconj с LEMMA=neither
* (коррелят). В поддереве root не должно быть отрицания (Polarity=Neg или
* LEMMA ∈ {not, never, no, none}). Если все условия выполнены — nor лишний,
* заменить на or.
*/

#include "rule_conj001.h"
#include <QSet>

const Rule_CONJ001& Rule_CONJ001::instance()
{
    static Rule_CONJ001 inst;
    return inst;
}

QString Rule_CONJ001::ruleId() const
{
    return QStringLiteral("CONJ-001");
}

QSet<Upos> Rule_CONJ001::anchorUpos() const
{
    return {Upos::CCONJ};
}

bool Rule_CONJ001::canConflict() const
{
    return true;
}

namespace {

// Отрицательные леммы, снимающие ошибку nor
const QSet<QString> negationLemmas = {
    QStringLiteral("not"), QStringLiteral("never"),
    QStringLiteral("no"), QStringLiteral("none")
};

/*!
* \brief Найти корень предложения, поднимаясь от узла к родителям.
* \param [in] node Стартовый узел.
* \return Корневой узел (без parent).
*/
const TokenNode* findSentenceRoot(const TokenNode& node)
{
    const TokenNode* cur = &node;
    while (cur->parent)
        cur = cur->parent;
    return cur;
}

/*!
* \brief Проверить, есть ли в поддереве узла отрицание.
* \param [in] node Корень поддерева для проверки.
* \return true если найдено отрицание (Polarity=Neg или LEMMA ∈ {not, never, no, none}).
*
* Рекурсивно обходит поддерево node, ищет токен с признаком отрицания.
*/
bool subtreeHasNegation(const TokenNode& node)
{
    // Polarity=Neg в FEATS
    if (node.features.polarityNeg)
        return true;
    // LEMMA ∈ {not, never, no, none}
    if (negationLemmas.contains(node.lemma.toLower()))
        return true;
    // Рекурсивно проверяем всех потомков
    for (const TokenNode* child : node.children)
        if (subtreeHasNegation(*child))
            return true;
    return false;
}

/*!
* \brief Проверить, есть ли у токена зависимый cc:preconj с LEMMA=neither.
* \param [in] node Токен для проверки.
* \return true если найден cc:preconj с LEMMA=neither (коррелят).
*
* Наличие neither перед nor снимает ошибку: neither...nor — корректная
* конструкция.
*/
bool hasNeitherPreconjChild(const TokenNode& node)
{
    for (const TokenNode* child : node.children)
        if (child->deprel == Deprel::CcPreconj &&
            child->lemma.toLower() == QStringLiteral("neither"))
            return true;
    return false;
}

} // namespace

QSet<CandidateError> Rule_CONJ001::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — CCONJ с LEMMA=nor
    if (anchor.upos != Upos::CCONJ)
        return res;
    if (anchor.lemma.toLower() != QStringLiteral("nor"))
        return res;

    // DEPREL должен быть cc
    if (anchor.deprel != Deprel::Cc)
        return res;

    // C2 = HEAD(nor)
    if (!anchor.parent)
        return res;
    const TokenNode& c2 = *anchor.parent;

    // C2.DEPREL должен быть conj (иначе не проверять — sentence-initial, discourse)
    if (c2.deprel != Deprel::Conj)
        return res;

    // C1 = HEAD(C2)
    if (!c2.parent)
        return res;
    const TokenNode& c1 = *c2.parent;

    // У C1 не должно быть коррелята neither (cc:preconj)
    if (hasNeitherPreconjChild(c1))
        return res;

    // Поднимаемся от C1 к root, проверяем поддерево root на отрицание
    const TokenNode* root = findSentenceRoot(c1);
    if (subtreeHasNegation(*root))
        return res;

    // Найдена ошибка: nor без отрицания и без neither
    CandidateError ce;
    ce.ruleId = QStringLiteral("CONJ-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}