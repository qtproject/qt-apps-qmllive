include(qmllive.pri)

!minQtVersion(5, 1, 1):error("You need at least Qt 5.1.1 to build this application")

TEMPLATE = subdirs


SUBDIRS += \
    src \
    # tests
