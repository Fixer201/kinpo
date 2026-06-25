/*!
* \file TEST_Rule_PREP002.h
* \brief Объявление тестового класса для правила PREP-002.
*
* Проверяет правило «for ↔ since».
*/

#ifndef TEST_RULE_PREP_002_H
#define TEST_RULE_PREP_002_H

#include <QObject>

/*!
* \class TEST_Rule_PREP002
* \brief Тестовый класс для PREP-002 (раздел 6.50–6.51 тестов_v3.md).
*/
class TEST_Rule_PREP002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP002();
    ~TEST_Rule_PREP002();

private slots:
    void TestRule_data(); ///< DDT данные PREP-002
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_PREP_002_H
