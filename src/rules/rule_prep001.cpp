/*!
* \file rule_prep001.cpp
* \brief Реализация правила PREP-001: Неверный временной предлог.
*
* Срабатывает на ADP с LEMMA ∈ {in, on, at}, DEPREL=case. HEAD предлога —
* временное выражение N. Тип N определяет ожидаемый предлог:
*  — конкретное время (noon, midnight, midday, dawn, dusk, sunrise, sunset,
*    daybreak, nightfall, или N.UPOS=NUM и N.FORM вида \d{1,2}:\d{2}, или
*    у N есть зависимый с LEMMA=o'clock, или parent(N) с LEMMA=o'clock) → at;
*  — день недели (N.LEMMA из time_units.txt — дни) или дата (N.UPOS=NOUN и
*    у N есть зависимый с NumType=Ord) → on;
*  — месяц/сезон (N.LEMMA из time_units.txt — месяцы/сезоны) → in;
*  — год (N.UPOS=NUM и N.FORM вида \d{4}) или N.LEMMA=century → in;
*  — weekend → at или on (ошибка только in);
*  — morning, afternoon, evening → in;
*  — night → at.
* Исключение: если N.LEMMA ∈ {morning, afternoon, evening} и у N есть
* зависимый с DEPREL=compound и LEMMA из дней недели → ожидаемый предлог = on.
*/

#include "rule_prep001.h"
#include <QSet>
#include <QRegularExpression>

const Rule_PREP001& Rule_PREP001::instance()
{
    static Rule_PREP001 inst;
    return inst;
}

QString Rule_PREP001::ruleId() const
{
    return QStringLiteral("PREP-001");
}

QSet<Upos> Rule_PREP001::anchorUpos() const
{
    return {Upos::Adp};
}

bool Rule_PREP001::canConflict() const
{
    // Один и тот же предлог может быть ошибочным для нескольких правил
    return true;
}

