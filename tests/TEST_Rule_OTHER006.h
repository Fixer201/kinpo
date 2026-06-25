/*!
* \file TEST_Rule_OTHER006.h
* \brief Объявление тестового класса для правила OTHER-006.
*
* Проверяет правило «better then → than».
*/

#ifndef TEST_RULE_OTHER_006_H
#define TEST_RULE_OTHER_006_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER006
* \brief Тестовый класс для OTHER-006 (раздел 6.98 тестов_v3.md).
*/
class TEST_Rule_OTHER006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER006();
    ~TEST_Rule_OTHER006();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-006
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_006_H
