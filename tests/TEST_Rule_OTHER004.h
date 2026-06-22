/*!
* \file TEST_Rule_OTHER004.h
* \brief Объявление тестового класса для правила OTHER-004.
*
* Правило проверяет неверный падеж местоимения: объектная форма
* в позиции подлежащего и именительная в позиции дополнения.
*/

#ifndef TEST_RULE_OTHER004_H
#define TEST_RULE_OTHER004_H

#include <QObject>

/*! \brief Тесты правила OTHER-004 (раздел 6.93–6.95). */
class TEST_Rule_OTHER004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER004();
    ~TEST_Rule_OTHER004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER004_H