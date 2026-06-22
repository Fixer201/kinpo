/*!
* \file TEST_Rule_OTHER001.h
* \brief Объявление тестового класса для правила OTHER-001.
*/

#ifndef TEST_RULE_OTHER001_H
#define TEST_RULE_OTHER001_H

#include <QObject>

class TEST_Rule_OTHER001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER001();
    ~TEST_Rule_OTHER001();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER001_H