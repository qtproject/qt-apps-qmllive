# QmlLive

A live coding environment for QML.

Allows to reload your qml scene when a file in your workspace changes. This can be done on the same host using the QmlLiveBench or on a remote device using the QmlLiveRuntime using the QmlLiveBench as server.

## Sub Projects

- qmllivebench - the qml-live workbench. All inclusive
- qmllivert - a default qml-live runtime for easy use
- src - common source code shared by all projects

## Desktop

For desktop usage just compile livebench.

    $ qmake
    $ make
    $ ./bin/qmllivebench

The project files will automatically detect the qt version and enables
or disables some features based on that.


## Using QmlLiveBench

Open QmlLiveBench on your host system and use File->Open Workspace to open your workspace. QmlLiveBench will observe all file changes in the directory and all sub-directories. Now you need to select a QML file in the workspace file viewer to start the rendering.

When you now edit and save a file. The the changes wil be applied and the rendering reloaded. This goes in a fraction of a second.

If you scene needs additional imports you can specify them under the settings dialog. Here you will also find the proxy information and the configurations for the target devices.

## Running a remote session

Configuring the target:

Make sure you have any required imports available, which are either not in the workspace or are native pulgins. The qml files will be
provided by the remote client using a synchronization protocol. Create a clean directory and start qmlliveruntime from there, passing any required imports using -importpath.

Connecting with the client:

Start qmlliveremote on your host machine. Open the qml workspace and connect to
the target. Enable Publish changes and press Publish all to transfer the entire
workspace to the target. Click on the qml file you want to run, it should start
on the target. Edit using your favorite tools on the host and instantly get the
changes on the target when saving.

# Contributions

* Dominik Holland - Thank you for shouldering most of the work and to ensure the IPC code is rock-solid.
* Robert Griebl - Thank you for knowing so many details on Qt and helping with the internals.
* Juergen Bocklage-Ryannel - For providing the initial idea and believing into the product.

Copyright (C) 2016 Pelagicore AG
