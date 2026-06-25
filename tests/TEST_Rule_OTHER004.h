/*!
* \file TEST_Rule_OTHER004.h
* \brief Объявление тестового класса для правила OTHER-004.
*
* Проверяет правило «Неверный падеж местоимения».
*/

#ifndef TEST_RULE_OTHER_004_H
#define TEST_RULE_OTHER_004_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER004
* \brief Тестовый класс для OTHER-004 (раздел 6.93–6.95 тестов_v3.md).
*/
class TEST_Rule_OTHER004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER004();
    ~TEST_Rule_OTHER004();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_004_H
