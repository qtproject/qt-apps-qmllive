TEMPLATE = app
TARGET = app

macx*: CONFIG -= app_bundle
QT *= quick

include(../../src/lib.pri)

SOURCES += main.cpp

target.path = $$EXAMPLES_PREFIX/app
INSTALLS += target

qml.files = \
        qml/customruntime-item.qml \
        qml/customruntime-window.qml \
        qml/item.qml \
        qml/window.qml
qml.path = $$EXAMPLES_PREFIX/app/qml
INSTALLS += qml
OTHER_FILES += $$qml.files

icon.files = icon.png
icon.path = $$EXAMPLES_PREFIX/app
INSTALLS += icon
OTHER_FILES += $$icon.files
