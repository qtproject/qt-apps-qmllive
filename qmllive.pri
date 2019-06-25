android|ios|qnx {
    message("Note: the bench, examples and shared library will not be built on this platform")
    CONFIG += skip-bench skip-examples static-link-runtime
}

isEmpty(PREFIX): PREFIX = $$[QT_INSTALL_PREFIX]
isEmpty(EXAMPLES_PREFIX): EXAMPLES_PREFIX = $$[QT_INSTALL_EXAMPLES]/qmllive

VERSIONS = $$split(VERSION, ".")
VERSION_MAJOR = $$member(VERSIONS, 0)
unset(VERSIONS)

unix:exists($$SOURCE_DIR/.git):GIT_REVISION=$$system(cd "$$SOURCE_DIR" && git describe --tags --always 2>/dev/null)
isEmpty(GIT_REVISION) {
    GIT_REVISION="unknown revision"
    GIT_COMMITTER_DATE="no date"
} else {
    GIT_COMMITTER_DATE=$$system(cd "$$SOURCE_DIR" && git show "$$GIT_REVISION" --pretty=format:"%ci" --no-patch 2>/dev/null)
}

DEFINES += QMLLIVE_VERSION=$$VERSION
!isEmpty(QMLLIVE_VERSION_EXTRA): DEFINES += QMLLIVE_VERSION_EXTRA="\"$$QMLLIVE_VERSION_EXTRA\""
isEmpty(QMLLIVE_REVISION): QMLLIVE_REVISION=$$replace(GIT_REVISION," ","_")
DEFINES += QMLLIVE_REVISION=$$QMLLIVE_REVISION
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
