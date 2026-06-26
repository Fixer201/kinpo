/*!
* \file rule_art005a.cpp
* \brief Реализация правила ART-005a: Лишний the перед титулом + имя.
*
* Срабатывает на NOUN с DEPREL=nmod:desc к PROPN (или fallback через titles.txt
* и связь с PROPN через flat/compound), если в дереве есть DET с LEMMA=the,
* DEPREL=det, HEAD = NOUN или PROPN. Обязательно совпадение LEMMA NOUN с titles.txt.
* Исключения: Reverend, Honorable (используются с the).
*/

#include "rule_art005a.h"
#include <QSet>
#include <algorithm>

const Rule_ART005a& Rule_ART005a::instance()
{
    static Rule_ART005a inst;
    return inst;
}

QString Rule_ART005a::ruleId() const
{
    return QStringLiteral("ART-005a");
}

QSet<Upos> Rule_ART005a::anchorUpos() const
{
    return {Upos::NOUN};
}

bool Rule_ART005a::canConflict() const
{
    return true;
}

namespace {

bool isSpecialTitle(const QString& lemma)
{
    const QString lower = lemma.toLower();
    return lower == QStringLiteral("reverend") || lower == QStringLiteral("honorable");
}

bool nounHasOrIsDetChild(const TokenNode& noun, int& detId)
{
    for (const TokenNode* child : noun.children) {
        if (child->deprel == Deprel::Det &&
            child->form.toLower() == QStringLiteral("the")) {
            detId = child->id;
            return true;
        }
    }
    return false;
}

bool propnHasDetChild(const TokenNode& propn, int& detId)
{
    for (const TokenNode* child : propn.children) {
        if (child->deprel == Deprel::Det &&
            child->form.toLower() == QStringLiteral("the")) {
            detId = child->id;
            return true;
        }
    }
    return false;
}

} // namespace

QSet<CandidateError> Rule_ART005a::check(const TokenNode& anchor,
                                          int /*sentenceIndex*/,
                                          const DocumentModel& /*document*/,
                                          const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::NOUN)
        return res;

    const QString nounLemma = anchor.lemma.toLower();

    // Обязательное условие: LEMMA NOUN в titles.txt
    if (!runtime.resources.titles.contains(nounLemma))
        return res;

    // Исключения: Reverend, Honorable
    if (isSpecialTitle(nounLemma))
        return res;

    // Определяем PROPN, с которым связан титул
    const TokenNode* propn = nullptr;

    // Основной путь: DEPREL=nmod:desc к PROPN
    if (anchor.deprel == Deprel::NmodDesc && anchor.parent &&
        anchor.parent->upos == Upos::PROPN) {
        propn = anchor.parent;
    }

    // Fallback: T.LEMMA из titles.txt + связь с PROPN через flat/compound
    if (!propn) {
        // HEAD — PROPN, связь через flat/compound
        if (anchor.parent && anchor.parent->upos == Upos::PROPN &&
            (anchor.deprel == Deprel::Flat ||
             anchor.deprel == Deprel::FlatName ||
             anchor.deprel == Deprel::Compound)) {
            propn = anchor.parent;
        }
        // Зависимый PROPN, связь через flat/compound
        bool propnFound = false;
        for (const TokenNode* child : anchor.children) {
            if (!propnFound && child->upos == Upos::PROPN &&
                (child->deprel == Deprel::Flat ||
                 child->deprel == Deprel::FlatName ||
                 child->deprel == Deprel::Compound)) {
                propn = child;
                propnFound = true;
            }
        }
    }

    if (!propn)
        return res;

    // Ищем DET с LEMMA=the, DEPREL=det, HEAD = NOUN или PROPN
    int detId = -1;
    bool found = nounHasOrIsDetChild(anchor, detId) ||
                 propnHasDetChild(*propn, detId);
    if (!found)
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-005a");
    ce.sentId = QStringLiteral("test");
    QList<int> displayIds = {detId, anchor.id, propn->id};
    std::sort(displayIds.begin(), displayIds.end());
    ce.displayTokenIds = displayIds;
    ce.conflictTokenIds = {detId};
    AtomicEdit edit;
    edit.type = AtomicEditType::DeleteTokens;
    edit.targetTokenIds = {detId};
    ce.edits.append(edit);
    ce.description = QStringLiteral("Артикль «the» не используется с титулом перед именем.");
    res.insert(ce);
    return res;
}