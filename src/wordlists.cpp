/*!
* \file wordlists.cpp
* \brief Реализация модуля загрузки словарей.
*
* Содержит функцию loadResources и вспомогательные функции для парсинга
* простых списков слов и сложных форматов (past_forms, det_compat, verb_prep).
*/

#include "wordlists.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSet>
#include <QHash>
#include <QStringList>
#if QT_VERSION_MAJOR >= 6
#include <QStringConverter>
#endif

/*!
* \brief Установить UTF-8 кодировку для QTextStream.
* \param [in,out] stream Поток для настройки.
*
* В Qt5 и Qt6 установка кодировки выполняется разными функциями:
* setCodec в Qt5 и setEncoding в Qt6. Вспомогательная функция скрывает
* это различие, позволяя остальному коду работать одинаково.
*/
void setUtf8Encoding(QTextStream& stream)
{
#if QT_VERSION_MAJOR >= 6
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
}

namespace {

/*!
* \brief Загрузить простой список слов из файла в QSet<QString>.
* \param [in] filePath Путь к файлу словаря.
* \param [out] target Множество для заполнения.
* \return true при успехе, false если файл недоступен.
*
* Каждая непустая строка, не начинающаяся с '#', добавляется в нижнем
* регистре. Пробелы в начале и конце строки удаляются.
*/
bool loadWordSet(const QString& filePath, QSet<QString>& target)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#'))
            target.insert(line.toLower());
    }
    return true;
}

/*!
* \brief Загрузить таблицу неправильных глаголов (past_forms.txt).
* \param [in] filePath Путь к файлу.
* \param [out] target Хеш-таблица для заполнения.
* \return true при успехе, false если файл недоступен.
*
* Формат: LEMMA\tPastSimple\tPastParticiple. LEMMA приводится к нижнему
* регистру.
*/
bool loadPastForms(const QString& filePath, QHash<QString, PastForms>& target)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#')) {
            QStringList cols = line.split('\t');
            if (cols.size() == 3) {
                PastForms pf;
                pf.pastSimple = cols[1].toLower();
                pf.pastParticiple = cols[2].toLower();
                target.insert(cols[0].toLower(), pf);
            }
        }
    }
    return true;
}

/*!
* \brief Разобрать строку условий det_compat (например, "Plur,not_unc").
* \param [in] conditions Строка условий.
* \param [out] number Целевое число.
* \param [out] countability Целевая исчисляемость.
* \return true если строка разобрана корректно.
*/
bool parseDetCompatConditions(const QString& conditions,
                              NumberCondition& number,
                              CountabilityCondition& countability)
{
    number = NumberCondition::Any;
    countability = CountabilityCondition::Any;

    QStringList parts = conditions.split(',');
    for (const QString& p : parts) {
        QString token = p.trimmed().toLower();
        if (token == "sing") {
            number = NumberCondition::Sing;
        } else if (token == "plur") {
            number = NumberCondition::Plur;
        } else if (token == "unc") {
            countability = CountabilityCondition::Uncountable;
        } else if (token == "not_unc") {
            countability = CountabilityCondition::Countable;
        } else {
            return false;
        }
    }
    return true;
}

/*!
* \brief Загрузить таблицу совместимости детерминативов (det_compat.txt).
* \param [in] filePath Путь к файлу.
* \param [out] target Хеш-таблица для заполнения.
* \return true при успехе, false если файл недоступен.
*
* Формат: D_LEMMA\tCONDITIONS\tCORRECTION. D_LEMMA приводится к нижнему
* регистру. "-" в колонке CORRECTION означает пустое исправление (удалить).
*/
bool loadDetCompat(const QString& filePath, QHash<QString, QList<DetCompatEntry>>& target)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#')) {
            QStringList cols = line.split('\t');
            if (cols.size() == 3) {
                NumberCondition number = NumberCondition::Any;
                CountabilityCondition countability = CountabilityCondition::Any;
                if (parseDetCompatConditions(cols[1], number, countability)) {
                    DetCompatEntry entry;
                    entry.number = number;
                    entry.countability = countability;
                    entry.correction = cols[2];
                    target[cols[0].toLower()].append(entry);
                }
            }
        }
    }
    return true;
}

