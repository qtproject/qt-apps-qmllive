!skip-bench: requires(qtHaveModule(widgets))
requires(!winrt)

load(configure)
load(config-output)
include(qmllive.pri)
include(doc/doc.pri)

!skip-bench:!minQtVersion(5, 4, 0): error("You need at least Qt 5.4.0 to build QmlLive Bench")
!skip-tests:!minQtVersion(5, 4, 0): error("You need at least Qt 5.4.0 to build QmlLive tests")
!minQtVersion(5, 2, 0): error("You need at least Qt 5.4.0 to build QmlLive Bench and/or tests, 5.2.0 for the rest")

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src
!skip-tests: SUBDIRS += tests
!skip-examples: SUBDIRS += examples

OTHER_FILES += \
    README.md \
    INSTALL.md \
    CONTRIBUTORS.md
