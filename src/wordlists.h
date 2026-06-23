/*!
* \file wordlists.h
* \brief Интерфейс модуля загрузки словарей.
*
* Содержит функцию loadResources для загрузки словарей из директории
* listsDir в структуру RuleResources.
*/

#pragma once

#include "datamodel.h"
#include <QString>
#include <QList>
#include <QTextStream>
#include <utility>

void setUtf8Encoding(QTextStream& stream);

/*!
* \brief Загрузить все словари из указанной директории.
* \param [in] listsDir Путь к директории со словарями.
* \return Пара: RuleResources с загруженными словарями и список предупреждений
*         о файлах, которые не удалось загрузить.
*
* Простые списки слов загружаются в нижнем регистре для case-insensitive
* сравнения. Строки, начинающиеся с '#', пропускаются как комментарии.
* Пустые строки игнорируются. Если файл отсутствует или недоступен для
* чтения, соответствующее множество остаётся пустым, а в список
* предупреждений добавляется сообщение.
*/
std::pair<RuleResources, QList<QString>> loadResources(const QString& listsDir);