QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../src

SOURCES += \
    TEST_Buildsentencemodel.cpp \
    TEST_CheckSentence.cpp \
    main.cpp \
    auxiliaryfunctionsfortesting.cpp \
    TEST_ParseSentenceBlock.cpp \
    TEST_ValidateSentenceStructure.cpp \
    $$PWD/../src/datamodel.cpp \
    $$PWD/../src/inputmodule.cpp

HEADERS += \
    TEST_Buildsentencemodel.h \
    TEST_CheckSentence.h \
    TEST_ParseSentenceBlock.h \
    TEST_ValidateSentenceStructure.h \
    auxiliaryfunctionsfortesting.h \
    $$PWD/../src/datamodel.h \
    $$PWD/../src/inputmodule.h

# gcov
QMAKE_CXXFLAGS += --coverage -O0 -g
QMAKE_LFLAGS += --coverage
LIBS += -lgcov
