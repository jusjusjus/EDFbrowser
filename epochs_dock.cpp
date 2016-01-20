

#include<iostream>
#include "epochs_dock.h"



UI_Epochswindow::UI_Epochswindow(QWidget *w_parent) : UI_Annotationswindow(0, w_parent, "Epochs"), epoch(30), page(epoch), epochlength((long long)epoch*TIME_DIMENSION), pagelength(epochlength), start_offset(0LL)
{
	annotationlist = &(mainwindow->epochlist[0]);
	load_epochinfo();
	updateList();							// This overrides the call to updateList() in the base constructor.
}



void UI_Epochswindow::load_epochinfo()
{
	char cfg_path[MAX_PATH_LENGTH];
	QString errorStr;
	int errorLine,
		errorColumn;
	QDomDocument domDocument;
	QDomElement root, child;

	configpath(cfg_path, "Epochsconfig.xml");
	QFile file(cfg_path);
	if(! file.open(QFile::ReadOnly | QFile::Text) )
						{ QMessageBox::warning(mainwindow, tr("Epochsconfig"), tr("Cannot read file %1:\n%2.").arg(cfg_path).arg(file.errorString())); return; }

	if(! domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn) )
						{ QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr)); return; }


	root = domDocument.documentElement();
	if( root.tagName() != "Epochsconfig" )
						{ QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("%1 is not a Epochsconfig.xml file.").arg(cfg_path) ); return; }

	else if( root.hasAttribute("version") && root.attribute("version") != "0.0.1" )
						{ QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("%1 is not Epochsconfig.xml version 0.0.1 file.").arg(cfg_path) ); return; }


	child = root.firstChildElement("Epoch");
	if( child.isNull() or (not child.hasAttribute("Seconds")) ) { QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("Epoch is not properly defined in %1.").arg(cfg_path) ); return; }

	else
	{
		epoch = atoi( child.attribute("Seconds").toLatin1().data() );
		epochlength = (long long)epoch*TIME_DIMENSION;
	}

	child = root.firstChildElement("Page");
	if( not child.isNull() )
	{
		page = 0;
		if( child.hasAttribute("Seconds") ) page +=    atoi( child.attribute("Seconds").toLatin1().data() );
		if( child.hasAttribute("Minutes") ) page += 60*atoi( child.attribute("Minutes").toLatin1().data() );

		pagelength = (long long)page*TIME_DIMENSION;
	}
}



void UI_Epochswindow::setup()
{
	if(annotationlist[0] == NULL)
	{
		fill_with_epochs();
		updateList();
	}
}



void UI_Epochswindow::fill_with_epochs()
{
	struct annotationblock* annotations;
	long long onset, end;
	unsigned number_of_epochs;

	onset = mainwindow->edfheaderlist[0]->starttime_offset + this->start_offset;
	end = mainwindow->edfheaderlist[0]->long_data_record_duration * mainwindow->edfheaderlist[0]->datarecords;

	number_of_epochs = (end-onset)/epochlength;
	
	annotations = (struct annotationblock *)calloc(number_of_epochs, sizeof(struct annotationblock));

	for(unsigned i=0; i<number_of_epochs; i++)
	{
		annotations[i].onset = onset;
		snprintf(annotations[i].duration, 16, "%f", (double)epoch);
	
		strncpy(annotations[i].annotation, "???", MAX_ANNOTATION_LEN);
		annotations->annotation[MAX_ANNOTATION_LEN] = 0;
	
		edfplus_annotation_add_item(annotationlist, &(annotations[i]));

		onset += epochlength;			// @i=0: onset = mainwindow->edfheaderlist[0]->starttime_offset + this->start_offset;
	}	
}



