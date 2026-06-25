/*!
* \file TEST_Rule_ART006.h
* \brief Объявление тестового класса для правила ART-006.
*
* Проверяет правило «Неверный a/an».
*/

#ifndef TEST_RULE_ART006_H
#define TEST_RULE_ART006_H

#include <QObject>

/*!
* \class TEST_Rule_ART006
* \brief Тестовый класс для ART-006 (раздел 6.21–6.26 тестов_v3.md).
*/
class TEST_Rule_ART006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART006();
    ~TEST_Rule_ART006();

private slots:
    void TestRule_data(); ///< DDT данные ART-006
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_ART006_H
