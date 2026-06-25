/*!
* \file TEST_Rule_DET002.h
* \brief Объявление тестового класса для правила DET-002.
*
* Проверяет правило «Два центральных детерминатива».
*/

#ifndef TEST_RULE_DET_002_H
#define TEST_RULE_DET_002_H

#include <QObject>

/*!
* \class TEST_Rule_DET002
* \brief Тестовый класс для DET-002 (раздел 6.34–6.36 тестов_v3.md).
*/
class TEST_Rule_DET002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET002();
    ~TEST_Rule_DET002();

private slots:
    void TestRule_data(); ///< DDT данные DET-002
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_DET_002_H
