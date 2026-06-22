/*!
* \file TEST_Rule_OTHER002.h
* \brief Объявление тестового класса для правила OTHER-002.
*/

#ifndef TEST_RULE_OTHER002_H
#define TEST_RULE_OTHER002_H

#include <QObject>

class TEST_Rule_OTHER002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER002();
    ~TEST_Rule_OTHER002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER002_H