/*!
* \file TEST_Rule_ART002.h
* \brief Объявление тестового класса для правил ART-002 и ART-002a.
*
* Проверяет срабатывание и исключения правила «Пропущен the перед ADJ».
*/

#ifndef TEST_RULE_ART002_H
#define TEST_RULE_ART002_H

#include <QObject>

/*!
* \class TEST_Rule_ART002
* \brief Тестовый класс для ART-002/ART-002a (раздел 6.7–6.12 тестов_v3.md).
*/
class TEST_Rule_ART002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART002();
    ~TEST_Rule_ART002();

private slots:
    void TestRule_data(); ///< DDT данные ART-002/ART-002a
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_ART002_H