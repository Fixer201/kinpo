/*!
* \file TEST_Rule_DET005.h
* \brief Объявление тестового класса для правила DET-005.
*
* Проверяет правило «Множественное число неисчисляемого».
*/

#ifndef TEST_RULE_DET_005_H
#define TEST_RULE_DET_005_H

#include <QObject>

/*!
* \class TEST_Rule_DET005
* \brief Тестовый класс для DET-005 (раздел 6.42 тестов_v3.md).
*/
class TEST_Rule_DET005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET005();
    ~TEST_Rule_DET005();

private slots:
    void TestRule_data(); ///< DDT данные DET-005
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_DET_005_H
