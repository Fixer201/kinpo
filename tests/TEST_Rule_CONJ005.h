/*!
* \file TEST_Rule_CONJ005.h
* \brief Объявление тестового класса для правила CONJ-005.
*
* Проверяет правило «if ↔ whether».
*/

#ifndef TEST_RULE_CONJ_005_H
#define TEST_RULE_CONJ_005_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ005
* \brief Тестовый класс для CONJ-005 (раздел 6.73–6.74 тестов_v3.md).
*/
class TEST_Rule_CONJ005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ005();
    ~TEST_Rule_CONJ005();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-005
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_005_H
