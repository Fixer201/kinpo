/*!
* \file TEST_Rule_ART003.h
* \brief Объявление тестового класса для правила ART-003.
*/

#ifndef TEST_RULE_ART003_H
#define TEST_RULE_ART003_H

#include <QObject>

class TEST_Rule_ART003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART003();
    ~TEST_Rule_ART003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_ART003_H