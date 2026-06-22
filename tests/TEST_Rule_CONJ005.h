/*!
* \file TEST_Rule_CONJ005.h
* \brief Объявление тестового класса для правила CONJ-005.
*/

#ifndef TEST_RULE_CONJ005_H
#define TEST_RULE_CONJ005_H

#include <QObject>

class TEST_Rule_CONJ005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ005();
    ~TEST_Rule_CONJ005();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ005_H