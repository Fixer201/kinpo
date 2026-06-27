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
    return {Upos::CConj};
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

    bool found = false;
    for (const TokenNode* token : sentence.tokens) {
        if (token->upos == Upos::CConj && token->deprel == Deprel::CcPreconj &&
            token->lemma.toLower() == lower)
            found = true;
    }
    return found;
}

/*!
* \brief Проверить, есть ли в предложении токен с заданной леммой.
* \param [in] document Модель документа.
* \param [in] sentenceIndex Индекс предложения.
* \param [in] lemma Искомая лемма.
* \return true если токен с такой леммой найден.
*/
bool hasLemmaInSentence(const DocumentModel& document, int sentenceIndex,
                        const QString& lemma)
{
    if (sentenceIndex < 0 || sentenceIndex >= static_cast<int>(document.sentences.size()))
        return false;

    const SentenceModel& sentence = *document.sentences[sentenceIndex];
    const QString lower = lemma.toLower();

    for (const TokenNode* token : sentence.tokens)
        if (token->lemma.toLower() == lower)
            return true;
    return false;
}

/*!
* \brief Создать CandidateError для CONJ-004 с заменой союза.
* \param [in] anchor Ошибочный союз.
* \param [in] res Множество для добавления кандидата.
* \param [in] expected Ожидаемый союз (nor или or).
* \return Множество с добавленным кандидатом.
*/
QSet<CandidateError> produceError(const TokenNode& anchor, QSet<CandidateError> res,
                                  const QString& expected)
{
    CandidateError ce;
    ce.ruleId = QStringLiteral("CONJ-004");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    AtomicEdit edit;
    edit.type = AtomicEditType::ReplaceTokens;
    edit.targetTokenIds = {anchor.id};
    edit.newTokens.append(expected);
    ce.edits.append(edit);
    const QString correlate = (expected == QStringLiteral("nor"))
                              ? QStringLiteral("neither") : QStringLiteral("either");
    ce.description = QStringLiteral("В паре «%1» ожидается «%2», а не «%3».")
                         .arg(correlate).arg(expected).arg(anchor.lemma.toLower());
    res.insert(ce);
    return res;
}

} // namespace

QSet<CandidateError> Rule_CONJ004::check(const TokenNode& anchor,
                                         int sentenceIndex,
                                         const DocumentModel& document,
                                         const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь — координатор, присоединяющий элемент перечисления
    if (anchor.upos != Upos::CConj || anchor.deprel != Deprel::Cc)
        return res;

    const QString lemma = anchor.lemma.toLower();

    // neither в предложении: or всегда ошибочен, nor — только в смешанной
    // конструкции где есть и or, и nor одновременно
    if (lemma == QStringLiteral("or") && hasCorrelate(document, sentenceIndex, QStringLiteral("neither"))) {
        return produceError(anchor, res, QStringLiteral("nor"));
    }
    if (lemma == QStringLiteral("nor") && hasCorrelate(document, sentenceIndex, QStringLiteral("neither")) &&
        hasLemmaInSentence(document, sentenceIndex, QStringLiteral("or"))) {
        return produceError(anchor, res, QStringLiteral("or"));
    }

    // either в предложении: nor всегда ошибочен, or — только в смешанной
    // конструкции где есть и nor, и or одновременно
    if (lemma == QStringLiteral("nor") && hasCorrelate(document, sentenceIndex, QStringLiteral("either"))) {
        return produceError(anchor, res, QStringLiteral("or"));
    }
    if (lemma == QStringLiteral("or") && hasCorrelate(document, sentenceIndex, QStringLiteral("either")) &&
        hasLemmaInSentence(document, sentenceIndex, QStringLiteral("nor"))) {
        return produceError(anchor, res, QStringLiteral("nor"));
    }

    return res;
}