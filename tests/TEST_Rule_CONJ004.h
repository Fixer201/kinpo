/*!
* \file TEST_Rule_CONJ004.h
* \brief Объявление тестового класса для правила CONJ-004.
*/

#ifndef TEST_RULE_CONJ004_H
#define TEST_RULE_CONJ004_H

#include <QObject>

class TEST_Rule_CONJ004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ004();
    ~TEST_Rule_CONJ004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ004_H