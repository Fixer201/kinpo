/*!
* \file rule_det002.cpp
* \brief Реализация правила DET-002: Два центральных детерминатива.
*
* Срабатывает на NOUN с ≥ 2 прямыми зависимыми-детерминативами из списка
* центральных det. Предетерминативы (all, both, half) и conj-зависимые
* не учитываются. Удаляется артикль (LEMMA ∈ {a, an, the}).
*/

#include "rule_det002.h"
#include <QSet>

QString Rule_DET002::ruleId() const
{
    return QStringLiteral("DET-002");
}

QSet<Upos> Rule_DET002::anchorUpos() const
{
    return {Upos::NOUN};
}

bool Rule_DET002::canConflict() const
{
    return true;
}

namespace {

// Центральные детерминативы
const QSet<QString> centralDets = {
    QStringLiteral("a"), QStringLiteral("an"), QStringLiteral("the"),
    QStringLiteral("this"), QStringLiteral("that"), QStringLiteral("these"), QStringLiteral("those"),
    QStringLiteral("my"), QStringLiteral("your"), QStringLiteral("his"), QStringLiteral("her"),
    QStringLiteral("its"), QStringLiteral("our"), QStringLiteral("their"),
    QStringLiteral("every"), QStringLiteral("each"), QStringLiteral("no"),
    QStringLiteral("some"), QStringLiteral("any"), QStringLiteral("either"), QStringLiteral("neither")
};

// Предетерминативы (не считаются)
const QSet<QString> predeterminers = {
    QStringLiteral("all"), QStringLiteral("both"), QStringLiteral("half")
};

// Артикли (удаляются при конфликте)
const QSet<QString> articles = {
    QStringLiteral("a"), QStringLiteral("an"), QStringLiteral("the")
};

/*!
* \brief Собрать центральные детерминативы среди прямых зависимых NOUN.
* \param [in] noun Токен NOUN.
* \return Список токенов-детерминативов (DEPREL=det, LEMMA в centralDets,
*         не предетерминатив, не через conj).
*/
QList<const TokenNode*> collectCentralDets(const TokenNode& noun)
{
    QList<const TokenNode*> dets;
    for (const TokenNode* child : noun.children) {
        if (child->deprel == Deprel::Conj)
            continue;
        const QString lemmaLower = child->lemma.toLower();
        if (!centralDets.contains(lemmaLower))
            continue;
        if (predeterminers.contains(lemmaLower))
            continue;
        dets.append(child);
    }
    return dets;
}

} // namespace

QSet<CandidateError> Rule_DET002::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::NOUN)
        return res;

    const QList<const TokenNode*> dets = collectCentralDets(anchor);

    // Нужно ≥ 2 центральных детерминатива
    if (dets.size() < 2)
        return res;

    // Удаляется артикль (приоритет 2.2: всегда артикль)
    const TokenNode* article = nullptr;
    for (const TokenNode* det : dets) {
        if (articles.contains(det->lemma.toLower())) {
            article = det;
            break;
        }
    }

    if (!article)
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("DET-002");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {article->id};
    ce.conflictTokenIds = {article->id};
    res.insert(ce);
    return res;
}