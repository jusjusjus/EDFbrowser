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




#ifndef ANNOTATIONSWINDOW_H
#define ANNOTATIONSWINDOW_H



#include "mainwindow.h"		// class UI_Mainwindow;
#include "averager_dialog.h"	// class UI_AveargerWindow;



class UI_Annotationswindow : public QListWidget
{
	Q_OBJECT

public:
	UI_Annotationswindow(int, QWidget *parent, const char* title="Annotations");

	UI_Mainwindow *mainwindow;

	QDockWidget *docklist;

	virtual void updateList(annotationblock* annotation, int jump);
	virtual void updateList();
	virtual void deselect();
	virtual void setSelectedText(QString&);
	void setCurrentRow(int row, QItemSelectionModel::SelectionFlags command=QItemSelectionModel::ClearAndSelect);
	QTimer *autosave;


protected:

	static QFont specialfont;
	struct annotationblock *annotation;			// selected annotation block?
	struct annotationblock **annotationlist;		// contains the list of annotation data associated with this dock.

	int file_num,
		relative,
		selected,
		invert_filter,
		hide_nk_triggers,
		hide_bs_triggers;

	QWidget *main_widget;

	QHBoxLayout *h_layout;

	QVBoxLayout *v_layout;

	QLabel *label1;

	QLineEdit *lineSearch;

	QAction *show_between_act,
		*average_annot_act,
		*hide_annot_act,
		*unhide_annot_act,
		*hide_same_annots_act,
		*unhide_same_annots_act,
		*unhide_all_annots_act,
		*hide_all_NK_triggers_act,
		*hide_all_BS_triggers_act,
		*unhide_all_NK_triggers_act,
		*unhide_all_BS_triggers_act,
		*Delete_act;


protected slots:

	virtual void annotation_selected(QListWidgetItem *);
	virtual void delete_annotation();
	virtual void selectionChanged(int);
	virtual void show_editdock(bool);
	void backupAnnotations();


private slots:

	void show_between(bool);
	void average_annot(bool);
	void hide_annot(bool);
	void unhide_annot(bool);
	void hide_same_annots(bool);
	void unhide_same_annots(bool);
	void unhide_all_annots(bool);
	void filter_edited(const QString);
	void hide_all_NK_triggers(bool);
	void hide_all_BS_triggers(bool);
	void unhide_all_NK_triggers(bool);
	void unhide_all_BS_triggers(bool);
};



#endif // ANNOTATIONSWINDOW_H