void UI_Epochswindow::selectionChanged(int currentRow)
{
	int n;
	long long new_viewtime;
	double fraction, fraction_2, duration, window_length;

	if(currentRow < 0) return;				// No annotation selected.

	// select the right annotation:
	annotation = *annotationlist;						// first annotation in the list.  What is file_num?
	n = currentRow;					// yields the annotation number.

	//annotation = edfplus_annotation_item(annotationlist, n);		// why doesn't this work?
	//if(annotation == NULL) return;								// ... ???

  	if(mainwindow->annot_editor_active)
		mainwindow->annotationEditDock->set_selected_annotation(file_num, n);	//   ...(int file_nr, int annot_nr)

	while(n--) annotation = annotation->next_annotation;				// Linear list seek starting from the first annotation, until number n.


	// find fraction so that left and right margin to the annotation (duration) are equal.
	duration = atof(annotation->duration);
	window_length = (double)mainwindow->pagetime/(double)TIME_DIMENSION;
	fraction = 0.5 * (1. - duration / window_length);
	fraction_2 = 1.-fraction;
	if(fraction < 0.)		// if not possible, center everything.
	{
		fraction = 0.5;
		fraction_2 = 0.99;
	}

	// set the new viewtime
	new_viewtime = annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset - (long long)(mainwindow->pagetime * fraction); // new time:  onset - start - fraction * page;

	if(mainwindow->viewtime_sync == VIEWTIME_SYNCED_OFFSET)
	{
		for(int i=0; i<mainwindow->files_open; i++)			// for each file
		{
			mainwindow->edfheaderlist[i]->viewtime = new_viewtime;
		}
	}

	if(mainwindow->viewtime_sync == VIEWTIME_UNSYNCED)
	{
		mainwindow->edfheaderlist[file_num]->viewtime = new_viewtime;
	}

	if( (mainwindow->viewtime_sync == VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync == VIEWTIME_USER_DEF_SYNCED) )
	{
		new_viewtime -= mainwindow->edfheaderlist[file_num]->viewtime;
	  	for(int i=0; i<mainwindow->files_open; i++)
	  	{
	    		mainwindow->edfheaderlist[i]->viewtime += new_viewtime;
	  	}
	}

	mainwindow->maincurve->set_crosshairs(fraction, fraction_2);		// sets the crosshairs to match annotation

	mainwindow->setup_viewbuf();
	mainwindow->set_pagetime(-1, 0);
}



void UI_Epochswindow::annotation_selected(QListWidgetItem * item)
{
	int n;
	long long new_viewtime, epochstart;

	annotation = *annotationlist;				// first annotation in the list.	What is file_num?
	n = item->data(Qt::UserRole).toInt();						// yields the annotation number.

	if(mainwindow->epoch_editor_active)
		mainwindow->epochEditDock->set_selected_annotation(n);	//	 ...(int file_nr, int annot_nr)

	while(n--) annotation = annotation->next_annotation;				// Linear list seek starting from the first annotation, until number n.


	n = page/epoch;

	new_viewtime = annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset - (long long)(n/2) * epochlength; // new time:	onset - start; the beginning of the epoch.

	epochstart = (long long)(n/2) * epochlength;

	if(mainwindow->viewtime_sync == VIEWTIME_SYNCED_OFFSET)
	{
		for(int i=0; i<mainwindow->files_open; i++)			// for each file
		{
			mainwindow->edfheaderlist[i]->viewtime = new_viewtime;
		}
	}

	if(mainwindow->viewtime_sync == VIEWTIME_UNSYNCED)
	{
		mainwindow->edfheaderlist[0]->viewtime = new_viewtime;
	}

	if( (mainwindow->viewtime_sync == VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync == VIEWTIME_USER_DEF_SYNCED) )
	{
		new_viewtime -= mainwindow->edfheaderlist[0]->viewtime;
		for(int i=0; i<mainwindow->files_open; i++)
		{
			mainwindow->edfheaderlist[i]->viewtime += new_viewtime;
		}
	}
	
	mainwindow->epochstart = epochstart;
	mainwindow->setup_viewbuf();
	mainwindow->set_pagetime(pagelength, 2, epochlength);
}



