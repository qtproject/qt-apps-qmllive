
exists($$[QT_INSTALL_BINS]/qdoc):exists($$[QT_INSTALL_BINS]/qhelpgenerator) {
    check_qdoc = "qdoc/qhelpgenerator in $$[QT_INSTALL_BINS]"
    QDOC = $$[QT_INSTALL_BINS]/qdoc
    QHELPGENERATOR = $$[QT_INSTALL_BINS]/qhelpgenerator
} else {
    check_qdoc = "qdoc/qhelpgenerator in PATH"
    QDOC = qdoc
    QHELPGENERATOR = qhelpgenerator
}

VERSION_TAG = $$replace(VERSION, "[-.]", )

defineReplace(cmdEnv) {
    !equals(QMAKE_DIR_SEP, /): 1 ~= s,^(.*)$,(set \\1) &&,g
    return("$$1")
}

defineReplace(qdoc) {
    return("$$cmdEnv(OUTDIR=$$1 QMLLIVE_VERSION=$$VERSION QMLLIVE_VERSION_TAG=$$VERSION_TAG) $$QDOC")
}

printConfigLine("Documentation generator", $$check_qdoc)

html-docs.commands = $$qdoc($$BUILD_DIR/doc/html) $$PWD/qmllive.qdocconf
html-docs.files = $$BUILD_DIR/doc/html

html-docs-online.commands = $$qdoc($$BUILD_DIR/doc/html) $$PWD/qmllive-online.qdocconf
html-docs-online.files = $$BUILD_DIR/doc/html

qch-docs.commands = $$QHELPGENERATOR $$BUILD_DIR/doc/html/qmllive.qhp -o $$BUILD_DIR/doc/qch/qmllive.qch
qch-docs.files = $$BUILD_DIR/doc/qch
qch-docs.CONFIG += no_check_exist directory

docs.depends = html-docs qch-docs
docs-online.depends = html-docs-online

QMAKE_EXTRA_TARGETS += html-docs qch-docs docs html-docs-online docs-online

OTHER_FILES += \
    $$PWD/qmllive.qdocconf \
    $$PWD/*.qdoc \
    $$PWD/examples/*.qdoc
