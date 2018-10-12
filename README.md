
# *edfView*
===

*edfView* is a Qt5 application to view and annotate signal data in .edf/.bdf(+)
format.  It's a fork of Teunis van Beelen's infamous *EDFBrowser*.

# Building from Sources
---

## Requirements Summary

 - qt5 tool chain for your operating system

## Clone [the .git Repository](https://github.com/jusjusjus/edfView)

Use `git` to clone the repository to your local computer: `> git
https://github.com/jusjusjus/edfView.git`, and enter (`> cd edfView`).

## Windows

To compile in a Windows machine: ( I used QT 5.9.1 minggw 32bit to compile this project)
 - Make sure the environment variables and path are set correctly
 - Open this project and make sure to select the correct compiler for configuration
 - Go to Sources/main.cpp in QT creator and build this porject

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
