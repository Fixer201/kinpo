QT = core
CONFIG += c++17 cmdline

INCLUDEPATH += $$PWD/../src

SOURCES += \
    ../src/checkersystem.cpp \
    ../src/rules/rule_art001.cpp \
    main.cpp \
    ../src/datamodel.cpp \
    ../src/inputmodule.cpp \
    ../src/modelbuilder.cpp

HEADERS += \
    ../src/checkersystem.h \
    ../src/datamodel.h \
    ../src/inputmodule.h \
    ../src/modelbuilder.h \
    ../src/rules/rule_art001.h
