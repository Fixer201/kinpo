/*!
* \file TEST_Rule_AUX005.h
* \brief Объявление тестового класса для правила AUX-005.
*/

#ifndef TEST_RULE_AUX005_H
#define TEST_RULE_AUX005_H

#include <QObject>

class TEST_Rule_AUX005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX005();
    ~TEST_Rule_AUX005();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX005_H