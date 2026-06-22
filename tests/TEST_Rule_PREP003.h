/*!
* \file TEST_Rule_PREP003.h
* \brief Объявление тестов для правила PREP-003 (раздел 6.52–6.53).
*
* Проверяет правило "while ↔ during":
*  — while перед именной группой → during
*  — while перед клаузой (VERB в зависимых HEAD) → NO ERRORS
*/

#ifndef TEST_RULE_PREP003_H
#define TEST_RULE_PREP003_H

#include <QObject>

class TEST_Rule_PREP003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP003();
    ~TEST_Rule_PREP003();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_PREP003_H