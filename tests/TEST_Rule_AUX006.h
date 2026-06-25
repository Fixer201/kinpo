/*!
* \file TEST_Rule_AUX006.h
* \brief Объявление тестового класса для правила AUX-006.
*
* Проверяет правило «Избыточная to после let».
*/

#ifndef TEST_RULE_AUX_006_H
#define TEST_RULE_AUX_006_H

#include <QObject>

/*!
* \class TEST_Rule_AUX006
* \brief Тестовый класс для AUX-006 (раздел 6.85–6.86 тестов_v3.md).
*/
class TEST_Rule_AUX006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX006();
    ~TEST_Rule_AUX006();

private slots:
    void TestRule_data(); ///< DDT данные AUX-006
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_006_H
