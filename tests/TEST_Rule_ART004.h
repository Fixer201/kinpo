/*!
* \file TEST_Rule_ART004.h
* \brief Объявление тестового класса для правила ART-004.
*/

#ifndef TEST_RULE_ART004_H
#define TEST_RULE_ART004_H

#include <QObject>

class TEST_Rule_ART004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART004();
    ~TEST_Rule_ART004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_ART004_H