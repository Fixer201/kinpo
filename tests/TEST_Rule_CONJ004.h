/*!
* \file TEST_Rule_CONJ004.h
* \brief Объявление тестового класса для правила CONJ-004.
*
* Проверяет правило «Неверный коррелят».
*/

#ifndef TEST_RULE_CONJ_004_H
#define TEST_RULE_CONJ_004_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ004
* \brief Тестовый класс для CONJ-004 (раздел 6.67–6.72 тестов_v3.md).
*/
class TEST_Rule_CONJ004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ004();
    ~TEST_Rule_CONJ004();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_004_H
