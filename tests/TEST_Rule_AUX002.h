/*!
* \file TEST_Rule_AUX002.h
* \brief Объявление тестового класса для правила AUX-002.
*
* Проверяет правило «Неверная форма после do/have».
*/

#ifndef TEST_RULE_AUX_002_H
#define TEST_RULE_AUX_002_H

#include <QObject>

/*!
* \class TEST_Rule_AUX002
* \brief Тестовый класс для AUX-002 (раздел 6.79–6.80 тестов_v3.md).
*/
class TEST_Rule_AUX002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX002();
    ~TEST_Rule_AUX002();

private slots:
    void TestRule_data(); ///< DDT данные AUX-002
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_002_H
