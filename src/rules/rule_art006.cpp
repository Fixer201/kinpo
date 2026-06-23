/*!
* \file rule_art006.cpp
* \brief Реализация правила ART-006: Неверный a/an.
*
* Срабатывает на DET с FORM ∈ {a, an}, UPOS=DET. Следующий токен (линейный)
* определяет ожидаемый артикль через CMUdict, таблицу аббревиатур или
* анализ числа. Исключение: не срабатывает при DET-001.
*/

#include "rule_art006.h"
#include <QSet>
#include <QStringList>

const Rule_ART006& Rule_ART006::instance()
{
    static Rule_ART006 inst;
    return inst;
}

QString Rule_ART006::ruleId() const
{
    return QStringLiteral("ART-006");
}

QSet<Upos> Rule_ART006::anchorUpos() const
{
    return {Upos::DET};
}

bool Rule_ART006::canConflict() const
{
    return true;
}

namespace {

// Гласные фонемы CMU (без цифр ударения)
const QSet<QString> vowelPhonemes = {
    QStringLiteral("AA"), QStringLiteral("AE"), QStringLiteral("AH"),
    QStringLiteral("AO"), QStringLiteral("AW"), QStringLiteral("AY"),
    QStringLiteral("EH"), QStringLiteral("ER"), QStringLiteral("EY"),
    QStringLiteral("IH"), QStringLiteral("IY"), QStringLiteral("OW"),
    QStringLiteral("OY"), QStringLiteral("UH"), QStringLiteral("UW")
};

// Буквы аббревиатур с гласным звуком
const QSet<QChar> vowelLetters = {
    'A', 'E', 'F', 'H', 'I', 'L', 'M', 'N', 'O', 'R', 'S', 'X'
};

/*!
* \brief Определить, начинается ли фонема с гласной.
* \param [in] phoneme Фонема CMU (например, "AE1").
* \return true если фонема гласная.
*
* Цифры ударения (0/1/2) отбрасываются, базовая фонема проверяется
* по списку гласных.
*/
bool isVowelPhoneme(const QString& phoneme)
{
    QString base = phoneme;
    while (!base.isEmpty() && base.back().isDigit())
        base.chop(1);
    return vowelPhonemes.contains(base);
}

/*!
* \brief Проверить, является ли форма аббревиатурой.
* \param [in] form Форма токена.
* \return true если форма — аббревиатура (все заглавные, ≥2 буквы).
*/
bool isAbbreviation(const QString& form)
{
    if (form.size() < 2)
        return false;
    for (const QChar& c : form) {
        if (!c.isUpper())
            return false;
    }
    return true;
}

/*!
* \brief Проверить, является ли форма числом.
* \param [in] form Форма токена.
* \return true если форма — число или число с суффиксом (18th).
*/
bool isNumber(const QString& form)
{
    if (form.isEmpty())
        return false;
    // Первые символы — цифры, остальное — суффикс (th, st, nd, rd)
    int i = 0;
    while (i < form.size() && form[i].isDigit())
        ++i;
    if (i == 0)
        return false;
    if (i == form.size())
        return true;
    const QString suffix = form.mid(i).toLower();
    return suffix == QStringLiteral("th") || suffix == QStringLiteral("st") ||
           suffix == QStringLiteral("nd") || suffix == QStringLiteral("rd");
}

/*!
* \brief Определить ожидаемый артикль для числа.
* \param [in] form Форма числа (например, "18", "11th").
* \return "an" если число требует "an", "a" если "a".
*/
QString expectedArticleForNumber(const QString& form)
{
    // «8...» → an (8, 80, 800, 8th)
    // {11, 11th, 18, 18th} → an
    if (form.startsWith('8'))
        return QStringLiteral("an");
    const QString lower = form.toLower();
    if (lower == QStringLiteral("11") || lower == QStringLiteral("11th") ||
        lower == QStringLiteral("18") || lower == QStringLiteral("18th"))
        return QStringLiteral("an");
    return QStringLiteral("a");
}

/*!
* \brief Определить ожидаемый артикль для аббревиатуры.
* \param [in] form Форма аббревиатуры (заглавные буквы).
* \return "an" если первая буква с гласным звуком, "a" если с согласным.
*/
QString expectedArticleForAbbreviation(const QString& form)
{
    if (vowelLetters.contains(form.at(0)))
        return QStringLiteral("an");
    return QStringLiteral("a");
}

} // namespace

QSet<CandidateError> Rule_ART006::check(const TokenNode& anchor,
                                        int /*sentenceIndex*/,
                                        const DocumentModel& /*document*/,
                                        const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    if (anchor.upos != Upos::DET)
        return res;

    const QString formLower = anchor.form.toLower();
    if (formLower != QStringLiteral("a") && formLower != QStringLiteral("an"))
        return res;

    // Следующий токен (линейный) с пропуском PUNCT
    const TokenNode* next = anchor.nextNonPunct();
    if (!next)
        return res;

    const QString nextForm = next->form;
    const QString nextFormLower = nextForm.toLower();
    QString expected;

    // Правило 1: CMUdict
    auto it = runtime.resources.cmudict.find(nextFormLower);
    if (it != runtime.resources.cmudict.end() && !it.value().isEmpty()) {
        // Первая фонема определяет гласный/согласный звук
        if (isVowelPhoneme(it.value().first()))
            expected = QStringLiteral("an");
        else
            expected = QStringLiteral("a");
    }
    // Правило 2: Аббревиатура (заглавные, ≥2, нет в CMUdict)
    else if (isAbbreviation(nextForm)) {
        expected = expectedArticleForAbbreviation(nextForm);
    }
    // Правило 3: Число
    else if (isNumber(nextForm)) {
        expected = expectedArticleForNumber(nextForm);
    }
    // Правило 4: Иначе — не применяется
    else {
        return res;
    }

    // Если ожидаемый артикль совпадает с фактическим — ошибки нет
    if (expected == formLower)
        return res;

    CandidateError ce;
    ce.ruleId = QStringLiteral("ART-006");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    res.insert(ce);
    return res;
}