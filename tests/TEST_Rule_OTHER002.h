/*!
* \file TEST_Rule_OTHER002.h
* \brief Объявление тестового класса для правила OTHER-002.
*
* Проверяет правило «good в наречной позиции».
*/

#ifndef TEST_RULE_OTHER_002_H
#define TEST_RULE_OTHER_002_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER002
* \brief Тестовый класс для OTHER-002 (раздел 6.89–6.90 тестов_v3.md).
*/
class TEST_Rule_OTHER002 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER002();
    ~TEST_Rule_OTHER002();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-002
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_002_H
