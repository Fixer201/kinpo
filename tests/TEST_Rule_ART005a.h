/*!
* \file TEST_Rule_ART005a.h
* \brief Объявление тестового класса для правила ART-005a.
*
* Проверяет правило «Лишний the перед титулом + имя».
*/

#ifndef TEST_RULE_ART005A_H
#define TEST_RULE_ART005A_H

#include <QObject>

/*!
* \class TEST_Rule_ART005a
* \brief Тестовый класс для ART-005a (раздел 6.18–6.20 тестов_v3.md).
*/
class TEST_Rule_ART005a : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART005a();
    ~TEST_Rule_ART005a();

private slots:
    void TestRule_data(); ///< DDT данные ART-005a
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_ART005A_H
