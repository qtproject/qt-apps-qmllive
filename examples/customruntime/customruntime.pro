TEMPLATE = app
TARGET = customruntime
CONFIG += c++11

macx*: CONFIG -= app_bundle
QT *= quick

include(../../src/lib.pri)

SOURCES += main.cpp

target.path = $$EXAMPLES_PREFIX/customruntime
INSTALLS += target

qml.files = qml/*.qml
qml.path = $$target.path/qml
INSTALLS += qml
OTHER_FILES += $$qml.files

icon.files = icon.png
icon.path = $$target.path
INSTALLS += icon
OTHER_FILES += $$icon.files
