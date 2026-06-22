/*!
* \file TEST_Rule_AUX004.h
* \brief Объявление тестового класса для правила AUX-004.
*/

#ifndef TEST_RULE_AUX004_H
#define TEST_RULE_AUX004_H

#include <QObject>

class TEST_Rule_AUX004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX004();
    ~TEST_Rule_AUX004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX004_H