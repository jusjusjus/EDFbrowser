
# Requirements
---
Qt5

The GCC compiler or MinGW-w64 on windows. <http://mingw-w64.sourceforge.net/>


# Introduction
---

EDFView is a Qt4 application and uses qmake as part of the build
process.  qmake is part of your Qt4 installation.


To compile in a Windows machine: ( I used QT 5.9.1 minggw 32bit to compile this project)
 - Make sure the environment variables and path are set correctly
 - Open this project and make sure to select the correct compiler for configuration
 - Go to Sources/main.cpp in QT creator and build this porject


# Building from Sources and Installation
---

Use `git` to clone the repository to your local computer: `> git
https://github.com/jusjusjus/edfView.git`, and enter (`> cd edfView`).

## Ubuntu

To install *edfView* on ubuntu>=16.04 you need to first install `qt5-default`:
using the command `> apt-get install qt5-default`.  The qt compilation tool
`qmake` relies on the environment variable `QT_SELECT`.  You need to point it
to `qt5`.  Enter `> export QT_SELECT=qt5`, and before make sure that `qt5`
shows up in the list returned by `> qtchooser -l`.

Now, run through the installation procedure:
```
> qmake
> make
```
