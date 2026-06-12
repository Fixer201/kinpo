/*!
* \file TEST_CheckSentence.h
* \brief Объявление тестового класса для функции checkSentence.
*/

#ifndef TEST_CHECKSENTENCE_H
#define TEST_CHECKSENTENCE_H

#include <QObject>

/*!
* \class TEST_CheckSentence
* \brief Тестовый класс для checkSentence (раздел 4 тестов_v3.md).
*/
class TEST_CheckSentence : public QObject
{
    Q_OBJECT
public:
    TEST_CheckSentence();
    ~TEST_CheckSentence();

private slots:
    void TestCheckSentence_data();  ///\u003c DDT: раздел 4
    void TestCheckSentence();         ///\u003c Выполнение проверок
};

#endif // TEST_CHECKSENTENCE_H