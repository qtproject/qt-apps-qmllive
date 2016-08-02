requires(qtHaveModule(widgets))
requires(!winrt)


load(configure)
load(config-output)
include(qmllive.pri)

!minQtVersion(5, 1, 1):error("You need at least Qt 5.1.1 to build this application")

TEMPLATE = subdirs

SUBDIRS += \
    src \
    tests

!CONFIG(skip-examples): SUBDIRS += examples

OTHER_FILES += \
    README.md \
    INSTALL.md \
    CONTRIBUTORS.md

include(doc/doc.pri)
