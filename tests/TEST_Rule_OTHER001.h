/*!
* \file TEST_Rule_OTHER001.h
* \brief Объявление тестового класса для правила OTHER-001.
*
* Проверяет правило «Двойное сравнение».
*/

#ifndef TEST_RULE_OTHER_001_H
#define TEST_RULE_OTHER_001_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER001
* \brief Тестовый класс для OTHER-001 (раздел 6.87–6.88 тестов_v3.md).
*/
class TEST_Rule_OTHER001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER001();
    ~TEST_Rule_OTHER001();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-001
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_001_H
