TEMPLATE      = lib
CONFIG       += plugin c++11
QT           += qml

include(../../src/lib.pri)

INCLUDEPATH  += ../../src
HEADERS       = mycontentadapterplugin.h
SOURCES       = mycontentadapterplugin.cpp
TARGET        = myContentAdapterPlugin
DESTDIR       = ../../bin/plugins

RESOURCES += res.qrc

OTHER_FILES += \
    plugin.qml

target.path = $$EXAMPLES_PREFIX/contentplugin
INSTALLS += target
