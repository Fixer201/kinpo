QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

DEFINES += PROJECT_SOURCE_ROOT=\\\"$$PWD/..\\\"

TEMPLATE = app

INCLUDEPATH += $$PWD/../src
INCLUDEPATH += $$PWD/../src/rules
INCLUDEPATH += $$PWD/../src

SOURCES += \
    TEST_Buildsentencemodel.cpp \
    TEST_CheckSentence.cpp \
    TEST_ResolveCandidate.cpp \
    TEST_Rule_ART001.cpp \
    TEST_Rule_ART002.cpp \
    TEST_Rule_ART003.cpp \
    TEST_Rule_ART004.cpp \
    TEST_Rule_ART005a.cpp \
    TEST_Rule_ART006.cpp \
    TEST_Rule_DET001.cpp \
    TEST_Rule_DET002.cpp \
    TEST_Rule_DET003.cpp \
    TEST_Rule_DET004.cpp \
    TEST_Rule_DET005.cpp \
    TEST_Rule_PREP001.cpp \
    TEST_Rule_PREP002.cpp \
    main.cpp \
    auxiliaryfunctionsfortesting.cpp \
    TEST_ParseSentenceBlock.cpp \
    TEST_ValidateSentenceStructure.cpp \
    $$PWD/../src/datamodel.cpp \
    $$PWD/../src/inputmodule.cpp \
    $$PWD/../src/modelbuilder.cpp \
    $$PWD/../src/wordlists.cpp \
    $$PWD/../src/checkersystem.cpp \
    $$PWD/../src/rules/rule_art001.cpp \
    $$PWD/../src/rules/rule_art002.cpp \
    $$PWD/../src/rules/rule_art003.cpp \
    $$PWD/../src/rules/rule_art004.cpp \
    $$PWD/../src/rules/rule_art005a.cpp \
    $$PWD/../src/rules/rule_art006.cpp \
    $$PWD/../src/rules/rule_det001.cpp \
    $$PWD/../src/rules/rule_det002.cpp \
    $$PWD/../src/rules/rule_det003.cpp \
    $$PWD/../src/rules/rule_det004.cpp \
    $$PWD/../src/rules/rule_det005.cpp \
    $$PWD/../src/rules/rule_prep001.cpp \
    $$PWD/../src/rules/rule_prep002.cpp

HEADERS += \
    TEST_Buildsentencemodel.h \
    TEST_CheckSentence.h \
    TEST_ResolveCandidate.h \
    TEST_Rule_ART001.h \
    TEST_Rule_ART002.h \
    TEST_Rule_ART003.h \
    TEST_Rule_ART004.h \
    TEST_Rule_ART005a.h \
    TEST_Rule_ART006.h \
    TEST_Rule_DET001.h \
    TEST_Rule_DET002.h \
    TEST_Rule_DET003.h \
    TEST_Rule_DET004.h \
    TEST_Rule_DET005.h \
    TEST_Rule_PREP001.h \
    TEST_Rule_PREP002.h \
    TEST_ParseSentenceBlock.h \
    TEST_ValidateSentenceStructure.h \
    auxiliaryfunctionsfortesting.h \
    $$PWD/../src/datamodel.h \
    $$PWD/../src/inputmodule.h \
    $$PWD/../src/modelbuilder.h \
    $$PWD/../src/wordlists.h \
    $$PWD/../src/checkersystem.h \
    $$PWD/../src/rules/rule_art001.h \
    $$PWD/../src/rules/rule_art002.h \
    $$PWD/../src/rules/rule_art003.h \
    $$PWD/../src/rules/rule_art004.h \
    $$PWD/../src/rules/rule_art005a.h \
    $$PWD/../src/rules/rule_art006.h \
    $$PWD/../src/rules/rule_det001.h \
    $$PWD/../src/rules/rule_det002.h \
    $$PWD/../src/rules/rule_det003.h \
    $$PWD/../src/rules/rule_det004.h \
    $$PWD/../src/rules/rule_det005.h \
    $$PWD/../src/rules/rule_prep001.h \
    $$PWD/../src/rules/rule_prep002.h

# gcov
QMAKE_CXXFLAGS += --coverage -O0 -g
QMAKE_LFLAGS += --coverage
LIBS += -lgcov
