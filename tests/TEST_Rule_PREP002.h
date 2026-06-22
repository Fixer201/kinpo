/*!
* \file TEST_Rule_PREP002.h
* \brief Объявление тестов для правила PREP-002 (раздел 6.50–6.51).
*
* Проверяет правило "for ↔ since":
*  — for перед моментом времени в конструкции Perfect → since
*  — since перед длительностью → for
*/

#ifndef TEST_RULE_PREP002_H
#define TEST_RULE_PREP002_H

#include <QObject>

class TEST_Rule_PREP002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP002();
    ~TEST_Rule_PREP002();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_PREP002_H