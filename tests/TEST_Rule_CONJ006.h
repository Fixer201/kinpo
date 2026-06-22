/*!
* \file TEST_Rule_CONJ006.h
* \brief Объявление тестового класса для правила CONJ-006.
*/

#ifndef TEST_RULE_CONJ006_H
#define TEST_RULE_CONJ006_H

#include <QObject>

class TEST_Rule_CONJ006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ006();
    ~TEST_Rule_CONJ006();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_CONJ006_H