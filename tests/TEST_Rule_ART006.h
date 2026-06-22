/*!
* \file TEST_Rule_ART006.h
* \brief Объявление тестового класса для правила ART-006.
*/

#ifndef TEST_RULE_ART006_H
#define TEST_RULE_ART006_H

#include <QObject>

class TEST_Rule_ART006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART006();
    ~TEST_Rule_ART006();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_ART006_H