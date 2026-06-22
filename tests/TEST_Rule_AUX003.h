/*!
* \file TEST_Rule_AUX003.h
* \brief Объявление тестового класса для правила AUX-003.
*/

#ifndef TEST_RULE_AUX003_H
#define TEST_RULE_AUX003_H

#include <QObject>

class TEST_Rule_AUX003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX003();
    ~TEST_Rule_AUX003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX003_H