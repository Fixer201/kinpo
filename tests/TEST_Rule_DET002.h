/*!
* \file TEST_Rule_DET002.h
* \brief Объявление тестового класса для правила DET-002.
*/

#ifndef TEST_RULE_DET002_H
#define TEST_RULE_DET002_H

#include <QObject>

class TEST_Rule_DET002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET002();
    ~TEST_Rule_DET002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_DET002_H