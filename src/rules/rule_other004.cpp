/*!
* \file rule_other004.cpp
* \brief Реализация правила OTHER-004: неверный падеж местоимения.
*
* Якорём служит местоимение (PRON). Если местоимение стоит в позиции
* подлежащего (DEPREL=nsubj), но имеет объектную форму (me, him, her,
* us, them), оно заменяется на именительную. Если местоимение стоит
* в позиции дополнения (DEPREL=obj или obl), но имеет именительную
* форму (I, he, she, we, they), оно заменяется на объектную.
*/

#include "rule_other004.h"
#include <QSet>
#include <QHash>

const Rule_OTHER004& Rule_OTHER004::instance()
{
    static Rule_OTHER004 inst;
    return inst;
}

namespace {

/*! \brief Объектная форма в позиции подлежащего: заменяется на именительную. */
const QHash<QString, QString>& nsubjMap()
{
    static const QHash<QString, QString> m = {
        {QStringLiteral("me"),   QStringLiteral("I")},
        {QStringLiteral("him"),  QStringLiteral("he")},
        {QStringLiteral("her"),  QStringLiteral("she")},
        {QStringLiteral("us"),   QStringLiteral("we")},
        {QStringLiteral("them"), QStringLiteral("they")},
    };
    return m;
}

/*! \brief Именительная форма в позиции дополнения: заменяется на объектную. */
const QHash<QString, QString>& objMap()
{
    static const QHash<QString, QString> m = {
        {QStringLiteral("i"),    QStringLiteral("me")},
        {QStringLiteral("he"),   QStringLiteral("him")},
        {QStringLiteral("she"),  QStringLiteral("her")},
        {QStringLiteral("we"),   QStringLiteral("us")},
        {QStringLiteral("they"), QStringLiteral("them")},
    };
    return m;
}

} // namespace

QString Rule_OTHER004::ruleId() const
{
    return QStringLiteral("OTHER-004");
}

QSet<Upos> Rule_OTHER004::anchorUpos() const
{
    return {Upos::PRON};
}

bool Rule_OTHER004::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER004::check(const TokenNode& anchor,
                                          int /*sentenceIndex*/,
                                          const DocumentModel& /*document*/,
                                          const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь должен быть местоимением
    if (anchor.upos != Upos::PRON)
        return res;

    const QString f = anchor.form.toLower();

    // Объектная форма в позиции подлежащего
    if (anchor.deprel == Deprel::Nsubj && nsubjMap().contains(f)) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-004");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    // Именительная форма в позиции дополнения
    if ((anchor.deprel == Deprel::Obj || anchor.deprel == Deprel::Obl) &&
        objMap().contains(f)) {
        CandidateError ce;
        ce.ruleId = QStringLiteral("OTHER-004");
        ce.sentId = QStringLiteral("test");
        ce.displayTokenIds = {anchor.id};
        ce.conflictTokenIds = {anchor.id};
        res.insert(ce);
        return res;
    }

    return res;
}