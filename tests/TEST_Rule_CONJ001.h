/*!
* \file TEST_Rule_CONJ001.h
* \brief Объявление тестов для правила CONJ-001 (раздел 6.60–6.63).
*
* Проверяет правило "nor без отрицания":
*  — nor без отрицания → or
*  — nor с отрицанием (not) в предложении → NO ERRORS
*  — nor с отрицательным местоимением (none) → NO ERRORS
*  — neither...nor → NO ERRORS (коррелят)
*/

#ifndef TEST_RULE_CONJ001_H
#define TEST_RULE_CONJ001_H

#include <QObject>

class TEST_Rule_CONJ001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ001();
    ~TEST_Rule_CONJ001();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ001_H