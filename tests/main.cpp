/*!
* \file main.cpp
* \brief Точка входа тестового приложения.
*
* Собирает и запускает все тестовые классы через QTest::qExec.
* Каждый новый класс-тест требует добавления include и вызова qExec.
*/

#include <QtTest>

#include "TEST_ParseSentenceBlock.h"
#include "TEST_ValidateSentenceStructure.h"

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_ParseSentenceBlock, argc, argv);
    status |= QTest::qExec(new TEST_ValidateSentenceStructure, argc, argv);
    return status;
}
