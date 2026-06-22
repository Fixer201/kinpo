/*!
* \file TEST_Rule_OTHER003.h
* \brief Объявление тестового класса для правила OTHER-003.
*/

#ifndef TEST_RULE_OTHER003_H
#define TEST_RULE_OTHER003_H

#include <QObject>

class TEST_Rule_OTHER003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER003();
    ~TEST_Rule_OTHER003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER003_H