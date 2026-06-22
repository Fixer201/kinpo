QT = core
CONFIG += c++17 cmdline

INCLUDEPATH += $$PWD/../src

SOURCES += \
    ../src/checkersystem.cpp \
    ../src/rules/rule_art001.cpp \
    ../src/rules/rule_art002.cpp \
    ../src/rules/rule_art003.cpp \
    ../src/rules/rule_art004.cpp \
    ../src/rules/rule_art005a.cpp \
    ../src/rules/rule_art006.cpp \
    ../src/rules/rule_det001.cpp \
    ../src/rules/rule_det002.cpp \
    ../src/rules/rule_det003.cpp \
    main.cpp \
    ../src/datamodel.cpp \
    ../src/inputmodule.cpp \
    ../src/modelbuilder.cpp \
    ../src/wordlists.cpp

HEADERS += \
    ../src/checkersystem.h \
    ../src/datamodel.h \
    ../src/inputmodule.h \
    ../src/modelbuilder.h \
    ../src/wordlists.h \
    ../src/rules/rule_art001.h \
    ../src/rules/rule_art002.h \
    ../src/rules/rule_art003.h \
    ../src/rules/rule_art004.h \
    ../src/rules/rule_art005a.h \
    ../src/rules/rule_art006.h \
    ../src/rules/rule_det001.h \
    ../src/rules/rule_det002.h \
    ../src/rules/rule_det003.h
