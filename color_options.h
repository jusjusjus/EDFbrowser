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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
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




#ifndef COLOROPTIONS_H
#define COLOROPTIONS_H



#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QColor>
#include <QSpinBox>
#include <QPalette>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QStringList>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QString>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QFileDialog>

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "special_button.h"
#include "signalcurve.h"



class UI_Mainwindow;



class ColorOptions : public QWidget
{
	Q_OBJECT

public:
	ColorOptions(QWidget *parent=0);

	UI_Mainwindow *mainwindow;


private:

	QScrollArea *scrollarea;
	
	QFrame *frame;
	
	QGridLayout *scrolllayout;
	
	QTabWidget *tabholder;
	
	QVBoxLayout *mainLayout;
	
	QHBoxLayout *horLayout;
	
	QPushButton *CloseButton,
		    *DefaultButton,
		    *colorSchema_NK_Button,
		    *colorSchema_Dark_Button,
		    *ApplyButton,
		    *DefaultButton2,
		    *saveColorSchemaButton,
		    *loadColorSchemaButton;
	
	SpecialButton *BgColorButton,
			*SrColorButton,
			*BrColorButton,
			*MrColorButton,
			*TxtColorButton,
			*SigColorButton,
			*BaseColorButton,
			*Crh1ColorButton,
			*Crh2ColorButton,
			*FrColorButton,
			*AnnotMkrButton,
			*slabel2_1,
			*slabel2_3;
	
	QLabel *label_bg,
		*label_sruler,
		*label3,
		*label4,
		*label5,
		*label_sigcolor,
		*label7,
		*label8,
		*label9,
		*label10,
		*label11,
		*label_amarker,
		*label13,
		*label4_1,
		*label4_2,
		*label4_3,
		*label4_4,
		*label4_5,
		*label4_6,
		*label4_7,
		*label4_8,
		*label4_9,
		*label4_10,
		*label2_2,
		*label2_4,
		*label1_3,
		*label2_3,
		*label3_3,
		*label3_4;
	
	QSpinBox *spinbox2_1,
		 *spinbox2_2,
		 *spinbox3_1,
		 *spinbox4_1,
		 *spinbox4_2,
		 *spinbox4_3;
	
	QComboBox *combobox4_1,
		  *combobox4_2,
		  *combobox4_3;
	
	QCheckBox *checkbox1,
		  *checkbox2,
		  *chbox_show_baselines,
		  *checkbox4,
		  *checkbox4_1,
		  *checkbox4_2,
		  *checkbox4_3,
		  *checkbox4_4,
		  *checkbox4_5,
		  *checkbox2_1;
	
	QTableWidget *colorBarTable;
	
	QRadioButton *radiobutton1,
		     *radiobutton2,
		     *radiobutton3;
	
	QLineEdit *lineEdit3_1;
	
	void update_interface(void);


private slots:

	void BgColorButtonClicked(SpecialButton *);
	void SrColorButtonClicked(SpecialButton *);
	void BrColorButtonClicked(SpecialButton *);
	void MrColorButtonClicked(SpecialButton *);
	void TxtColorButtonClicked(SpecialButton *);
	void SigColorButtonClicked(SpecialButton *);
	void BaseColorButtonClicked(SpecialButton *);
	void Crh1ColorButtonClicked(SpecialButton *);
	void Crh2ColorButtonClicked(SpecialButton *);
	void FrColorButtonClicked(SpecialButton *);
	void AnnotMkrButtonClicked(SpecialButton *);
	void bw_printing(int);
	void show_annot_markers(int);
	void show_baselines(int);
	void checkbox4Clicked(int);
	void checkbox4_1Clicked(int);
	void DefaultButtonClicked();
	void ApplyButtonClicked();
	void calibrate_checkbox_stateChanged(int);
	void spinBox4_1ValueChanged(int);
	void combobox4_1IndexChanged(int);
	void combobox4_2IndexChanged(int);
	void combobox4_3IndexChanged(int);
	void spinBox4_2ValueChanged(int);
	void spinBox4_3ValueChanged(int);
	void saveColorSchemaButtonClicked();
	void loadColorSchemaButtonClicked();
	void loadColorSchema_NK();
	void loadColorSchema_Dark();

};



#endif // COLOROPTIONS_H