namespace {

// Дни недели (первые 7 записей time_units.txt)
const QSet<QString> daysOfWeek = {
    QStringLiteral("monday"), QStringLiteral("tuesday"),
    QStringLiteral("wednesday"), QStringLiteral("thursday"),
    QStringLiteral("friday"), QStringLiteral("saturday"),
    QStringLiteral("sunday")
};

// Месяцы и сезоны (записи 8-22 в time_units.txt)
const QSet<QString> monthsAndSeasons = {
    QStringLiteral("january"), QStringLiteral("february"),
    QStringLiteral("march"), QStringLiteral("april"),
    QStringLiteral("may"), QStringLiteral("june"),
    QStringLiteral("july"), QStringLiteral("august"),
    QStringLiteral("september"), QStringLiteral("october"),
    QStringLiteral("november"), QStringLiteral("december"),
    QStringLiteral("spring"), QStringLiteral("summer"),
    QStringLiteral("autumn"), QStringLiteral("fall"),
    QStringLiteral("winter")
};

// Части дня, требующие предлог in (исключение — compound с днём недели → on)
const QSet<QString> dayPartsIn = {
    QStringLiteral("morning"), QStringLiteral("afternoon"),
    QStringLiteral("evening")
};

// Конкретные моменты времени, требующие предлог at
const QSet<QString> specificTimeWords = {
    QStringLiteral("noon"), QStringLiteral("midnight"),
    QStringLiteral("midday"), QStringLiteral("dawn"),
    QStringLiteral("dusk"), QStringLiteral("sunrise"),
    QStringLiteral("sunset"), QStringLiteral("daybreak"),
    QStringLiteral("nightfall")
};

// Паттерн времени вида 12:30
const QRegularExpression timePattern(QStringLiteral("^\\d{1,2}:\\d{2}$"));
// Паттерн года вида 2020
const QRegularExpression yearPattern(QStringLiteral("^\\d{4}$"));

/*!
* \brief Проверить, есть ли у токена зависимый с заданной LEMMA.
* \param [in] node Токен для проверки.
* \param [in] lemma Искомая лемма (в нижнем регистре).
* \return true если найден зависимый с совпадающей LEMMA.
*/
bool hasChildWithLemma(const TokenNode& node, const QString& lemma)
{
    for (const TokenNode* child : node.children)
        if (child->lemma.toLower() == lemma)
            return true;
    return false;
}

/*!
* \brief Проверить, есть ли у токена зависимый с DEPREL=compound и LEMMA из дней недели.
* \param [in] node Токен для проверки.
* \return true если найден compound-зависимый с LEMMA дня недели.
*
* Реализует исключение: "on Monday morning" — compound Monday переводит
* morning в категорию "день недели", ожидаемый предлог становится on.
*/
bool hasCompoundDayOfWeek(const TokenNode& node)
{
    for (const TokenNode* child : node.children)
        if (child->deprel == Deprel::Compound &&
            daysOfWeek.contains(child->lemma.toLower()))
            return true;
    return false;
}

/*!
* \brief Проверить, есть ли у токена зависимый с NumType=Ord.
* \param [in] node Токен для проверки.
* \return true если найден зависимый с признаком NumType=Ord.
*
* Дата определяется наличием порядкового числительного: "on 3 May".
*/
bool hasChildNumTypeOrd(const TokenNode& node)
{
    for (const TokenNode* child : node.children)
        if (child->features.numTypeOrd)
            return true;
    return false;
}

/*!
* \brief Определить ожидаемый предлог для временного выражения N.
* \param [in] n Временное выражение (HEAD предлога).
* \return "in", "on", "at" или пустая строка если N не распознано.
*
* Порядок проверок соответствует приоритетам спецификации:
* конкретное время → день/дата → месяц/сезон → год/век → weekend →
* часть дня → ночь.
*/
/*!
* \brief Определить ожидаемый предлог для временного выражения N.
* \param [in] n Временное выражение (HEAD предлога).
* \param [in] runtime Runtime со словарём timeUnits.
* \return "in", "on", "at" или пустая строка если N не распознано.
*
* Порядок проверок соответствует приоритетам спецификации:
* конкретное время → день/дата → месяц/сезон → год/век → weekend →
* часть дня → ночь.
*
* Проверки по шаблону (время вида 12:30, год, o'clock) и по хардкодным
* категориям (weekend, night, century) выполняются до сверки
* с time_units.txt. Категорийные проверки (дни недели, месяцы,
* части дня) требуют, чтобы лемма была в time_units.txt — это
* гарантирует что кастомный lists/time_units.txt влияет на PREP-001.
*/
QString expectedPreposition(const TokenNode& n, const CheckerRuntime& runtime)
{
    const QString lemmaLower = n.lemma.toLower();
    const QString form = n.form;

    // Конкретное время: слова-маркеры, время вида 12:30 или o'clock рядом
    // Эти проверки по FORM или хардкодному набору — timeUnits не нужен.
    if (specificTimeWords.contains(lemmaLower))
        return QStringLiteral("at");
    if (n.upos == Upos::Num && timePattern.match(form).hasMatch())
        return QStringLiteral("at");
    if (hasChildWithLemma(n, QStringLiteral("o'clock")))
        return QStringLiteral("at");
    if (n.upos == Upos::Num && n.parent &&
        n.parent->lemma.toLower() == QStringLiteral("o'clock"))
        return QStringLiteral("at");

    // Год (число из 4 цифр) или слово "century"
    if (n.upos == Upos::Num && yearPattern.match(form).hasMatch())
        return QStringLiteral("in");
    if (lemmaLower == QStringLiteral("century"))
        return QStringLiteral("in");

    // Weekend: in — ошибка, on и at допустимы.
    if (lemmaLower == QStringLiteral("weekend"))
        return QStringLiteral("");

    // Ночь → at
    if (lemmaLower == QStringLiteral("night"))
        return QStringLiteral("at");

    // Категорийные проверки ниже требуют присутствия леммы в time_units.txt.
    // Это сделано намеренно: если пользователь через --lists убрал слово из
    // time_units.txt, PREP-001 не будет проверять для него предлог.
    if (!runtime.resources.timeUnits.contains(lemmaLower))
        return QString();

    // День недели или дата (NOUN с порядковым числительным)
    if (daysOfWeek.contains(lemmaLower))
        return QStringLiteral("on");
    if (n.upos == Upos::Noun && hasChildNumTypeOrd(n))
        return QStringLiteral("on");

    // Месяц или сезон
    if (monthsAndSeasons.contains(lemmaLower))
        return QStringLiteral("in");

    // Часть дня (morning/afternoon/evening) → in, но compound с днём недели → on
    if (dayPartsIn.contains(lemmaLower)) {
        if (hasCompoundDayOfWeek(n))
            return QStringLiteral("on");
        return QStringLiteral("in");
    }

    // N не распознано как временное выражение
    return QString();
}

} // namespace

