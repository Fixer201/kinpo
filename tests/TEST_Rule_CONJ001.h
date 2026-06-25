/*!
* \file TEST_Rule_CONJ001.h
* \brief Объявление тестового класса для правила CONJ-001.
*
* Проверяет правило «nor без отрицания».
*/

#ifndef TEST_RULE_CONJ_001_H
#define TEST_RULE_CONJ_001_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ001
* \brief Тестовый класс для CONJ-001 (раздел 6.60–6.63 тестов_v3.md).
*/
class TEST_Rule_CONJ001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ001();
    ~TEST_Rule_CONJ001();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-001
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_001_H
