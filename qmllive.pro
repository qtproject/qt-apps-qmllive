!skip-bench: requires(qtHaveModule(widgets))
requires(linux|macos|win32:!winrt:!integrity:!wasm:!ios)

load(configure)
load(config-output)
include(qmllive.pri)

!skip-bench:!minQtVersion(5, 4, 0): error("You need at least Qt 5.4.0 to build QML Live Bench")
!skip-tests:!minQtVersion(5, 4, 0): error("You need at least Qt 5.4.0 to build QML Live tests")
!minQtVersion(5, 2, 0): error("You need at least Qt 5.4.0 to build QML Live Bench and/or tests, 5.2.0 for the rest")

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src
!skip-tests: SUBDIRS += tests
!skip-examples: SUBDIRS += examples
SUBDIRS += doc

include(doc/doc.pri)

OTHER_FILES += \
    README.md \
    INSTALL.md \
    CONTRIBUTORS.md \
    .qmake.conf \
