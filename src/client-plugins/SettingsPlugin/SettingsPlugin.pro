#-------------------------------------------------
#
# Project created by QtCreator 2012-06-27T18:54:35
#
#-------------------------------------------------

TARGET = SettingsPlugin
TEMPLATE = lib
CONFIG += plugin dylib
VERSION = 1.0.0
EXTRAS=clientplugin

DEFINES += SETTINGSPLUGIN_LIBRARY

SOURCES += settingsplugin.cpp \
    settingsdialog.cpp

HEADERS += settingsplugin.h\
        SettingsPlugin_global.h \
    settingsdialog.h

FORMS += settingsdialog.ui

include(../../Shared/Common.pri)

LIBS += $$utilities
