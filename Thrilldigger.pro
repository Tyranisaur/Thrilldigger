#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T13:52:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Thrilldigger
TEMPLATE = app


SOURCES += main.cpp \
    settingswindow.cpp \
    simulatorwindow.cpp \
    board.cpp \
    solverwindow.cpp \
    solver.cpp

HEADERS  += \
    settingswindow.h \
    simulatorwindow.h \
    board.h \
    solverwindow.h \
    solver.h \
    dugtype.h \
    celltype.h \
    problemparameters.h

FORMS    += \
    settingswindow.ui \
    simulatorwindow.ui \
    solverwindow.ui
