/*!
* \file TEST_Rule_PREP001.h
* \brief Объявление тестов для правила PREP-001 (раздел 6.43–6.49).
*
* Проверяет правило "Неверный временной предлог":
*  — день недели (in→on), месяц (on→in), часть дня (at→in), год (on→in),
*    ночь (in→at)
*  — корректный предлог не срабатывает (at 5 o'clock)
*  — исключение: compound с днём недели переводит часть дня в категорию on
*    (on Monday morning — NO ERRORS)
*/

#ifndef TEST_RULE_PREP001_H
#define TEST_RULE_PREP001_H

#include <QObject>

class TEST_Rule_PREP001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP001();
    ~TEST_Rule_PREP001();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_PREP001_H