include(../qmllive.pri)

TEMPLATE = lib
TARGET = qmllive

DESTDIR = $${BUILD_DIR}/lib
DEFINES += QMLLIVE_LIBRARY

macx*: QMAKE_SONAME_PREFIX = @rpath

SOURCES += \
    qmllive.cpp

public_headers += \
    qmllive_global.h \
    qmllive.h

include(src.pri)

win32: target.path = $${PREFIX}/bin
else: target.path = $${PREFIX}/lib
INSTALLS += target

headers.files = $$public_headers
headers.path = $${PREFIX}/include/qmllive
INSTALLS += headers

!win32 {
    CONFIG += create_pc create_prl no_install_prl
    QMAKE_PKGCONFIG_NAME = qmllive
    QMAKE_PKGCONFIG_DESCRIPTION = Qt QML Live Library
    QMAKE_PKGCONFIG_PREFIX = $$PREFIX
    QMAKE_PKGCONFIG_LIBDIR = ${prefix}/lib
    QMAKE_PKGCONFIG_INCDIR = ${prefix}/include/qmllive
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
}
