
Requirements
============
Qt (minimum version 4.7.1 or later, preferable 4.8.6, Do not use Qt 5.x.x to compile EDFView!!) http://qt-project.org/

The GCC compiler or MinGW-w64 on windows. <http://mingw-w64.sourceforge.net/>


Introduction
============

EDFView is a Qt4 application and uses qmake as part of the build
process.  qmake is part of your Qt4 installation.


 To compile in a Windows machine: ( I used QT 5.9.1 minggw 32bit to compile this project)
 --Make sure the environment variables and path are set correctly
 --Open this project and make sure to select the correct compiler for configuration
 --Go to Sources/main.cpp in QT creator and build this porject


Build and run without "installing"
==================================

You need to have Qt and GCC (MinGW-w64 on Windows) installed.

Extract the sourcefile and enter the following commands:

qmake

make

Now you can run the program.





