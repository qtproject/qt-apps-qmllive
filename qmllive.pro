!CONFIG(skip-bench): requires(qtHaveModule(widgets))
requires(!winrt)

load(configure)
load(config-output)
include(qmllive.pri)
include(doc/doc.pri)

!minQtVersion(5, 4, 0):error("You need at least Qt 5.4.0 to build this application")

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src
!CONFIG(skip-tests): SUBDIRS += tests
!CONFIG(skip-examples): SUBDIRS += examples

OTHER_FILES += \
    README.md \
    INSTALL.md \
    CONTRIBUTORS.md
