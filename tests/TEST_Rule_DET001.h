/*!
* \file TEST_Rule_DET001.h
* \brief Объявление тестового класса для правила DET-001.
*
* Проверяет правило «Несовместимость det/quant + NOUN».
*/

#ifndef TEST_RULE_DET_001_H
#define TEST_RULE_DET_001_H

#include <QObject>

/*!
* \class TEST_Rule_DET001
* \brief Тестовый класс для DET-001 (раздел 6.27–6.33 тестов_v3.md).
*/
class TEST_Rule_DET001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_DET001();
    ~TEST_Rule_DET001();

private slots:
    void TestRule_data(); ///< DDT данные DET-001
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_DET_001_H
