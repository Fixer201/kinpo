/*!
* \file TEST_Rule_PREP004.h
* \brief Объявление тестов для правила PREP-004 (раздел 6.54–6.55).
*
* Проверяет правило "Perfect + ago":
*  — ago + Present Perfect (has went) → ошибка, требуется Past Simple
*  — ago + правильный глагол (has telephoned) → ошибка (fallback)
*/

#ifndef TEST_RULE_PREP004_H
#define TEST_RULE_PREP004_H

#include <QObject>

class TEST_Rule_PREP004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP004();
    ~TEST_Rule_PREP004();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_PREP004_H