
# *edfView*
---

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
using the command `$ apt-get install qt5-default`.  Now, run the installation.

```bash
$ qmake
$ make
```

If all went well, the executable `program `edfView` appears, with which you can
view and score your .edf sleep records.
