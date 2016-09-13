build_online_docs: {
    QMAKE_DOCS_TARGETDIR = qmllive
    QMAKE_DOCS = $$PWD/qmllive-online.qdocconf
} else {
    QMAKE_DOCS = $$PWD/qmllive.qdocconf
}

CONFIG += prepare_docs
load(qt_docs_targets)

OTHER_FILES += \
    $$PWD/*.qdocconf \
    $$PWD/*.qdoc \
    $$PWD/examples/*.qdoc \
    $$PWD/images/*.png
