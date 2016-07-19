QT       += testlib core network

TARGET = tst_testipc
CONFIG   += testcase

include($$PWD/../../src/ipc/ipc.pri)
INCLUDEPATH += $$PWD/../../src

TEMPLATE = app

SOURCES += \
    tst_testipc.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
TESTDATA = testdata/*

