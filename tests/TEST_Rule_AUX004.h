/*!
* \file TEST_Rule_AUX004.h
* \brief Объявление тестового класса для правила AUX-004.
*
* Проверяет правило «Два модальных у одного VERB».
*/

#ifndef TEST_RULE_AUX_004_H
#define TEST_RULE_AUX_004_H

#include <QObject>

/*!
* \class TEST_Rule_AUX004
* \brief Тестовый класс для AUX-004 (раздел 6.82 тестов_v3.md).
*/
class TEST_Rule_AUX004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX004();
    ~TEST_Rule_AUX004();

private slots:
    void TestRule_data(); ///< DDT данные AUX-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_004_H
