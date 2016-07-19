TEMPLATE = subdirs
CONFIG += ordered

!android:!ios {
  SUBDIRS += bench \
             previewGenerator
}

SUBDIRS += \
  runtime


include(src.pri)
