#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T15:08:40
#
#-------------------------------------------------

TARGET = SmogonPlugin
TEMPLATE = lib
CONFIG += plugin dylib
VERSION = 1.0.0 
EXTRAS=clientplugin

QT += xml network 

DEFINES += SMOGONPLUGIN_LIBRARY

SOURCES += smogonplugin.cpp \
           smogonscraper.cpp \
            pokemontab.cpp \
            pokemonteamtabs.cpp \
    smogonbuild.cpp \
    teambuildersmogonplugin.cpp \
    smogonsinglepokedialog.cpp


HEADERS += smogonplugin.h\
        SmogonPlugin_global.h \
    ../../Teambuilder/plugininterface.h \
    ../../Teambuilder/engineinterface.h \
    smogonscraper.h \
    pokemontab.h \
    pokemonteamtabs.h \
    smogonbuild.h \
    teambuildersmogonplugin.h \
    smogonsinglepokedialog.h \
    ../../PokemonInfo/teamholderinterface.h

FORMS += smogonsinglepokedialog.ui

include(../../Shared/Common.pri)

LIBS += $$battlemanager
