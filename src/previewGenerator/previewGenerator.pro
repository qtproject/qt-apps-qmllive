include(../../qmllive.pri)

TEMPLATE = app
TARGET = previewGenerator
win32: DESTDIR = $$BUILD_DIR/bin
else: DESTDIR = $$BUILD_DIR/libexec/qmllive

# install rules
macx*: CONFIG -= app_bundle
win32: target.path = $$PREFIX/bin
else: target.path = $$PREFIX/libexec/qmllive
INSTALLS += target

CONFIG += c++11
QT = gui core quick

SOURCES += \
    main.cpp
