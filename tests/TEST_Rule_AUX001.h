/*!
* \file TEST_Rule_AUX001.h
* \brief Объявление тестового класса для правила AUX-001.
*
* Проверяет правило «Избыточный do при модальном».
*/

#ifndef TEST_RULE_AUX_001_H
#define TEST_RULE_AUX_001_H

#include <QObject>

/*!
* \class TEST_Rule_AUX001
* \brief Тестовый класс для AUX-001 (раздел 6.77–6.78 тестов_v3.md).
*/
class TEST_Rule_AUX001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX001();
    ~TEST_Rule_AUX001();

private slots:
    void TestRule_data(); ///< DDT данные AUX-001
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_001_H
