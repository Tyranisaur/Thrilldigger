#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T13:52:58
#
#-------------------------------------------------

QT       += core gui

QTPLUGIN += gif

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Thrilldigger
TEMPLATE = app


SOURCES += src/main.cpp \
    src/settingswindow.cpp \
    src/simulatorwindow.cpp \
    src/board.cpp \
    src/solverwindow.cpp \
    src/solver.cpp \
    src/configurationiterator.cpp \
    src/partitioniterator.cpp \
    src/benchmark.cpp

HEADERS  += \
    headers/settingswindow.h \
    headers/simulatorwindow.h \
    headers/board.h \
    headers/solverwindow.h \
    headers/solver.h \
    headers/dugtype.h \
    headers/problemparameters.h \
    headers/hole.h \
    headers/configurationiterator.h \
    headers/constraint.h \
    headers/partition.h \
    headers/partitioniterator.h \
    headers/benchmark.h

FORMS    += \
    forms/settingswindow.ui \
    forms/simulatorwindow.ui \
    forms/solverwindow.ui

RESOURCES += \
    application.qrc
