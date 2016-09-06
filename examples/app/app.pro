TEMPLATE = app
TARGET = app

QT *= quick

include(../../src/lib.pri)

SOURCES += main.cpp
OTHER_FILES += \
        qml/customruntime-item.qml \
        qml/customruntime-window.qml \
        qml/item.qml \
        qml/window.qml \
        icon.png
