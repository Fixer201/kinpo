/*!
* \file TEST_Rule_AUX001.h
* \brief Объявление тестового класса для правила AUX-001.
*/

#ifndef TEST_RULE_AUX001_H
#define TEST_RULE_AUX001_H

#include <QObject>

class TEST_Rule_AUX001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX001();
    ~TEST_Rule_AUX001();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX001_H