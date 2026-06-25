/*!
* \file TEST_Rule_DET004.h
* \brief Объявление тестового класса для правила DET-004.
*
* Проверяет правило «Омофон в притяжательной позиции».
*/

#ifndef TEST_RULE_DET_004_H
#define TEST_RULE_DET_004_H

#include <QObject>

/*!
* \class TEST_Rule_DET004
* \brief Тестовый класс для DET-004 (раздел 6.40–6.41 тестов_v3.md).
*/
class TEST_Rule_DET004 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET004();
    ~TEST_Rule_DET004();

private slots:
    void TestRule_data(); ///< DDT данные DET-004
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_DET_004_H
