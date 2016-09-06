android|ios {
    message("Note: the bench, examples and shared library will not be built on this platform")
    CONFIG += skip-bench skip-examples static-link-runtime
}

VERSION = 0.1.0
isEmpty(PREFIX): PREFIX = $$[QT_INSTALL_PREFIX]

VERSIONS = $$split(VERSION, ".")
VERSION_MAJOR = $$member(VERSIONS, 0)
unset(VERSIONS)

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
