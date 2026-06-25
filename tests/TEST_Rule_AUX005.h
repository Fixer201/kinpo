/*!
* \file TEST_Rule_AUX005.h
* \brief Объявление тестового класса для правила AUX-005.
*
* Проверяет правило «Пропущен have после модального».
*/

#ifndef TEST_RULE_AUX_005_H
#define TEST_RULE_AUX_005_H

#include <QObject>

/*!
* \class TEST_Rule_AUX005
* \brief Тестовый класс для AUX-005 (раздел 6.83–6.84 тестов_v3.md).
*/
class TEST_Rule_AUX005 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX005();
    ~TEST_Rule_AUX005();

private slots:
    void TestRule_data(); ///< DDT данные AUX-005
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_005_H
