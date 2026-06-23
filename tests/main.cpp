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
#include "TEST_ParseCommandLine.h"
#include "TEST_InitializeRuntime.h"
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
#include "TEST_Rule_DET004.h"
#include "TEST_Rule_DET005.h"
#include "TEST_Rule_PREP001.h"
#include "TEST_Rule_PREP002.h"
#include "TEST_Rule_PREP003.h"
#include "TEST_Rule_PREP004.h"
#include "TEST_Rule_PREP005.h"
#include "TEST_Rule_CONJ001.h"
#include "TEST_Rule_CONJ002.h"
#include "TEST_Rule_CONJ003.h"
#include "TEST_Rule_CONJ004.h"
#include "TEST_Rule_CONJ005.h"
#include "TEST_Rule_CONJ006.h"
#include "TEST_Rule_AUX001.h"
#include "TEST_Rule_AUX002.h"
#include "TEST_Rule_AUX003.h"
#include "TEST_Rule_AUX004.h"
#include "TEST_Rule_AUX005.h"
#include "TEST_Rule_AUX006.h"
#include "TEST_Rule_OTHER001.h"
#include "TEST_Rule_OTHER002.h"
#include "TEST_Rule_OTHER003.h"
#include "TEST_Rule_OTHER004.h"
#include "TEST_Rule_OTHER005.h"
#include "TEST_Rule_OTHER006.h"
#include "TEST_ValidateSentenceStructure.h"

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_BuildSentenceModel, argc, argv);
    status |= QTest::qExec(new TEST_ParseSentenceBlock, argc, argv);
    status |= QTest::qExec(new TEST_ParseCommandLine, argc, argv);
    status |= QTest::qExec(new TEST_InitializeRuntime, argc, argv);
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
    status |= QTest::qExec(new TEST_Rule_DET004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_DET005, argc, argv);
    status |= QTest::qExec(new TEST_Rule_PREP001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_PREP002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_PREP003, argc, argv);
    status |= QTest::qExec(new TEST_Rule_PREP004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_PREP005, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ003, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ005, argc, argv);
    status |= QTest::qExec(new TEST_Rule_CONJ006, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX003, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX005, argc, argv);
    status |= QTest::qExec(new TEST_Rule_AUX006, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER001, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER002, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER003, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER004, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER005, argc, argv);
    status |= QTest::qExec(new TEST_Rule_OTHER006, argc, argv);
    return status;
}
