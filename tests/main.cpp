/*!
* \file main.cpp
* \brief Точка входа тестового приложения.
*
* Собирает и запускает все тестовые классы через QTest::qExec.
* Каждый новый класс-тест требует добавления include и вызова qExec.
*/

#include <QtTest>

#include "TEST_Buildsentencemodel.h"
#include "TEST_CheckSentence.h"
#include "TEST_ParseSentenceBlock.h"
#include "TEST_ResolveCandidate.h"
#include "TEST_Rule_ART001.h"
#include "TEST_Rule_ART002.h"
#include "TEST_Rule_ART003.h"
#include "TEST_Rule_ART004.h"
#include "TEST_Rule_ART005a.h"
#include "TEST_Rule_ART006.h"
#include "TEST_Rule_DET001.h"
#include "TEST_Rule_DET002.h"
#include "TEST_Rule_DET003.h"
#include "TEST_ValidateSentenceStructure.h"

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_BuildSentenceModel, argc, argv);
    status |= QTest::qExec(new TEST_ParseSentenceBlock, argc, argv);
    status |= QTest::qExec(new TEST_ValidateSentenceStructure, argc, argv);
    status |= QTest::qExec(new TEST_CheckSentence, argc, argv);
    status |= QTest::qExec(new TEST_ResolveCandidate, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART003, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART005a, argc, argv);
    status |= QTest::qExec(new TEST_Rule_ART006, argc, argv);
    status |= QTest::qExec(new TEST_Rule_DET001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_DET002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_DET003, argc, argv);
    return status;
}
