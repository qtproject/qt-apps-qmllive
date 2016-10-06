android|ios|qnx {
    message("Note: the bench, examples and shared library will not be built on this platform")
    CONFIG += skip-bench skip-examples static-link-runtime
}

VERSION = 0.1.0
isEmpty(PREFIX): PREFIX = $$[QT_INSTALL_PREFIX]
isEmpty(EXAMPLES_PREFIX): EXAMPLES_PREFIX = $$[QT_INSTALL_LIBS]/qmllive/examples

VERSIONS = $$split(VERSION, ".")
VERSION_MAJOR = $$member(VERSIONS, 0)
unset(VERSIONS)

DEFINES += QMLLIVE_VERSION=$$VERSION
!isEmpty(QMLLIVE_VERSION_EXTRA): DEFINES += QMLLIVE_VERSION_EXTRA="\"$$QMLLIVE_VERSION_EXTRA\""
!isEmpty(QMLLIVE_REVISION): DEFINES += QMLLIVE_REVISION=$$QMLLIVE_REVISION
!isEmpty(QMLLIVE_SETTINGS_VARIANT): DEFINES += QMLLIVE_SETTINGS_VARIANT=$$QMLLIVE_SETTINGS_VARIANT

# from qtcreator.pri
defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}
