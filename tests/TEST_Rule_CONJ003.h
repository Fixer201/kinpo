/*!
* \file TEST_Rule_CONJ003.h
* \brief Объявление тестового класса для правила CONJ-003.
*
* Проверяет правило «although ↔ despite».
*/

#ifndef TEST_RULE_CONJ_003_H
#define TEST_RULE_CONJ_003_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ003
* \brief Тестовый класс для CONJ-003 (раздел 6.66, 6.99–6.100 тестов_v3.md).
*/
class TEST_Rule_CONJ003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ003();
    ~TEST_Rule_CONJ003();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-003
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_003_H
