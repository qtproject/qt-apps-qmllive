# Build and Install QmlLive

## Dependencies

- Windows, Linux or macOS
- Qt5.4 or higher


## Building for desktop

    $ qmake
    $ make

The desktop executable is available at `./bin/qmllivebench`.

## Building for device

Note: Only needed when you want to have live reloading enabled on the target.

    $ qmake # from cross compiler toolchain
    $ make

You need to cross-compile QmlLive and copy the `./bin/qmlliveruntime` onto the
target.

## Build documentation

    $ qmake CONFIG+=force_independent
    $ make docs

The documentation will be avilable at 'doc/qmllive/index.html'.

Copyright (C) 2019 Luxoft Sweden AB