/*!
* \brief Загрузить таблицу глагольного управления (verb_prep.txt).
* \param [in] filePath Путь к файлу.
* \param [out] target Хеш-таблица для заполнения.
* \return true при успехе, false если файл недоступен.
*
* Формат: VERB\twrong_prep\tcorrect_prep. VERB приводится к нижнему регистру.
* "-" в wrong_prep означает вставку предлога (InsertPrep).
* "-to" в correct_prep означает вставку "to" (InsertPrep).
* "-" в correct_prep означает удаление предлога (DeletePrep).
* Иначе — замена предлога (ReplacePrep).
*/
bool loadVerbPrep(const QString& filePath, QHash<QString, QSet<VerbPrepEntry>>& target)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#')) {
            QStringList cols = line.split('\t');
            if (cols.size() == 3) {
                VerbPrepEntry entry;
                QString wrongPrep = cols[1].trimmed().toLower();
                QString correctPrep = cols[2].trimmed().toLower();

                if (wrongPrep == "-") {
                    // Вставка предлога
                    entry.wrongPrep = std::nullopt;
                    entry.action = VerbPrepAction::InsertPrep;
                    if (correctPrep.startsWith('+')) {
                        entry.prep = correctPrep.mid(1);
                    } else {
                        entry.prep = correctPrep;
                    }
                } else if (correctPrep == "-") {
                    // Удаление предлога
                    entry.wrongPrep = wrongPrep;
                    entry.action = VerbPrepAction::DeletePrep;
                    entry.prep = std::nullopt;
                } else {
                    // Замена предлога
                    entry.wrongPrep = wrongPrep;
                    entry.action = VerbPrepAction::ReplacePrep;
                    if (correctPrep.startsWith('+')) {
                        entry.prep = correctPrep.mid(1);
                    } else {
                        entry.prep = correctPrep;
                    }
                }

                target[cols[0].toLower()].insert(entry);
            }
        }
    }
    return true;
}

/*!
* \brief Загрузить фонетический словарь CMUdict (cmudict.txt).
* \param [in] filePath Путь к файлу.
* \param [out] target Хеш-таблица для заполнения.
* \return true при успехе, false если файл недоступен.
*
* Формат: слово и фонемы разделены пробелами, фонемы — латинские буквы
* с цифрой ударения (0/1/2). Слово приводится к нижнему регистру.
* Фонемы сохраняются без изменений.
*/
bool loadCmuDict(const QString& filePath, QHash<QString, QStringList>& target)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith(';') && !line.startsWith('#')) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                const QString word = parts.takeFirst().toLower();
                target.insert(word, parts);
            }
        }
    }
    return true;
}

} // namespace

std::pair<RuleResources, QList<QString>> loadResources(const QString& listsDir)
{
    RuleResources res;
    QList<QString> warnings;

    auto tryLoad = [&](const QString& fileName, auto& target, auto loader) {
        QString path = QDir(listsDir).filePath(fileName);
        if (!loader(path, target))
            warnings.append(QStringLiteral("Не удалось загрузить %1").arg(fileName));
    };

    tryLoad("geo_the.txt",         res.geoThe,         loadWordSet);
    tryLoad("adj_requires_the.txt", res.adjRequiresThe, loadWordSet);
    tryLoad("languages.txt",       res.languages,       loadWordSet);
    tryLoad("sports.txt",          res.sports,          loadWordSet);
    tryLoad("meals.txt",           res.meals,           loadWordSet);
    tryLoad("titles.txt",          res.titles,          loadWordSet);
    tryLoad("uncountable.txt",     res.uncountable,     loadWordSet);
    tryLoad("propn_with_the.txt",  res.propnThe,        loadWordSet);
    tryLoad("classifiers.txt",     res.classifiers,     loadWordSet);
    tryLoad("time_units.txt",      res.timeUnits,       loadWordSet);
    tryLoad("activity_verbs.txt",  res.activityVerbs,   loadWordSet);
    tryLoad("positions.txt",       res.positions,       loadWordSet);
    tryLoad("durations.txt",       res.durations,       loadWordSet);
    tryLoad("past_forms.txt",      res.pastForms,       loadPastForms);
    tryLoad("det_compat.txt",      res.detCompat,       loadDetCompat);
    tryLoad("verb_prep.txt",       res.verbPrep,        loadVerbPrep);
    tryLoad("cmudict.txt",         res.cmudict,         loadCmuDict);

    return {std::move(res), std::move(warnings)};
}