QSet<CandidateError> Rule_PREP001::check(const TokenNode& anchor,
                                         int /*sentenceIndex*/,
                                         const DocumentModel& /*document*/,
                                         const CheckerRuntime& runtime) const
{
    QSet<CandidateError> res;

    // Якорь — ADP с LEMMA ∈ {in, on, at}
    if (anchor.upos != Upos::Adp)
        return res;

    const QString formLower = anchor.lemma.toLower();
    if (formLower != QStringLiteral("in") &&
        formLower != QStringLiteral("on") &&
        formLower != QStringLiteral("at"))
        return res;

    // DEPREL должен быть case
    if (anchor.deprel != Deprel::Case)
        return res;

    // HEAD — временное выражение N
    if (!anchor.parent)
        return res;

    const TokenNode& n = *anchor.parent;
    const QString expected = expectedPreposition(n, runtime);

    // Weekend: ошибка только для in, on и at допустимы.
    // expectedPreposition возвращает пустую строку, обрабатываем отдельно.
    if (expected.isEmpty()) {
        if (n.lemma.toLower() == QStringLiteral("weekend") &&
            formLower == QStringLiteral("in")) {
            CandidateError ce;
            ce.ruleId = QStringLiteral("PREP-001");
            ce.sentId = QStringLiteral("test");
            ce.displayTokenIds = {anchor.id};
            ce.conflictTokenIds = {anchor.id};
            AtomicEdit edit;
            edit.type = AtomicEditType::ReplaceTokens;
            edit.targetTokenIds = {anchor.id};
            edit.newTokens.append(QStringLiteral("at"));
            ce.edits.append(edit);
            ce.description = QStringLiteral("С временным выражением «%1» используется предлог «%2», а не «%3».")
                                 .arg(n.form).arg(QStringLiteral("at")).arg(formLower);
            res.insert(ce);
        }
        return res;
    }

    // Форма совпадает с ожидаемой — ошибки нет
    if (formLower == expected)
        return res;

    // Найдена ошибка: предлог не соответствует типу временного выражения
    CandidateError ce;
    ce.ruleId = QStringLiteral("PREP-001");
    ce.sentId = QStringLiteral("test");
    ce.displayTokenIds = {anchor.id};
    ce.conflictTokenIds = {anchor.id};
    {
        AtomicEdit edit;
        edit.type = AtomicEditType::ReplaceTokens;
        edit.targetTokenIds = {anchor.id};
        edit.newTokens.append(expected);
        ce.edits.append(edit);
    }
    ce.description = QStringLiteral("С временным выражением «%1» используется предлог «%2», а не «%3».")
                         .arg(n.form).arg(expected).arg(formLower);
    res.insert(ce);

    // Night: добавить кандидата удаления det=the, если у HEAD(night) есть
    // зависимый det с lemma=the
    if (n.lemma.toLower() == QStringLiteral("night")) {
        bool detFound = false;
        for (const TokenNode* child : n.children) {
            if (!detFound && child->deprel == Deprel::Det &&
                child->lemma.toLower() == QStringLiteral("the")) {
                CandidateError detCe;
                detCe.ruleId = QStringLiteral("PREP-001");
                detCe.sentId = QStringLiteral("test");
                detCe.displayTokenIds = {child->id};
                detCe.conflictTokenIds = {child->id};
                AtomicEdit detEdit;
                detEdit.type = AtomicEditType::DeleteTokens;
                detEdit.targetTokenIds = {child->id};
                detCe.edits.append(detEdit);
                detCe.description = QStringLiteral("Перед «night» с предлогом «at» артикль не используется.");
                res.insert(detCe);
                detFound = true;
            }
        }
    }

    return res;
}