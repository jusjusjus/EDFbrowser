/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014 Teunis van Beelen
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




#ifndef IMPORTANNOTATIONSFORM1_H
#define IMPORTANNOTATIONSFORM1_H



#include <QApplication>
#include <QObject>
#include <QFileDialog>
#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QProgressDialog>
#include <QCursor>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "popup_messagewindow.h"
#include "utc_date_time.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"
#include "xml.h"
#include "edflib.h"
#include "check_edf_file.h"
#include "edf_annotations.h"


#define XML_FORMAT      0
#define ASCIICSV_FORMAT 1
#define DCEVENT_FORMAT  2
#define EDFPLUS_FORMAT  3

#define TAB_CNT         4


class UI_Mainwindow;
class UI_Annotationswindow;



struct import_annotations_var_block
{
  char separator[4];
  int format;
  int onsettimeformat;
  int onsetcolumn;
  int descriptioncolumn;
  int manualdescription;
  int useduration;
  int useend;
  int durationcolumn;
  int endcolumn;
  char description[21];
  int datastartline;
  int dceventbittime;
  double triggerlevel;
  int ignoreconsecutive;
};



class UI_ImportAnnotationswindow : public QObject
{
	Q_OBJECT

public:

// UI_ImportAnnotationswindow(QWidget *parent);
	UI_ImportAnnotationswindow(QWidget *parent, UI_Annotationswindow *annotations_dock, struct annotationblock **annotationlist);

	UI_Mainwindow  *mainwindow;

private:

	struct annotationblock **annotationlist;
	UI_Annotationswindow *annotations_dock;

	QDialog        *ImportAnnotsDialog;

	QTabWidget     *tabholder;

	QWidget        *tab[TAB_CNT];

	QLabel         *SeparatorLabel,
               		*OnsetColumnLabel,
               		*DatastartLabel,
               *OnsetTimeLabel,
               *DCEventSignalLabel,
               *DCEventBitTimeLabel,
               *DCEventTriggerLevelLabel;

QLineEdit      *SeparatorLineEdit,
               *DescriptionLineEdit;

QSpinBox       *OnsetColumnSpinBox,
               *DurationColumnSpinBox,
               *EndColumnSpinBox,
               *DescriptionColumnSpinBox,
               *DatastartSpinbox,
               *BitTimeSpinbox;

QDoubleSpinBox *DCEventTriggerLevelSpinBox;

QComboBox      *RelativeTimeComboBox,
               *DCEventSignalComboBox;

QVBoxLayout    *asciiSettingsVBoxLayout,
               *DCEventVBoxLayout,
               *mainLayout;

QHBoxLayout    *asciiSettingsHBoxLayout1,
               *asciiSettingsHBoxLayout2,
               *asciiSettingsHBoxLayout3,
               *asciiSettingsHBoxLayout4,
               *asciiSettingsHBoxLayout5,
               *asciiSettingsHBoxLayout6,
               *asciiSettingsHBoxLayout7,
               *asciiSettingsHBoxLayout8,
               *DCEventHBoxLayout1,
               *DCEventHBoxLayout2,
               *DCEventHBoxLayout3,
               *horLayout;

QRadioButton   *DescriptionColumnRadioButton,
               *UseManualDescriptionRadioButton;

QCheckBox      *IgnoreConsecutiveCheckBox,
               *DurationCheckBox,
		*EndCheckBox;

	QButtonGroup *durationGroup;

QPushButton    *CloseButton,
               *ImportButton;

	int tab_index_array[TAB_CNT];

int import_from_xml(void);
int import_from_ascii(void);
int import_from_edfplus(void);
int import_from_dcevent(void);
	int read_datetime(char* line, long long& onset);


private slots:

void ImportButtonClicked();
void descriptionRadioButtonClicked(bool);
void DCEventSignalChanged(int);
void DurationCheckBoxChanged(int);
	void EndCheckBoxChanged(int);
void TabChanged(int);

};



#endif




