/*!
* \file TEST_Rule_AUX006.h
* \brief Объявление тестового класса для правила AUX-006.
*/

#ifndef TEST_RULE_AUX006_H
#define TEST_RULE_AUX006_H

#include <QObject>

class TEST_Rule_AUX006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX006();
    ~TEST_Rule_AUX006();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_AUX006_H