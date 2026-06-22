/*!
* \file TEST_Rule_DET005.h
* \brief Объявление тестов для правила DET-005 (раздел 6.42).
*
* Проверяет правило "Множественное число неисчисляемого":
*  — informations → information (NOUN, Number=Plur, LEMMA ∈ uncountable.txt)
*/

#ifndef TEST_RULE_DET005_H
#define TEST_RULE_DET005_H

#include <QObject>

class TEST_Rule_DET005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET005();
    ~TEST_Rule_DET005();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_DET005_H