/*!
* \file TEST_Rule_DET004.h
* \brief Объявление тестового класса для правила DET-004.
*/

#ifndef TEST_RULE_DET004_H
#define TEST_RULE_DET004_H

#include <QObject>

class TEST_Rule_DET004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET004();
    ~TEST_Rule_DET004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_DET004_H