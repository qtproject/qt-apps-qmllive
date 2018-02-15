include(../../qmllive.pri)

TEMPLATE = app
macx*:TARGET = "QML Live Bench"
else:TARGET = qmllivebench
DESTDIR = $$BUILD_DIR/bin

CONFIG += c++11
QT *= gui core quick widgets core-private

macx* {
    LIBS += -framework AppKit -framework Foundation
    OBJECTIVE_SOURCES += cocoahelper.mm
    HEADERS += cocoahelper.h
}

SOURCES += \
    aboutdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    optionsdialog.cpp \
    benchlivenodeengine.cpp \
    previewimageprovider.cpp \
    directorypreviewadapter.cpp \
    qmlpreviewadapter.cpp \
    host.cpp \
    hostmodel.cpp \
    hostwidget.cpp \
    dummydelegate.cpp \
    allhostswidget.cpp \
    hostmanager.cpp \
    hostsoptionpage.cpp \
    httpproxyoptionpage.cpp \
    importpathoptionpage.cpp \
    hostdiscoverymanager.cpp \
    autodiscoveryhostsdialog.cpp \
    options.cpp \
    newprojectwizard.cpp \
    appearanceoptionpage.cpp \
    runtimemanager.cpp \
    runtimeprocess.cpp \
    runtimeoptionpage.cpp

HEADERS += \
    aboutdialog.h \
    mainwindow.h \
    optionsdialog.h \
    benchlivenodeengine.h \
    previewimageprovider.h \
    directorypreviewadapter.h \
    qmlpreviewadapter.h \
    host.h \
    hostmodel.h \
    hostwidget.h \
    dummydelegate.h \
    allhostswidget.h \
    hostmanager.h \
    hostsoptionpage.h \
    importpathoptionpage.h \
    httpproxyoptionpage.h \
    hostdiscoverymanager.h \
    autodiscoveryhostsdialog.h \
    options.h \
    newprojectwizard.h \
    appearanceoptionpage.h \
    runtimemanager.h \
    runtimeprocess.h \
    runtimeoptionpage.h

FORMS += \
    optionsdialog.ui \
    hostsoptionpage.ui \
    httpproxyoptionpage.ui \
    importpathoptionpage.ui \
    autodiscoveryhostsdialog.ui \
    runtimeoptionpage.ui \
    appearanceoptionpage.ui

include(../widgets/widgets.pri)
include(../lib.pri)

# install rules
target.path = $$PREFIX/bin
INSTALLS += target

win32: RC_FILE = ../../icons/appicon.rc
macx*: ICON = ../../icons/appicon.icns

DISTFILES += mac_wrapper.sh.in

macx* {
    QMAKE_INFO_PLIST = $$PWD/Info.plist

    make_wrapper.target = $${DESTDIR}/qmllivebench
    make_wrapper.depends = $$PWD/mac_wrapper.sh.in
    make_wrapper.commands = sed \"s/%TARGET%/$${TARGET}/g\" $${make_wrapper.depends} > $${make_wrapper.target} \
                                && chmod +x $${make_wrapper.target}
    QMAKE_EXTRA_TARGETS += make_wrapper
    PRE_TARGETDEPS += $${make_wrapper.target}
    wrapper.files = $${make_wrapper.target}
    wrapper.path = $${target.path}
    wrapper.CONFIG += no_check_exist executable
    INSTALLS += wrapper
}
