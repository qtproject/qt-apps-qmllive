TEMPLATE = app
TARGET = previewGenerator
DESTDIR = $$BUILD_DIR/bin

# install rules
isEmpty(PREFIX) {
    target.path = $$[QT_INSTALL_BINS]
} else {
    macos: INSTALLSUBDIR=$${TARGET}.app/Contents/MacOS/
    macos: CONFIG -= app_bundle
    target.path = $$PREFIX
}

INSTALLS += target

QT = gui core quick widgets

SOURCES += \
    main.cpp
