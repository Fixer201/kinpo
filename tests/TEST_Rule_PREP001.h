/*!
* \file TEST_Rule_PREP001.h
* \brief Объявление тестового класса для правила PREP-001.
*
* Проверяет правило «Неверный временной предлог».
*/

#ifndef TEST_RULE_PREP_001_H
#define TEST_RULE_PREP_001_H

#include <QObject>

/*!
* \class TEST_Rule_PREP001
* \brief Тестовый класс для PREP-001 (раздел 6.43–6.49 тестов_v3.md).
*/
class TEST_Rule_PREP001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_PREP001();
    ~TEST_Rule_PREP001();

private slots:
    void TestRule_data(); ///< DDT данные PREP-001
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_PREP_001_H
