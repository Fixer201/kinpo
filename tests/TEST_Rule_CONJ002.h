/*!
* \file TEST_Rule_CONJ002.h
* \brief Объявление тестового класса для правила CONJ-002.
*
* Проверяет правило «because ↔ because of».
*/

#ifndef TEST_RULE_CONJ_002_H
#define TEST_RULE_CONJ_002_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ002
* \brief Тестовый класс для CONJ-002 (раздел 6.64–6.65 тестов_v3.md).
*/
class TEST_Rule_CONJ002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ002();
    ~TEST_Rule_CONJ002();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-002
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_002_H
