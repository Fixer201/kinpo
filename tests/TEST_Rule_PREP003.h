/*!
* \file TEST_Rule_PREP003.h
* \brief Объявление тестового класса для правила PREP-003.
*
* Проверяет правило «while ↔ during».
*/

#ifndef TEST_RULE_PREP_003_H
#define TEST_RULE_PREP_003_H

#include <QObject>

/*!
* \class TEST_Rule_PREP003
* \brief Тестовый класс для PREP-003 (раздел 6.52–6.53 тестов_v3.md).
*/
class TEST_Rule_PREP003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP003();
    ~TEST_Rule_PREP003();

private slots:
    void TestRule_data(); ///< DDT данные PREP-003
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_PREP_003_H
