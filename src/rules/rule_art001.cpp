/*!
* \file rule_art001.cpp
* \brief Реализация правила ART-001: Лишний артикль перед PROPN
*
* Срабатывает на DET с lemma {a, an, the} перед PROPN
* Исключения для the: географические названия, фамилии во мн.ч., классификаторы
* TODO: загрузить geo_the.txt в CheckerRuntime::resources
*/

#include "rule_art001.h"
#include <QSet>

QString Rule_ART001::ruleId() const
{
    return QStringLiteral("ART-001");
}

QSet<Upos> Rule_ART001::anchorUpos() const
{
    return {Upos::DET};
}

bool Rule_ART001::canConflict() const
{
    return true;
}

namespace {

// TODO: заменить на lookup в runtime.resources.geoThe когда будет RuleResources
bool isGeoException(const QString& form)
{
    static const QSet<QString> names = {
        QStringLiteral("pacific"),
        QStringLiteral("ocean"),
        QStringLiteral("alps"),
        QStringLiteral("atlantic"),
        QStringLiteral("mediterranean"),
        QStringLiteral("sahara"),
        QStringLiteral("everest"),
        QStringLiteral("thames")
    };
    return names.contains(form.toLower());
}

// true если у propnToken есть child с DEPREL=compound
bool isClassifier(const TokenNode& propnToken)
{
    for (TokenNode* child : propnToken.children)
        if (child->deprel == Deprel::Compound)
            return true;
    return false;
}

} // anonymous namespace

QSet<CandidateError> Rule_ART001::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Быстрый фильтр: только DET с lemma {a, an, the}, parent = PROPN
    if (anchor.upos != Upos::DET)
        return res;
    QStringList articles = {QStringLiteral("a"), QStringLiteral("an"), QStringLiteral("the")};
    if (!articles.contains(anchor.form.toLower()))
        return res;
    if (!anchor.parent)
        return res;
    if (anchor.parent->upos != Upos::PROPN)
        return res;

    // Исключения только для the
    if (anchor.form.toLower() == QStringLiteral("the")) {
        if (isGeoException(anchor.parent->form))
            return res;
        if (anchor.parent->form.endsWith('s')) // фамилия во мн.ч.
            return res;
        if (isClassifier(*anchor.parent))
            return res;
    }

    // Ошибка подтверждена
    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}
