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
#include <algorithm>

const Rule_DET002& Rule_DET002::instance()
{
    static Rule_DET002 inst;
    return inst;
}

QString Rule_DET002::ruleId() const
{
    return QStringLiteral("DET-002");
}

QSet<Upos> Rule_DET002::anchorUpos() const
{
    return {Upos::Noun};
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

// Притяжательные детерминативы
const QSet<QString> possessiveDets = {
    QStringLiteral("my"), QStringLiteral("your"), QStringLiteral("his"),
    QStringLiteral("her"), QStringLiteral("its"), QStringLiteral("our"),
    QStringLiteral("their")
};

// Указательные детерминативы
const QSet<QString> demonstrativeDets = {
    QStringLiteral("this"), QStringLiteral("that"),
    QStringLiteral("these"), QStringLiteral("those")
};

/*!
* \brief Классифицировать тип детерминатива для описания ошибки.
* \param [in] lemma Лемма детерминатива (нижний регистр).
* \return Русское название типа: «притяжательное», «указательное», «квантор».
*/
QString detTypeLabel(const QString& lemma)
{
    if (possessiveDets.contains(lemma))
        return QStringLiteral("притяжательное");
    if (demonstrativeDets.contains(lemma))
        return QStringLiteral("указательное");
    return QStringLiteral("квантор");
}

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
        const QString lemmaLower = child->lemma.toLower();
        if (child->deprel != Deprel::Conj &&
            centralDets.contains(lemmaLower) &&
            !predeterminers.contains(lemmaLower))
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

    if (anchor.upos != Upos::Noun)
        return res;

    const QList<const TokenNode*> dets = collectCentralDets(anchor);

    // Нужно ≥ 2 центральных детерминатива
    if (dets.size() < 2)
        return res;

    // Собираем отсортированный список ID всех детерминативов для фрагмента
    QList<int> allDetIds;
    for (const TokenNode* det : dets)
        allDetIds.append(det->id);
    std::sort(allDetIds.begin(), allDetIds.end());

    // Собираем все артикли среди центральных детерминативов
    QList<const TokenNode*> articleDets;
    for (const TokenNode* det : dets) {
        if (articles.contains(det->lemma.toLower()))
            articleDets.append(det);
    }

    if (!articleDets.isEmpty()) {
        // Есть артикль: создать кандидат удаления для каждого артикля
        for (const TokenNode* art : articleDets) {
            // Найти второй детерминатив (не артикль) для определения типа
            QString typeLabel;
            for (const TokenNode* det : dets) {
                if (typeLabel.isEmpty() && det != art &&
                    !articles.contains(det->lemma.toLower()))
                    typeLabel = detTypeLabel(det->lemma.toLower());
            }
            if (typeLabel.isEmpty())
                typeLabel = QStringLiteral("детерминатив");

            CandidateError ce;
            ce.ruleId = QStringLiteral("DET-002");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = allDetIds;
            ce.conflictTokenIds = {art->id};
            AtomicEdit edit;
            edit.type = AtomicEditType::DeleteTokens;
            edit.targetTokenIds = {art->id};
            ce.edits.append(edit);
            ce.description = QStringLiteral("Артикль и %1 не могут использоваться вместе.").arg(typeLabel);
            res.insert(ce);
        }
        return res;
    }

    // Артикля нет: сохранить первого по id, остальных удалить
    const TokenNode* minDet = dets.first();
    for (const TokenNode* det : dets) {
        if (det->id < minDet->id)
            minDet = det;
    }
    for (const TokenNode* det : dets) {
        if (det != minDet) {
            QString typeLabel = detTypeLabel(det->lemma.toLower());

            CandidateError ce;
            ce.ruleId = QStringLiteral("DET-002");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = allDetIds;
            ce.conflictTokenIds = {det->id};
            AtomicEdit edit;
            edit.type = AtomicEditType::DeleteTokens;
            edit.targetTokenIds = {det->id};
            ce.edits.append(edit);
            ce.description = QStringLiteral("Артикль и %1 не могут использоваться вместе.").arg(typeLabel);
            res.insert(ce);
        }
    }
    return res;
}