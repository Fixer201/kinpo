/*!
* \file TEST_Rule_PREP005.h
* \brief Объявление тестового класса для правила PREP-005.
*
* Проверяет правило «Неверный предлог при глаголе».
*/

#ifndef TEST_RULE_PREP_005_H
#define TEST_RULE_PREP_005_H

#include <QObject>

/*!
* \class TEST_Rule_PREP005
* \brief Тестовый класс для PREP-005 (раздел 6.56–6.59 тестов_v3.md).
*/
class TEST_Rule_PREP005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP005();
    ~TEST_Rule_PREP005();

private slots:
    void TestRule_data(); ///< DDT данные PREP-005
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_PREP_005_H
