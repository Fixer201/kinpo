/*!
* \file TEST_ValidateSentenceStructure.h
* \brief Объявление тестового класса для функции validateSentenceStructure.
*
* Тестовый класс на базе QtTest, использующий DDT-подход:
* TestValidateSentenceStructure_data() задаёт таблицу входных параметров,
* TestValidateSentenceStructure() выполняет проверку для каждой строки.
*/

#ifndef TEST_ValidateSentenceStructure_H
#define TEST_ValidateSentenceStructure_H

#include <QObject>

/*!
* \class TEST_ValidateSentenceStructure
* \brief Тестовый класс для validateSentenceStructure.
*/
class TEST_ValidateSentenceStructure : public QObject
{
    Q_OBJECT
public:
    TEST_ValidateSentenceStructure();
    ~TEST_ValidateSentenceStructure();

private slots:
    void TestValidateSentenceStructure_data(); ///< Подготовка таблицы данных DDT.
    void TestValidateSentenceStructure();      ///< Выполнение проверок по таблице.
};

#endif // TEST_ValidateSentenceStructure_H
