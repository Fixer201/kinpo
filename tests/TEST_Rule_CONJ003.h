/*!
* \file TEST_Rule_CONJ003.h
* \brief Объявление тестов для правила CONJ-003 (раздел 6.66, 6.99–6.100).
*
* Проверяет правило "although ↔ despite":
*  — although перед именной группой → despite
*  — although перед именной группой с притяжательным → despite
*  — although перед клаузой → NO ERRORS
*/

#ifndef TEST_RULE_CONJ003_H
#define TEST_RULE_CONJ003_H

#include <QObject>

class TEST_Rule_CONJ003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ003();
    ~TEST_Rule_CONJ003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ003_H