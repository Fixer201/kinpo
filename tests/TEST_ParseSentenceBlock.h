/*!
* \file TEST_ParseSentenceBlock.h
* \brief Объявление тестового класса для функции parseSentenceBlock.
*
* Тестовый класс на базе QtTest, использующий DDT-подход:
* TestParseSentenceBlock_data() задает таблицу входных параметров,
* TestParseSentenceBlock() выполняет проверку для каждой строки.
*/

#ifndef TEST_ParseSentenceBlock_H
#define TEST_ParseSentenceBlock_H

#include <QObject>

/*!
* \class TEST_ParseSentenceBlock
* \brief Тестовый класс для parseSentenceBlock.
*/
class TEST_ParseSentenceBlock : public QObject
{
    Q_OBJECT
public:
    TEST_ParseSentenceBlock();
    ~TEST_ParseSentenceBlock();

private slots:
    void TestParseSentenceBlock_data(); ///< Подготовка таблицы данных DDT.
    void TestParseSentenceBlock();      ///< Выполнение проверок по таблице.
};

#endif // TEST_ParseSentenceBlock_H
