#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T19:31:11
#
#-------------------------------------------------

QT       += widgets

TARGET = DatabaseEditor
TEMPLATE = lib
CONFIG += plugin dylib
VERSION = 1.0.0
EXTRAS=clientplugin

DEFINES += DATABASEEDITOR_LIBRARY

SOURCES += databaseeditor.cpp \
    pokemoneditordialog.cpp

HEADERS += databaseeditor.h\
        databaseeditor_global.h \
    ../../Teambuilder/plugininterface.h \
    ../../Teambuilder/engineinterface.h \
    ../../PokemonInfo/teamholderinterface.h \
    pokemoneditordialog.h

include(../../Shared/Common.pri)

LIBS += $$teambuilder

FORMS += \
    pokemoneditordialog.ui
