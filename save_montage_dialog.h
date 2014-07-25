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




#ifndef SAVEMONTAGEFORM1_H
#define SAVEMONTAGEFORM1_H



#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QListWidget>
#include <QList>
#include <QMessageBox>
#include <QString>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"




class UI_Mainwindow;



class UI_SaveMontagewindow : public QObject
{
  Q_OBJECT

public:
  UI_SaveMontagewindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *SaveMontageDialog;

  QLabel       *label1;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *SaveButton;


private slots:

  void SaveButtonClicked();

};



#endif // SAVEMONTAGEFORM1_H


