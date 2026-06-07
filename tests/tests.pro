QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../src

SOURCES += \
    main.cpp \
    auxiliaryfunctionsfortesting.cpp \
    TEST_ParseSentenceBlock.cpp \
    $$PWD/../src/datamodel.cpp \
    $$PWD/../src/inputmodule.cpp

HEADERS += \
    TEST_ParseSentenceBlock.h \
    auxiliaryfunctionsfortesting.h \

# gcov
QMAKE_CXXFLAGS += --coverage -O0 -g
QMAKE_LFLAGS += --coverage
LIBS += -lgcov
