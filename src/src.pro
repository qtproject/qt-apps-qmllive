TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
  bench \
  previewGenerator \
  runtime \
  doc \

include(src.pri)
