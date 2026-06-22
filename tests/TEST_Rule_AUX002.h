/*!
* \file TEST_Rule_AUX002.h
* \brief Объявление тестового класса для правила AUX-002.
*/

#ifndef TEST_RULE_AUX002_H
#define TEST_RULE_AUX002_H

#include <QObject>

class TEST_Rule_AUX002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX002();
    ~TEST_Rule_AUX002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX002_H