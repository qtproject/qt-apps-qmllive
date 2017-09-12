include(../qmllive.pri)

INCLUDEPATH += $${PWD}

LIBS += -L$${BUILD_DIR}/lib

win32:!static: LIBS += -lqmllive$${VERSION_MAJOR}
else: LIBS += -lqmllive

# This can be simply eliminated by passing '-after QMAKE_RPATHDIR=' to qmake
QMAKE_RPATHDIR += $${BUILD_DIR}/lib
