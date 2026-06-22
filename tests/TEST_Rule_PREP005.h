/*!
* \file TEST_Rule_PREP005.h
* \brief Объявление тестов для правила PREP-005 (раздел 6.56–6.59).
*
* Проверяет правило "Неверный предлог при глаголе":
*  — лишний предлог (discuss about → удалить)
*  — пропущенный предлог (explain me → вставить to)
*  — неверный предлог (depend of → заменить на on)
*  — корректный предлог не срабатывает (explain about it)
*/

#ifndef TEST_RULE_PREP005_H
#define TEST_RULE_PREP005_H

#include <QObject>

class TEST_Rule_PREP005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP005();
    ~TEST_Rule_PREP005();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_PREP005_H