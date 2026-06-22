/*!
* \file TEST_Rule_DET001.h
* \brief Объявление тестового класса для правила DET-001.
*/

#ifndef TEST_RULE_DET001_H
#define TEST_RULE_DET001_H

#include <QObject>

class TEST_Rule_DET001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET001();
    ~TEST_Rule_DET001();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_DET001_H