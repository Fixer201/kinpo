/*!
* \file TEST_Rule_AUX003.h
* \brief Объявление тестового класса для правила AUX-003.
*
* Проверяет правило «Избыточная to после модального».
*/

#ifndef TEST_RULE_AUX_003_H
#define TEST_RULE_AUX_003_H

#include <QObject>

/*!
* \class TEST_Rule_AUX003
* \brief Тестовый класс для AUX-003 (раздел 6.81 тестов_v3.md).
*/
class TEST_Rule_AUX003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_AUX003();
    ~TEST_Rule_AUX003();

private slots:
    void TestRule_data(); ///< DDT данные AUX-003
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_AUX_003_H
