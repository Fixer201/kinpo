/*!
* \file rule_conj004.cpp
* \brief Реализация правила CONJ-004: неверный коррелят.
*
* Срабатывает на CCONJ с LEMMA=or или nor, DEPREL=cc. В предложении
* ищется коррелят-предетерминатив (CCONJ с DEPREL=cc:preconj и LEMMA=neither
* либо either). При neither каждый or ошибочен (ожидается nor). При either
* каждый nor ошибочен (ожидается or). Союз, согласующийся с коррелятом
* (nor при neither, or при either), не считается ошибкой.
*/

#include "rule_conj004.h"
#include <QSet>

const Rule_CONJ004& Rule_CONJ004::instance()
{
    static Rule_CONJ004 inst;
    return inst;
}

QString Rule_CONJ004::ruleId() const
{
    return QStringLiteral("CONJ-004");
}

QSet<Upos> Rule_CONJ004::anchorUpos() const
{
    return {Upos::CCONJ};
}

bool Rule_CONJ004::canConflict() const
{
    return true;
}

namespace {

/*!
* \brief Проверить наличие коррелята-предетерминатива в предложении.
* \param [in] document Модель документа.
* \param [in] sentenceIndex Индекс предложения.
* \param [in] correlateLemma Искомая лемма (neither или either).
* \return true, если в предложении есть CCONJ с DEPREL=cc:preconj
*         и указанной леммой.
*
* Коррелят (neither/either) присоединяется к первому элементу
* перечисления через DEPREL=cc:preconj.
*/
bool hasCorrelate(const DocumentModel& document, int sentenceIndex,
                  const QString& correlateLemma)
{
    if (sentenceIndex < 0 || sentenceIndex >= static_cast<int>(document.sentences.size()))
        return false;

    const SentenceModel& sentence = *document.sentences[sentenceIndex];
    const QString lower = correlateLemma.toLower();

    for (const TokenNode* token : sentence.tokens) {
        if (token->upos != Upos::CCONJ)
            continue;
        if (token->deprel != Deprel::CcPreconj)
            continue;
        if (token->lemma.toLower() == lower)
            return true;
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_CONJ004::check(const TokenNode& anchor,
                                         int sentenceIndex,
                                         const DocumentModel& document,
                                         const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — координатор, присоединяющий элемент перечисления
    if (anchor.upos != Upos::CCONJ || anchor.deprel != Deprel::Cc)
        return res;

    const QString lemma = anchor.lemma.toLower();

    // neither в предложении: or заменяется на nor, nor заменяется на or.
    // В смешанных конструкциях (neither...or...nor) оба союза ошибочны.
    if ((lemma == QStringLiteral("or") || lemma == QStringLiteral("nor")) &&
        hasCorrelate(document, sentenceIndex, QStringLiteral("neither"))) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("CONJ-004");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        {
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {anchor.id};
            const QString expected = (lemma == QStringLiteral("or"))
                                     ? QStringLiteral("nor") : QStringLiteral("or");
            edit.newTokens = {expected};
            ce.edits.append(edit);
        }
        const QString expected = (lemma == QStringLiteral("or"))
                                 ? QStringLiteral("nor") : QStringLiteral("or");
        ce.description = QStringLiteral("В паре «neither» ожидается «%1», а не «%2».")
                             .arg(expected).arg(lemma);
        res.insert(ce);
        return res;
    }

    // either в предложении: nor заменяется на or, or заменяется на nor.
    // В смешанных конструкциях (either...nor...or) оба союза ошибочны.
    if ((lemma == QStringLiteral("nor") || lemma == QStringLiteral("or")) &&
        hasCorrelate(document, sentenceIndex, QStringLiteral("either"))) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("CONJ-004");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        {
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {anchor.id};
            const QString expected = (lemma == QStringLiteral("nor"))
                                     ? QStringLiteral("or") : QStringLiteral("nor");
            edit.newTokens = {expected};
            ce.edits.append(edit);
        }
        const QString expected = (lemma == QStringLiteral("nor"))
                                 ? QStringLiteral("or") : QStringLiteral("nor");
        ce.description = QStringLiteral("В паре «either» ожидается «%1», а не «%2».")
                             .arg(expected).arg(lemma);
        res.insert(ce);
        return res;
    }

    return res;
}