/*!
* \file rule_conj005.cpp
* \brief Реализация правила CONJ-005: if...or not заменяется на whether.
*
* Якорь — подчинительный союз if (SCONJ, DEPREL=mark). V — голова if,
* то есть клауза, в которой if выступает маркером. В поддереве V ищется
* сочинительный союз or, за которым линейно следует отрицательная частица
* not с DEPREL=conj. Такая конструкция (if...or not) стилистически
* рекомендуется заменять на whether...or not.
*/

#include "rule_conj005.h"
#include <QSet>

const Rule_CONJ005& Rule_CONJ005::instance()
{
    static Rule_CONJ005 inst;
    return inst;
}

QString Rule_CONJ005::ruleId() const
{
    return QStringLiteral("CONJ-005");
}

QSet<Upos> Rule_CONJ005::anchorUpos() const
{
    return {Upos::SCONJ};
}

bool Rule_CONJ005::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Проверить, входит ли токен target в поддерево root.
* \param [in] target Искомый токен.
* \param [in] root Корень поддерева.
* \return true, если при подъёме от target к корню встречается root.
*
* Поднимаемся по parent-связям от target вверх. Если по пути встречаем
* root, токен принадлежит его поддереву.
*/
bool isInSubtree(const TokenNode* target, const TokenNode& root)
{
    const TokenNode* cur = target;
    while (cur != nullptr) {
        if (cur == &root)
            return true;
        cur = cur->parent;
    }
    return false;
}

/*!
* \brief Собрать токены or (CCONJ, LEMMA=or) в поддереве root.
* \param [in] root Корень поддерева.
* \return Список токенов or, найденных рекурсивным обходом поддерева.
*
* Рекурсивно обходит всех потомков root и собирает CCONJ с леммой or.
*/
QList<const TokenNode*> collectOrInSubtree(const TokenNode& root)
{
    QList<const TokenNode*> result;
    for (const TokenNode* child : root.children) {
        if (child->upos == Upos::CCONJ &&
            child->lemma.toLower() == QStringLiteral("or"))
            result.append(child);
        result += collectOrInSubtree(*child);
    }
    return result;
}

} // namespace

QSet<CandidateError> Rule_CONJ005::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь должен быть подчинительным союзом, выступающим маркером клаузы
    if (anchor.upos != Upos::SCONJ || anchor.deprel != Deprel::Mark)
        return res;

    // Проверяем только союз if; whether корректен и не должен срабатывать
    if (anchor.lemma.toLower() != QStringLiteral("if"))
        return res;

    // V — голова if, клауза, которую if вводит
    const TokenNode* v = anchor.parent;
    if (v == nullptr)
        return res;

    // Ищем or в поддереве V: or должен относиться к той же клаузе,
    // что и if, а не к внешней конструкции
    const QList<const TokenNode*> orTokens = collectOrInSubtree(*v);

    for (const TokenNode* orTok : orTokens) {
        const TokenNode* notTok = orTok->nextNonPunct();

        // not должен идти сразу после or, пропуская пунктуацию и MWT.
        // not — отрицательная частица, сочинённый элемент из поддерева V.
        if (notTok != nullptr &&
            notTok->upos == Upos::PART &&
            notTok->lemma.toLower() == QStringLiteral("not") &&
            notTok->deprel == Deprel::Conj &&
            isInSubtree(notTok, *v)) {

            CandidateError ce;
            ce.ruleId = QStringLiteral("CONJ-005");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            {
                AtomicEdit edit;
                edit.type = AtomicEditType::ReplaceTokens;
                edit.targetTokenIds = {anchor.id};
                edit.newTokens.append(QStringLiteral("whether"));
                ce.edits.append(edit);
            }
            ce.description = QStringLiteral("В конструкции «if...or not» ожидается «whether».");
            res.insert(ce);
            return res;
        }
    }

    return res;
}