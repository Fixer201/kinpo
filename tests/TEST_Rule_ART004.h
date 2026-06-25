/*!
* \file TEST_Rule_ART004.h
* \brief Объявление тестового класса для правила ART-004.
*
* Проверяет правило «Пропущен the перед географическим названием».
*/

#ifndef TEST_RULE_ART004_H
#define TEST_RULE_ART004_H

#include <QObject>

/*!
* \class TEST_Rule_ART004
* \brief Тестовый класс для ART-004 (раздел 6.17 тестов_v3.md).
*/
class TEST_Rule_ART004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART004();
    ~TEST_Rule_ART004();

private slots:
    void TestRule_data(); ///< DDT данные ART-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_ART004_H
