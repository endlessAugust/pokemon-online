#-------------------------------------------------
#
# Project created by QtCreator 2012-07-08T01:49:25
#
#-------------------------------------------------

QT       += core gui

TARGET = UpdateMaker
TEMPLATE = app
DESTDIR = ../../bin
LIBS += -L../../bin \
    -lutilities

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ../Utilities/ziputils.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += "-std=c++0x -U__STRICT_ANSI__"
