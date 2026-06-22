/*!
* \file TEST_Rule_ART005a.h
* \brief Объявление тестового класса для правила ART-005a.
*/

#ifndef TEST_RULE_ART005A_H
#define TEST_RULE_ART005A_H

#include <QObject>

class TEST_Rule_ART005a : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART005a();
    ~TEST_Rule_ART005a();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_ART005A_H