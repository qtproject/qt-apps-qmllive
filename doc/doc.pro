TEMPLATE = aux

CONFIG += force_qt
QT *= quick network

build_online_docs: {
    QMAKE_DOCS_TARGETDIR = qmllive
    QMAKE_DOCS = $$PWD/qmllive-online.qdocconf
} else {
    QMAKE_DOCS = $$PWD/qmllive.qdocconf
}

include(doc.pri)

OTHER_FILES += \
    $$PWD/*.qdocconf \
    $$PWD/*.qdoc \
    $$PWD/examples/*.qdoc \
    $$PWD/images/*.png
