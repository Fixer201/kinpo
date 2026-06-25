/*!
* \file TEST_Rule_OTHER005.h
* \brief Объявление тестового класса для правила OTHER-005.
*
* Проверяет правило «Двойное отрицание».
*/

#ifndef TEST_RULE_OTHER_005_H
#define TEST_RULE_OTHER_005_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER005
* \brief Тестовый класс для OTHER-005 (раздел 6.96–6.97 тестов_v3.md).
*/
class TEST_Rule_OTHER005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER005();
    ~TEST_Rule_OTHER005();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-005
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_005_H
