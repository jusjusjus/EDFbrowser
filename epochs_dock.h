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




#ifndef EPOCHSWINDOW_H
#define EPOCHSWINDOW_H



#include "annotations_dock.h"	// class UI_Annotationswindow



class UI_Epochswindow : public UI_Annotationswindow
{
	Q_OBJECT

public:
	UI_Epochswindow(QWidget *parent);
	void setup();

	virtual void deselect(void);
	virtual void updateList(annotationblock* annotation, int jump);
	virtual void updateList(void);


protected:
	int epoch,
	    page;	// seconds
	long long start,	// seconds * TIME_DIMENSION
		epochlength,
		pagelength,	// seconds * TIME_DIMENSION
	     	start_offset;	// seconds * TIME_DIMENSION

	void fill_with_epochs();
	void load_epochinfo();


protected slots:

	virtual void selectionChanged(int);
	virtual void annotation_selected(QListWidgetItem *);
	virtual void delete_annotation();
	virtual void show_editdock(bool);
};



#endif // EPOCHSWINDOW_H


