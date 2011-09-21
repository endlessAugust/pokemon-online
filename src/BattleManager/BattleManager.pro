#-------------------------------------------------
#
# Project created by QtCreator 2011-08-29T23:44:58
#
#-------------------------------------------------

QT += declarative opengl
TARGET = battlelib
TEMPLATE = lib
DESTDIR = ../../bin
LIBS += -L../../bin \
    -lutilities \
    -lpokemonlib

DEFINES += BATTLEMANAGER_LIBRARY

SOURCES += \
    main.cpp \
    testing.cpp \
    teamdata.cpp \
    battleinput.cpp \
    battleclientlog.cpp \
    auxpokebattledata.cpp \
    battlescene.cpp \
    battledataaccessor.cpp \
    battlesceneproxy.cpp \
    pokemoninfoaccessor.cpp \
    auxpokedataproxy.cpp

HEADERS +=\
    command.h \
    commandmanager.h \
    commandextracter.h \
    commandflow.h \
    commandinvoke.h \
    battleenum.h \
    battlecommandmanager.h \
    battleextracter.h \
    battlecommandinvoker.h \
    test.h \
    battledata.h \
    teamdata.h \
    battleinput.h \
    battleclientlog.h \
    auxpokebattledata.h \
    defaulttheme.h \
    battlescene.h \
    battledataaccessor.h \
    battlesceneproxy.h \
    pokemoninfoaccessor.h \
    datacontainer.h \
    proxydatacontainer.h \
    battledatatypes.h \
    auxpokedataproxy.h

QMAKE_CXXFLAGS += "-std=c++0x"

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEEF4D708
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = BattleManager.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    battlescene.qml \
    BattleDataQML/Pokemon.qml \
    BattleDataQML/Team.qml \
    ../../bin/qml/battlescene.qml \
    ../../bin/qml/BattleDataQML/qmldir \
    ../../bin/qml/BattleDataQML/Team.qml \
    ../../bin/qml/BattleDataQML/Pokemon.qml \
    ../../bin/qml/BattleDataQML/FieldPokemon.qml \
    ../../bin/qml/BattleDataQML/ProgressBar.qml \
    ../../bin/qml/BattleDataQML/FrameAnimation.qml \
    ../../bin/qml/BattleDataQML/PokeballAnimation.qml \
    ../../bin/qml/BattleDataQML/ColorShader.qml \
    ../../bin/qml/BattleDataQML/colors.js \
    ../../bin/qml/BattleDataQML/weather.js \
    ../../bin/qml/BattleDataQML/Weather/Rain.qml \
    ../../bin/qml/BattleDataQML/Weather/Sand.qml \
    ../../bin/qml/BattleDataQML/Weather/Hail.qml \
    ../../bin/qml/BattleDataQML/Weather/qmldir

RESOURCES +=































































