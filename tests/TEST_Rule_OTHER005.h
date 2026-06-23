/*!
* \file TEST_Rule_OTHER005.h
* \brief Объявление тестового класса для правила OTHER-005.
*
* Правило проверяет двойное отрицание: при наличии у VERB прямого
* зависимого с Polarity=Neg и прямого зависимого-отрицательного
* местоимения/наречия последнее заменяется на не-отрицательную форму.
*/

#ifndef TEST_RULE_OTHER005_H
#define TEST_RULE_OTHER005_H

#include <QObject>

/*! \brief Тесты правила OTHER-005 (раздел 6.96–6.97). */
class TEST_Rule_OTHER005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER005();
    ~TEST_Rule_OTHER005();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER005_H