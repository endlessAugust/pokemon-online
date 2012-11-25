#-------------------------------------------------
#
# Project created by QtCreator 2012-11-25T18:09:35
#
#-------------------------------------------------

QT       += network

TARGET = WebServerPlugin
TEMPLATE = lib
DESTDIR = ../../bin/serverplugins

DEFINES += WEBSERVERPLUGIN_LIBRARY

LIBS += -L../../bin \
    -lqtwebsocket

SOURCES += webserverplugin.cpp

HEADERS += webserverplugin.h\
        WebServerPlugin_global.h

QMAKE_CXXFLAGS += "-std=c++0x -U__STRICT_ANSI__"

include(../Shared/Common.pri)
