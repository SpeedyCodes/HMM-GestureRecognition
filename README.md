# HMM-GestureRecognition

## Compilation and running for the first time

- Install QT 6.5 with your compiler of choice
- Where Qt was installed, find the directory with binaries for your compiler and add it to PATH, e.g. `D:\Qt\6.5.0\mingw_64\bin`
- Add this directory to the `CMAKE_PREFIX_PATH` CMake option, e.g. `-DCMAKE_PREFIX_PATH=D:\Qt\6.5.0\mingw_64`
- At this point, compilation should succeed. However, Qt might not find the dynamically linked libraries it needs at runtime. To resolve this, copy `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Network.dll`, `Qt6Multimedia.dll` and `Qt6MultimediaWidgets.dll` to the working directory of the compiled executable. Then, from the command line, run `windeployqt [executable-name]` in the same directory. This will generate many files and directories, all of which can be deleted except the `platforms` directory.
- The executable should now run correctly.