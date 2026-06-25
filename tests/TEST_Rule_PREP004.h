/*!
* \file TEST_Rule_PREP004.h
* \brief Объявление тестового класса для правила PREP-004.
*
* Проверяет правило «Perfect + ago».
*/

#ifndef TEST_RULE_PREP_004_H
#define TEST_RULE_PREP_004_H

#include <QObject>

/*!
* \class TEST_Rule_PREP004
* \brief Тестовый класс для PREP-004 (раздел 6.54–6.55 тестов_v3.md).
*/
class TEST_Rule_PREP004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP004();
    ~TEST_Rule_PREP004();

private slots:
    void TestRule_data(); ///< DDT данные PREP-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_PREP_004_H
