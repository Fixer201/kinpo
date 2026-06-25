/*!
* \file TEST_Rule_OTHER003.h
* \brief Объявление тестового класса для правила OTHER-003.
*
* Проверяет правило «much/very перед ADJ».
*/

#ifndef TEST_RULE_OTHER_003_H
#define TEST_RULE_OTHER_003_H

#include <QObject>

/*!
* \class TEST_Rule_OTHER003
* \brief Тестовый класс для OTHER-003 (раздел 6.91–6.92 тестов_v3.md).
*/
class TEST_Rule_OTHER003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER003();
    ~TEST_Rule_OTHER003();

private slots:
    void TestRule_data(); ///< DDT данные OTHER-003
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_OTHER_003_H