void UI_Epochswindow::updateList(annotationblock *annotation, int jump)	// updates a single notation and may jump to the next/previous
{
	char	str[MAX_ANNOTATION_LEN + 32],	// this contains the final string?
		*str_tmp;

	int 	len,
		sequence_nr=0,
		modified=0;

	QListWidgetItem *listitem;
	QString string;


	selected = -1;

	string = QString::fromUtf8(annotation->annotation);


	str_tmp = string.toUtf8().data();

	len = 0;
	for(int i=0; ; i++)
	{
		if(str_tmp[i] == 0) break;
		if(((( (unsigned char *) str_tmp)[i])&224) == 192) len++;
	}

	for(int i=0; i<len; i++) string.append(' ');

			// print the date into str, starting at MAX_ANNOTATION_LEN?
	if(relative)	// time relative to start.
	{
		if((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) < 0LL)	// before onset
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	-%2i:%02i:%02i",
				(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
				(int)(((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
				(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
		}
		else											// after onset
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	%3i:%02i:%02i",
				(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
				(int)((((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
				(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
		}
	}
	else		// if time not relative to start, but absolute time.
	{
		snprintf(str, MAX_ANNOTATION_LEN + 32, "	%3i:%02i:%02i",
			(int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION)/ 3600) % 24),
			(int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 3600) / 60),
			(int)(((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 60));
	}

	str[MAX_ANNOTATION_LEN + 31] = 0;

	remove_trailing_zeros(str);

	string.append(QString::fromLatin1(str));

	listitem = new QListWidgetItem(string, this);

	listitem->setData(Qt::UserRole, QVariant(sequence_nr));

	if(annotation->modified == 1)	// if modified, change font of this listitem.
	{
		listitem->setFont(specialfont);
		listitem->setForeground(Qt::red);
		modified = 1;
	}

	// Set the tool tip of this annotation:	(That's when you hover over the item with the mouse.)
	if((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) < 0LL)
	{
		snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: -%i:%02i:%02i",
			(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
			(int)(((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
			(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
	}
	else
	{
		snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: %2i:%02i:%02i",
			(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
			(int)((((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
			(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
	}

	str[MAX_ANNOTATION_LEN + 31] = 0;

	remove_trailing_zeros(str);

	strcat(str, "\n");

	string = QString::fromLatin1(str);

	string.append(QString::fromUtf8(annotation->annotation));

	listitem->setToolTip(string);

	if(annotation->selected)
	{
		selected = sequence_nr;

		annotation->selected = 0;

		if(annotation->jump)		// if the item commands to jump,
		{
			jump = 1;		// ... the list is ordered a jump.

			annotation->jump = 0;	// The command is deactivated.
		}
	}

	annotation = annotation->next_annotation;

	sequence_nr++;

	if(mainwindow->epoch_editor_active)
	{
		if(selected >= 0)
		{
			setCurrentRow(selected, QItemSelectionModel::ClearAndSelect);
			mainwindow->epochEditDock->set_selected_annotation(selected); // this could possibly be done by class inheritance.

			if(jump)
			{
				jump = 0;

				annotation_selected( currentItem() );
			}

			selected = -1;
		}

		if(modified)
		{
			mainwindow->annotations_edited = 1;
			mainwindow->save_act->setEnabled(true);	// ask to save before closing.
		}
	}
}



void UI_Epochswindow::updateList(void)
{
	char str[MAX_ANNOTATION_LEN + 32],	// this contains the final string?
		 *str_tmp;

	int i,
		len,
		sequence_nr=0,
		jump=0,
		modified=0;

	QListWidgetItem *listitem;

	QString string;


	selected = -1;

	clear();

	// edfplus_annotation_sort(annotationlist);			// Epochs always remain sorted.

	annotation = this->annotationlist[0];

	while(annotation != NULL)
	{
		if(annotation->hidden_in_list)
		{
			annotation = annotation->next_annotation;
			sequence_nr++;
			continue;	// hidden annotations are not shown.
		}


		string = QString::fromUtf8(annotation->annotation);


		str_tmp = string.toUtf8().data();

		len = 0;
		for(i=0; ; i++)
		{
			if(str_tmp[i] == 0)	break;

			if(((( (unsigned char *) str_tmp)[i])&224) == 192)	len++;
		}

		for(i=0; i<len; i++)	string.append(' ');

	// print the date into str, starting at MAX_ANNOTATION_LEN?
		if(relative)	// time relative to start.
		{
			if((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) < 0LL)	// before onset
			{
				snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	-%2i:%02i:%02i",
					(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
					(int)(((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
					(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
			}
			else											// after onset
			{
				snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	%3i:%02i:%02i",
					(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
					(int)((((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
					(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
			}
		}
		else		// if time not relative to start, but absolute time.
		{
			snprintf(str, MAX_ANNOTATION_LEN + 32, "	%3i:%02i:%02i",
				(int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION)/ 3600) % 24),
				(int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 3600) / 60),
				(int)(((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 60));
		}

		str[MAX_ANNOTATION_LEN + 31] = 0;

		remove_trailing_zeros(str);

		string.append(QString::fromLatin1(str));

		listitem = new QListWidgetItem(string, this);

		listitem->setData(Qt::UserRole, QVariant(sequence_nr));

		if(annotation->modified == 1)	// if modified, change font of this listitem.
		{
			listitem->setFont(specialfont);
			listitem->setForeground(Qt::red);
			modified = 1;
		}

		// Set the tool tip of this annotation:	(That's when you hover over the item with the mouse.)
		if((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) < 0LL)
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "-%i:%02i:%02i",
				(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
				(int)(((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
				(int)((-(annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
		}
		else
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "%2i:%02i:%02i",
				(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION)/ 3600),
				(int)((((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
				(int)(((annotation->onset - mainwindow->edfheaderlist[0]->starttime_offset) / TIME_DIMENSION) % 60));
		}

		str[MAX_ANNOTATION_LEN + 31] = 0;

		remove_trailing_zeros(str);

		strcat(str, "\n");

		string = QString::fromLatin1(str);

		string.append(QString::fromUtf8(annotation->annotation));

		listitem->setToolTip(string);

		if(annotation->selected)
		{
			selected = sequence_nr;

			annotation->selected = 0;

			if(annotation->jump)	// if the item commands to jump ...
			{
				jump = 1;		//	the list is ordered a jump.

				annotation->jump = 0;	// The command is deactivated.
			}
		}

		annotation = annotation->next_annotation;

		sequence_nr++;
	}

	if(mainwindow->epoch_editor_active)
	{
		if(selected >= 0)
		{
			setCurrentRow(selected, QItemSelectionModel::ClearAndSelect);
			mainwindow->epochEditDock->set_selected_annotation(selected); // this could possibly be done by class inheritance.

			if(jump)
			{
				jump = 0;
				annotation_selected( currentItem() );
			}
			selected = -1;
		}

		if(modified)
		{
			mainwindow->annotations_edited = 1;
			mainwindow->save_act->setEnabled(true);	// ask to save before closing.
		}
	}
}



void UI_Epochswindow::deselect()
{
	UI_Annotationswindow::deselect();
	mainwindow->stiffness = 0;
	if(mainwindow->epochEditDock) mainwindow->epochEditDock->deselect();
}



void UI_Epochswindow::delete_annotation()
{	
	QString str("???");
	setSelectedText( str );
	setCurrentRow(currentRow()-1);
	mainwindow->maincurve->update();
}



void UI_Epochswindow::show_editdock(bool visible)
{
	mainwindow->epoch_editor_active = (int)visible;
	mainwindow->epochEditDock->open_close_dock(visible);
}



