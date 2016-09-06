include(../qmllive.pri)

TEMPLATE = subdirs
CONFIG += ordered

skip-bench:skip-examples:static-link-runtime {
  CONFIG += skip-lib
}

!skip-lib {
  SUBDIRS += lib.pro
}

!skip-bench {
  SUBDIRS += bench \
             previewGenerator
}

!skip-runtime {
  SUBDIRS += runtime
}
