/*!
* \file TEST_Rule_CONJ002.h
* \brief Объявление тестов для правила CONJ-002 (раздел 6.64–6.65).
*
* Проверяет правило "because ↔ because of":
*  — because перед именной группой → because of
*  — because of перед клаузой → because
*/

#ifndef TEST_RULE_CONJ002_H
#define TEST_RULE_CONJ002_H

#include <QObject>

class TEST_Rule_CONJ002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ002();
    ~TEST_Rule_CONJ002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ002_H