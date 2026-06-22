/*!
* \file TEST_Rule_DET003.h
* \brief Объявление тестового класса для правила DET-003.
*/

#ifndef TEST_RULE_DET003_H
#define TEST_RULE_DET003_H

#include <QObject>

class TEST_Rule_DET003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET003();
    ~TEST_Rule_DET003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_DET003_H