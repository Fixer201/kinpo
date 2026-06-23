/*!
* \file TEST_ParseCommandLine.h
* \brief Объявление тестового класса для функции parseCommandLine.
*
* Тестируется разбор аргументов командной строки в структуру RunConfig
* или Diagnostic{CliUsageError} при ошибке.
*/

#ifndef TEST_PARSECOMMANDLINE_H
#define TEST_PARSECOMMANDLINE_H

#include <QObject>

/*! \brief Тесты функции parseCommandLine (раздел 2.4.1 внешней спецификации). */
class TEST_ParseCommandLine : public QObject
{
    Q_OBJECT
public:
    TEST_ParseCommandLine();
    ~TEST_ParseCommandLine();

private slots:
    void TestParseCommandLine_data();
    void TestParseCommandLine();
};

#endif // TEST_PARSECOMMANDLINE_H