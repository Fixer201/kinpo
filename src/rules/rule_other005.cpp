/*!
* \file rule_other005.cpp
* \brief Реализация правила OTHER-005: двойное отрицание.
*
* Якорь — глагол (VERB). Правило срабатывает, если у VERB есть
* прямой зависимый с Polarity=Neg и хотя бы один прямой зависимый
* с LEMMA из списка отрицательных местоимений/наречий
* (nothing, nobody, nowhere, never, none, neither, "no one").
* Каждый такой отрицательный зависимый заменяется на не-отрицательную
* форму по таблице:
*   nothing → anything, nobody → anybody, nowhere → anywhere,
*   never → ever, none → any.
*
* Одно срабатывание на один отрицательный зависимый из списка замен.
*/

#include "rule_other005.h"
#include <QHash>
#include <QSet>

const Rule_OTHER005& Rule_OTHER005::instance()
{
    static Rule_OTHER005 inst;
    return inst;
}

namespace {

/*! \brief Таблица замен отрицательных форм на не-отрицательные (по LEMMA).
*
* neither и no one добавлены в список триггеров OTHER-005, но не имеют
* не-отрицательного соответствия в таблице внешней спецификации, поэтому
* replacement для них пуст.
*/
const QHash<QString, QString>& negReplacementMap()
{
    static const QHash<QString, QString> m = {
        {QStringLiteral("nothing"),  QStringLiteral("anything")},
        {QStringLiteral("nobody"),   QStringLiteral("anybody")},
        {QStringLiteral("nowhere"),  QStringLiteral("anywhere")},
        {QStringLiteral("never"),    QStringLiteral("ever")},
        {QStringLiteral("none"),     QStringLiteral("any")},
        {QStringLiteral("neither"),  QString()},
    };
    return m;
}

/*! \brief Проверить, является ли форма составной "no one" (два слова).
* \param [in] form Форма токена.
* \return true если форма в нижнем регистре равна "no one".
*/
bool isNoOneForm(const QString& form)
{
    return form.toLower() == QStringLiteral("no one");
}

} // namespace

QString Rule_OTHER005::ruleId() const
{
    return QStringLiteral("OTHER-005");
}

QSet<Upos> Rule_OTHER005::anchorUpos() const
{
    return {Upos::Verb};
}

bool Rule_OTHER005::canConflict() const
{
    return true;
}

QSet<CandidateError> Rule_OTHER005::check(const TokenNode& anchor,
                                           int /*sentenceIndex*/,
                                           const DocumentModel& /*document*/,
                                           const CheckerRuntime& /*runtime*/) const
{
    QSet<CandidateError> res;

    // Якорь должен быть глаголом
    if (anchor.upos != Upos::Verb)
        return res;

    // Проверяем наличие прямого зависимого с Polarity=Neg
    bool hasNeg = false;
    for (const TokenNode* child : anchor.children) {
        if (!hasNeg && child && child->features.polarityNeg)
            hasNeg = true;
    }
    if (!hasNeg)
        return res;

    // Собираем кандидатов для каждого прямого зависимого из списка замен
    for (const TokenNode* child : anchor.children) {
        if (child) {
            const QString lemma = child->lemma.toLower();
            if (negReplacementMap().contains(lemma) || isNoOneForm(child->form)) {

                CandidateError ce;
                ce.ruleId = QStringLiteral("OTHER-005");
                ce.sentId = QStringLiteral("test");
                ce.displayTokenIds = {child->id};
                ce.conflictTokenIds = {child->id};
                {
                    AtomicEdit edit;
                    edit.type = AtomicEditType::ReplaceTokens;
                    edit.targetTokenIds = {child->id};
                    const QString replacement = negReplacementMap().value(lemma);
                    if (!replacement.isEmpty())
                        edit.newTokens.append(replacement);
                    ce.edits.append(edit);
                }
                const QString replacement = negReplacementMap().value(lemma);
                if (!replacement.isEmpty()) {
                    ce.description = QStringLiteral("Двойное отрицание: «%1» следует заменить на «%2».")
                                         .arg(child->form).arg(replacement);
                } else {
                    ce.description = QStringLiteral("Двойное отрицание: «%1» следует заменить.").arg(child->form);
                }
                res.insert(ce);
            }
        }
    }

    return res;
}