/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/


#ifndef xml_INCLUDED
#define xml_INCLUDED


#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QFont>
#include <QToolBar>
#include <QSlider>
#include <QStyle>
#if QT_VERSION < 0x050000
  #include <QPlastiqueStyle>
  #include <QGtkStyle>
  #include <QWindowsStyle>
#else
  #include <QStyleFactory>
#endif
#include <QtGlobal>
#include <QCloseEvent>

#ifdef Q_OS_LINUX
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  #include <QMacStyle>
#else
  #include <QStyleFactory>
#endif
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

#ifdef Q_OS_WIN32
  #include <windows.h>
  #include <io.h>
  #ifndef CSIDL_COMMON_APPDATA
    #define CSIDL_COMMON_APPDATA 0x0023 // All Users\Application Data
  #endif
  #ifndef CSIDL_APPDATA
    #define CSIDL_APPDATA 0x001a // <username>\Application Data
  #endif
  #ifndef CSIDL_PROGRAM_FILES
    #define CSIDL_PROGRAM_FILES 0x0026 // C:\Program Files
  #endif
  #if (QT_VERSION >= 0x050000)
    #define QT_WA(unicode, ansi) unicode
  #endif
#endif

#include <QFileDialog>
#include <QAction>
#include <QActionGroup>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QLibrary>
#include <QString>
#include <QLocale>
#include <QMessageBox>
#include <QProgressDialog>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "global.h"



struct xml_handle
{
  FILE *file;
  int level;
  int offset;
  int len;
  char *elementname;
  char *attributes;
  struct xml_handle *parent_handle_p;
  struct xml_handle *child_handle_p;
  char *tag_search_result;
  int encoding;
};


struct xml_handle * xml_get_handle(const char *);
struct xml_handle * xml_create_handle(const char *, char *);
void xml_close(struct xml_handle *);
void xml_go_up(struct xml_handle *);
int xml_goto_nth_element_inside(struct xml_handle *, const char *, int);
int xml_goto_next_element_with_same_name(struct xml_handle *);
int xml_goto_next_element_at_same_level(struct xml_handle *);
void xml_goto_root(struct xml_handle *);
char * xml_get_content_of_element(struct xml_handle *);
void xml_strcpy_encode_entity(char *, const char *);
int xml_strncpy_encode_entity(char *, const char *, int);
void xml_fwrite_encode_entity(FILE *, const char *);
void xml_fnwrite_encode_entity(FILE *, const char *, int);
void xml_strcpy_decode_entity(char *, const char *);
int xml_strncpy_decode_entity(char *, const char *, int);
int xml_character_encoding_type(struct xml_handle *);
int xml_get_attribute_of_element(struct xml_handle *, const char *, char *, int);

void configpath(char*, const char*);
FILE *open_configfile(const char*);

#ifdef __WIN32
QString specialFolder(int);
#endif


#endif












































