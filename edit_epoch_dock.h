/*
***************************************************************************
*
* Author: Justus Schwabedal
*
* Copyright (C) 2015
*
* JSchwabedal@gmail.com
*
***************************************************************************
*
*/




#ifndef SLEEP_SCORING_DOCK_H
#define SLEEP_SCORING_DOCK_H



#include <QApplication>
#include <QObject>
#include <QDockWidget>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDomDocument>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "global.h"
#include "mainwindow.h"
#include "annotations_dock.h"
#include "popup_save_cancelwindow.h"
#include "edf_annot_list.h"



class UI_Mainwindow;




class UI_EpochEditwindow : public QObject
{
	Q_OBJECT

public:

	UI_EpochEditwindow(QWidget *parent);

	UI_Mainwindow *mainwindow;

	QDockWidget *dockedit;

	void set_selected_annotation(int);

	void set_selected_annotation(struct annotationblock *);

	virtual void deselect();

	virtual void setup();

	QAction *Delete_act;


private:

	struct annotationblock *annotation;

	int annot_num;

	QWidget *annot_edit_widget;

	QLabel *onsetLabel,
		 *durationLabel,
		 *descriptionLabel;

	std::vector<QString*> stages;
	std::vector<QKeySequence*> keys;
	QButtonGroup *stage_buttons;

	void load_xml();
	void load_default();


public slots:
	void open_close_dock(bool);


//protected slots:


private slots:

	void button_pressed(int);
};



#endif // SLEEP_SCORING_DOCK_H




