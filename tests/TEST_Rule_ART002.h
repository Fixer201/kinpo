/*!
* \file TEST_Rule_ART002.h
* \brief Объявление тестового класса для правил ART-002 и ART-002a.
*/

#ifndef TEST_RULE_ART002_H
#define TEST_RULE_ART002_H

#include <QObject>

class TEST_Rule_ART002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART002();
    ~TEST_Rule_ART002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_ART002_H