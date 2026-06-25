/*!
* \file TEST_Rule_DET003.h
* \brief Объявление тестового класса для правила DET-003.
*
* Проверяет правило «Притяжательный омофон как подлежащее».
*/

#ifndef TEST_RULE_DET_003_H
#define TEST_RULE_DET_003_H

#include <QObject>

/*!
* \class TEST_Rule_DET003
* \brief Тестовый класс для DET-003 (раздел 6.37–6.39 тестов_v3.md).
*/
class TEST_Rule_DET003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET003();
    ~TEST_Rule_DET003();

private slots:
    void TestRule_data(); ///< DDT данные DET-003
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_DET_003_H
