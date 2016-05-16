/*
***************************************************************************
*
*
***************************************************************************
*/



#include<iostream>
#include "annotations_dock.h"



#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
QFont UI_Annotationswindow::specialfont("andale mono", 12, QFont::Normal, true);
#elif defined(Q_OS_WIN32)
QFont UI_Annotationswindow::specialfont("courier", 11, QFont::Normal, true);
#endif



UI_Annotationswindow::UI_Annotationswindow(int file_number, QWidget *w_parent, const char* title) : QListWidget(), file_num(file_number)
{
	QPalette palette;
	mainwindow = (UI_Mainwindow *)w_parent;
	annotationlist = &(mainwindow->annotationlist[file_num]);	// list of annotation data associated with this annodations_dock

	setUniformItemSizes(true);

	docklist = new QDockWidget(title, w_parent);
  	docklist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  	docklist->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  relative = 0;

  selected = -1;

  invert_filter = 0;

  hide_nk_triggers = 0;

  hide_bs_triggers = 0;

  main_widget = new QWidget(w_parent);

  lineSearch = new QLineEdit;
  lineSearch->setMaxLength(16);
  lineSearch->setPlaceholderText("Search");

  setFont(*mainwindow->monofont);
  setAutoFillBackground(true);
  setPalette(palette);

  show_between_act = new QAction("Set timescale from here to next annotation", this);
  hide_annot_act = new QAction("Hide", this);
  unhide_annot_act = new QAction("Unhide", this);
  hide_same_annots_act = new QAction("Hide similar", this);
  unhide_same_annots_act = new QAction("Unhide similar", this);
  unhide_all_annots_act = new QAction("Unhide all", this);
  average_annot_act = new QAction("Average", this);
  hide_all_NK_triggers_act = new QAction("Hide all Nihon Kohden triggers", this);
  hide_all_BS_triggers_act = new QAction("Hide all Biosemi triggers", this);
  unhide_all_NK_triggers_act = new QAction("Unhide all Nihon Kohden triggers", this);
  unhide_all_BS_triggers_act = new QAction("Unhide all Biosemi triggers", this);

  setContextMenuPolicy(Qt::ActionsContextMenu);
  insertAction(NULL, show_between_act);
  insertAction(NULL, hide_annot_act);
  insertAction(NULL, hide_same_annots_act);
  insertAction(NULL, unhide_annot_act);
  insertAction(NULL, unhide_same_annots_act);
  insertAction(NULL, unhide_all_annots_act);
  insertAction(NULL, average_annot_act);
  insertAction(NULL, hide_all_NK_triggers_act);
  insertAction(NULL, unhide_all_NK_triggers_act);
  insertAction(NULL, hide_all_BS_triggers_act);
  insertAction(NULL, unhide_all_BS_triggers_act);

  h_layout = new QHBoxLayout;
  h_layout->addWidget(lineSearch);

  v_layout = new QVBoxLayout(main_widget);
  v_layout->addLayout(h_layout);
  v_layout->addWidget(this);
  v_layout->setSpacing(1);

  docklist->setWidget(main_widget);
  this->setParent(main_widget);


  Delete_act = new QAction(main_widget);
  Delete_act->setShortcut(Qt::Key_Delete);
  Delete_act->setShortcutContext(Qt::WidgetWithChildrenShortcut);		// Annots_dock and epochs_dock have this shortcut.  This setting resolves the ambiguity.
  main_widget->addAction(Delete_act);

  autosave = new QTimer(this);
//  autosave->start(10 * 1000);	// Backup annotations every 30 seconds.
  autosave->start(10 * 60 * 1000);	// Backup annotations every 10 minutes.

  updateList();

  QObject::connect(autosave,			SIGNAL(timeout()),			this, SLOT(backupAnnotations()));
  QObject::connect(this,                       SIGNAL(itemPressed(QListWidgetItem *)),	this, SLOT(annotation_selected(QListWidgetItem *)));
  QObject::connect(this,                       SIGNAL(currentRowChanged(int)), 		this, SLOT(selectionChanged(int)));
  QObject::connect(docklist,                   SIGNAL(visibilityChanged(bool)),		this, SLOT(show_editdock(bool)));
  QObject::connect(hide_annot_act,             SIGNAL(triggered(bool)),			this, SLOT(hide_annot(bool)));
  QObject::connect(unhide_annot_act,           SIGNAL(triggered(bool)),			this, SLOT(unhide_annot(bool)));
  QObject::connect(hide_same_annots_act,       SIGNAL(triggered(bool)),			this, SLOT(hide_same_annots(bool)));
  QObject::connect(unhide_same_annots_act,     SIGNAL(triggered(bool)),			this, SLOT(unhide_same_annots(bool)));
  QObject::connect(unhide_all_annots_act,      SIGNAL(triggered(bool)),			this, SLOT(unhide_all_annots(bool)));
  QObject::connect(average_annot_act,          SIGNAL(triggered(bool)),			this, SLOT(average_annot(bool)));
  QObject::connect(show_between_act,           SIGNAL(triggered(bool)),			this, SLOT(show_between(bool)));
  QObject::connect(hide_all_NK_triggers_act,   SIGNAL(triggered(bool)),			this, SLOT(hide_all_NK_triggers(bool)));
  QObject::connect(hide_all_BS_triggers_act,   SIGNAL(triggered(bool)),			this, SLOT(hide_all_BS_triggers(bool)));
  QObject::connect(unhide_all_NK_triggers_act, SIGNAL(triggered(bool)),			this, SLOT(unhide_all_NK_triggers(bool)));
  QObject::connect(unhide_all_BS_triggers_act, SIGNAL(triggered(bool)),			this, SLOT(unhide_all_BS_triggers(bool)));
  QObject::connect(lineSearch,                 SIGNAL(textEdited(const QString)),	this, SLOT(filter_edited(const QString)));

	QObject::connect(Delete_act, SIGNAL(triggered()), this, SLOT(delete_annotation()));


#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
	UI_Annotationswindow::specialfont.setPixelSize(12);
#elif defined(Q_OS_WIN32)
	UI_Annotationswindow::specialfont.setPixelSize(12);
#endif


}



void UI_Annotationswindow::delete_annotation()
{
// Declaration
	int selected_row;

	annotationblock *annotation;

	QListWidgetItem *item;


// Initialization
	selected_row = currentRow(); // check if row is selected.

	item = currentItem();

	if(item  == 0)			// If no annotation has been selected ..
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "delete_annotation() : No annotation selected.");
		messagewindow.exec();
		return;
	}


	annotation = (annotationblock*) item->data(Qt::UserRole).value<void*>();

	if(annotation == 0)
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "delete_annotation() : annotation is NULL pointer.");
		messagewindow.exec();
		return;
	}

	edfplus_annotation_do_delete( annotationlist, annotation );	// Delete the selected annotation.

	selected_row--;					// Select previous annotation.

	setCurrentRow(selected_row);

	if(selected_row >= 0)			// If there is a previous annotation ..
	{
		annotation = (annotationblock*) currentItem()->data(Qt::UserRole).value<void*>();

  		if(annotation != 0)							// 	.. and if it exists ..
  		{
    			annotation->selected = 1;					//		.. jump to it.
    			annotation->jump = 1;
  		}
	}

	mainwindow->annotations_edited = 1;
	mainwindow->save_act->setEnabled(true);
	updateList();					// Rehash annotationlist into a Qt list.
	mainwindow->maincurve->update();		// Update the window to plot the result.
}



void UI_Annotationswindow::hide_all_NK_triggers(bool)
{
  struct annotationblock *annot;

  annot = *annotationlist;

  hide_nk_triggers = 1;

  while(annot != NULL)
  {
    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hidden = 1;

      annot->hidden_in_list = 1;
    }

    annot = annot->next_annotation;
  }

  updateList();

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::hide_all_BS_triggers(bool)
{
  struct annotationblock *annot;

  annot = *annotationlist;

  hide_bs_triggers = 1;

  while(annot != NULL)
  {
    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hidden = 1;

      annot->hidden_in_list = 1;
    }

    annot = annot->next_annotation;
  }

  updateList();

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::unhide_all_NK_triggers(bool)
{
  struct annotationblock *annot;

  annot = *annotationlist;

  hide_nk_triggers = 0;

  while(annot != NULL)
  {
    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hidden = 0;

      annot->hidden_in_list = 0;
    }

    annot = annot->next_annotation;
  }

  updateList();

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::unhide_all_BS_triggers(bool)
{
  struct annotationblock *annot;

  annot = *annotationlist;

  hide_bs_triggers = 0;

  while(annot != NULL)
  {
    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hidden = 0;

      annot->hidden_in_list = 0;
    }

    annot = annot->next_annotation;
  }

  updateList();

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::filter_edited(const QString text)
{
  int i, number_of_annots, n, len;

  char filter_str[32];

  struct annotationblock *annot;


  annot = *annotationlist;

  number_of_annots = edfplus_annotation_count(&annot);

  if(number_of_annots < 1) return;

  if(text.length() < 1)
  {
	  // for(annot=*annotationlist; annot!=NULL; annot=annot->next_annotation)
    while(annot != NULL)
    {
      if(not (	((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) and hide_nk_triggers) or
		((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) and hide_bs_triggers)  ))
      {
        annot->hidden_in_list = 0;
        annot->hidden = 0;
      }

      annot = annot->next_annotation;
    }

    updateList();

    mainwindow->maincurve->update();

    return;
  }

  strcpy(filter_str, lineSearch->text().toUtf8().data());

  len = strlen(filter_str);

  if(invert_filter == 0)
  {
	  // for(annot=*annotationlist; annot!=NULL; annot=annot->next_annotation)
    while(annot != NULL)
    {
      if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) || ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
      {
        annot->hidden_in_list = 1;

        n = strlen(annot->annotation) - len + 1;

        for(i=0; i<n; i++)
        {
          if(!(strncmp(filter_str, annot->annotation + i, len)))
          {
            annot->hidden_in_list = 0;

            annot->hidden = 0;

            break;
          }
        }
      }

      annot = annot->next_annotation;
    }
  }
  else
  {
	  // for(annot=*annotationlist; annot!=NULL; annot=annot->next_annotation)
    while(annot != NULL)
    {
      if(not (	((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) and hide_nk_triggers) or
		((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) and hide_bs_triggers)  ))
      {
        annot->hidden_in_list = 0;

        n = strlen(annot->annotation) - len + 1;

        for(i=0; i<n; i++)
        {
          if(!(strncmp(filter_str, annot->annotation + i, len)))
          {
            annot->hidden_in_list = 1;
            annot->hidden = 1;

            break;
          }
        }
      }

      annot = annot->next_annotation;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::show_between(bool)
{
  int n;
  long long displaytime;
  struct annotationblock *annot;


  if(count() < 2) return;

  annot = *annotationlist;

  n = currentRow();

  if(n >= (count() - 1)) return;

  while(n--) annot = annot->next_annotation;

  displaytime = annot->onset;

  annot = annot->next_annotation;

  displaytime -= annot->onset;

  if(displaytime < 0) displaytime *= -1;

  if(displaytime < 1000) return;

  mainwindow->pagetime = displaytime;

  annotation_selected(currentItem());
}



void UI_Annotationswindow::hide_annot(bool)
{
  int n;

  struct annotationblock *annot;


  if(count() < 1) return;

  annot = *annotationlist;

  n = currentRow();

  while(n--) annot = annot->next_annotation;

  annot->hidden_in_list = 1;

  annot->hidden = 1;

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::unhide_annot(bool)
{
  int n;

  struct annotationblock *annot;


  if(count() < 1) return;

  annot = *annotationlist;

  n = currentRow();

  while(n--) annot = annot->next_annotation;

  annot->hidden_in_list = 0;

  annot->hidden = 0;

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::hide_same_annots(bool)
{
  int n;

  char str1[MAX_ANNOTATION_LEN],
       str2[MAX_ANNOTATION_LEN];

  struct annotationblock *annot;


  if(count() < 1) return;

  annot = *annotationlist;

  n = currentRow();

  while(n--) annot = annot->next_annotation;

  strcpy(str1, annot->annotation);

  remove_leading_spaces(str1);

  remove_trailing_spaces(str1);

  annot = *annotationlist;

  while(annot != NULL)
  {
    strcpy(str2, annot->annotation);

    remove_leading_spaces(str2);

    remove_trailing_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hidden_in_list = 1;

      annot->hidden = 1;
    }

    annot = annot->next_annotation;
  }

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::unhide_same_annots(bool)
{
  int n;

  char str1[MAX_ANNOTATION_LEN],
       str2[MAX_ANNOTATION_LEN];

  struct annotationblock *annot;


  if(count() < 1) return;

  annot = *annotationlist;

  n = currentRow();

  while(n--) annot = annot->next_annotation;

  strcpy(str1, annot->annotation);

  remove_leading_spaces(str1);

  remove_trailing_spaces(str1);

  annot = *annotationlist;

  while(annot != NULL)
  {
    strcpy(str2, annot->annotation);

    remove_leading_spaces(str2);

    remove_trailing_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hidden_in_list = 0;

      annot->hidden = 0;
    }

    annot = annot->next_annotation;
  }

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::unhide_all_annots(bool)
{
  struct annotationblock *annot;

  annot = *annotationlist;
// for(annot = *annotationlist; annot != NULL; annot = annot->next_annotation)
  while(annot != NULL)
  {
    annot->hidden = 0;

    annot->hidden_in_list = 0;

    annot = annot->next_annotation;
  }

  mainwindow->maincurve->update();
}



void UI_Annotationswindow::average_annot(bool)
{
  if(mainwindow->files_open != 1) return;

  if(!mainwindow->signalcomps)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "First add a signal to the screen.");
    messagewindow.exec();

    return;
  }

  if(mainwindow->annot_editor_active || mainwindow->epoch_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor and/or epoch editor and try again.");
    messagewindow.exec();

    return;
  }

  if(count() < 1)
  {
    return;
  }

  UI_AveragerWindow average_wndw(mainwindow, currentRow());
}



void UI_Annotationswindow::show_editdock(bool visible)
{
	mainwindow->annot_editor_active = (int)visible;
	mainwindow->annotationEditDock->open_close_dock(visible);
}



void UI_Annotationswindow::setSelectedText(QString& annot)
{
// Declaration
	QString annotationText("%1\t %2:%3:%4");

	int hour,
	    minute,
	    second,
	    row;

	QListWidgetItem *item;


// Initialization
	row = currentRow();

	if(row < 0)
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "setSelectedText(): No annotation selected.");
		messagewindow.exec();
		return;
	}

	item = currentItem();


	annotation = (annotationblock*) item->data(Qt::UserRole).value<void*>();

	if(annotation == 0)
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "setSelectedText(): annotation is NULL pointer.");
		messagewindow.exec();
		return;
	}

	hour = (int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION)/ 3600) % 24);
	minute = (int)((((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 3600) / 60);
	second = (int)(((annotation->onset + mainwindow->edfheaderlist[0]->l_starttime) / TIME_DIMENSION) % 60);
	annotationText = annotationText.arg(annot).arg(hour).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0')); 


// Text
	item->setText(annotationText);
	item->setToolTip(annotationText);
	item->setFont(specialfont);
	item->setForeground(Qt::red);

	strncpy(annotation->annotation, annot.toLatin1().data(), MAX_ANNOTATION_LEN);
	annotation->annotation[MAX_ANNOTATION_LEN] = 0;
	annotation->modified = 1;
	
	mainwindow->annotations_edited = 1;
	mainwindow->save_act->setEnabled(true);
}



void UI_Annotationswindow::updateList(annotationblock*, int)	// updates a single notation and may jump to the next/previous
{ updateList(); }



void UI_Annotationswindow::updateList()
{
	char str[MAX_ANNOTATION_LEN + 32],	// this contains the final string?
	     *str_tmp;

	int i,
	    len,
	    sequence_nr=0,
	    jump=0,
	    modified=0;

	annotationblock *selected_annotation;


	QListWidgetItem *listitem;

	QString string;

	QByteArray ba;

	selected = -1;

	selected_annotation = 0;

	clear();					// Empty out the Qt list.

	edfplus_annotation_sort(annotationlist);	// Sort annotationlist.

	annotation = *annotationlist;			// Get the first annotation.

	while(annotation != 0)	// If it exists ..
	{
		if(annotation->hidden_in_list)	//	.. check if it's hidden ..
		{
			annotation = annotation->next_annotation;	// Select the next annotation ..
			sequence_nr++;				// 					(sequence_nr sets the hash of annotationlist.)
			continue;					// 		.. and move back to the while loop.
		}

		string = QString::fromUtf8(annotation->annotation);	// Get a QString of the annotation.

		ba = string.toUtf8();
		str_tmp = ba.data();

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
			if((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) < 0LL)	// before onset
			{
				snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	-%2i:%02i:%02i",//.%04i",
								(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION)/ 3600),
								(int)(((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
								(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 60));//,
//								(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) % TIME_DIMENSION) / 1000LL));
			}
			else											// after onset
			{
				snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "	%3i:%02i:%02i",//.%04i",
								(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION)/ 3600),
								(int)((((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
								(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 60));//,
//								(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) % TIME_DIMENSION) / 1000LL));
			}
		}
		else		// if time not relative to start, but absolute time.
		{
			snprintf(str, MAX_ANNOTATION_LEN + 32, "	%3i:%02i:%02i",//.%04i",
							(int)((((annotation->onset + mainwindow->edfheaderlist[file_num]->l_starttime) / TIME_DIMENSION)/ 3600) % 24),
							(int)((((annotation->onset + mainwindow->edfheaderlist[file_num]->l_starttime) / TIME_DIMENSION) % 3600) / 60),
							(int)(((annotation->onset + mainwindow->edfheaderlist[file_num]->l_starttime) / TIME_DIMENSION) % 60));//,
//							(int)(((annotation->onset + mainwindow->edfheaderlist[file_num]->l_starttime) % TIME_DIMENSION) / 1000LL));
		}

		str[MAX_ANNOTATION_LEN + 31] = 0;

		remove_trailing_zeros(str);

		string.append(QString::fromLatin1(str));

		listitem = new QListWidgetItem(string, this);

		QVariant annotVar = qVariantFromValue( (void*) annotation );
		listitem->setData(Qt::UserRole, annotVar);	// Store a pointer to th annotation.

		if(annotation->modified == 1)	// if modified, change font of this listitem.
		{
			listitem->setFont(specialfont);
			listitem->setForeground(Qt::red);
			modified = 1;
		}

		// Set the tool tip of this annotation:	(That's when you hover over the item with the mouse.)
		if((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) < 0LL)
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: -%i:%02i:%02i.%04i",
							(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION)/ 3600),
							(int)(((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
							(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 60),
							(int)((-(annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) % TIME_DIMENSION) / 1000LL));
		}
		else
		{
			snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: %2i:%02i:%02i.%04i",
							(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION)/ 3600),
							(int)((((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
							(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) / TIME_DIMENSION) % 60),
							(int)(((annotation->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) % TIME_DIMENSION) / 1000LL));
		}

		if(annotation->duration[0] != 0)
		{
			snprintf(str + strlen(str), (MAX_ANNOTATION_LEN + 32) / 2, "\nduration: %s",annotation->duration);
		}

		str[MAX_ANNOTATION_LEN + 31] = 0;

		remove_trailing_zeros(str);

		strcat(str, "\n\n");

		string = QString::fromLatin1(str);

		string.append(QString::fromUtf8(annotation->annotation));

		listitem->setToolTip(string);

		if(annotation->selected)
		{
			selected = sequence_nr;	// Remember the index of the selected annotation.
			selected_annotation = annotation;
			annotation->selected = 0;

			if(annotation->jump)	// If the item commands to jump, ..
			{
				jump = 1;		// The list is ordered a jump.
				annotation->jump = 0;	// The command is deactivated.
			}
		}

		annotation = annotation->next_annotation;

		sequence_nr++;
	}	// while(annotation != NULL)

	if( mainwindow->annot_editor_active or mainwindow->epoch_editor_active )
	{
		if(selected >= 0)		// An annotation has been selected.
		{
			QListWidget::setCurrentRow(selected, QItemSelectionModel::ClearAndSelect);	// setCurrentRow to selected item.

			mainwindow->annotationEditDock->set_selected_annotation( selected_annotation );
			selected = -1;

			if(jump)
			{
				jump = 0;
				annotation_selected( currentItem() );
			}

		}

		if(modified)
		{
			mainwindow->annotations_edited = 1;
			mainwindow->save_act->setEnabled(true);	// ask to save before closing.
		}
	}
}



void UI_Annotationswindow::selectionChanged(int currentRow)
{
//	std::cout << "selectionChanged to " << currentRow << std::endl;
// Declaration
	long long new_viewtime;

	double fraction,
	       fraction_2,
	       duration,
	       window_length;
	
	QListWidgetItem *item;

// Parameter check
	if(currentRow < 0) return;				// No annotation selected.

// Initialization
	item = currentItem();	

	if(item == 0)			// If no annotation has been selected ..
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "selectionChanged() : No annotation selected.");
		messagewindow.exec();
		return;
	}

	annotation = (annotationblock*) item->data(Qt::UserRole).value<void*>();

	if(annotation == 0)			// If no annotation has been selected ..
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "selectionChanged() : Annotation is NULL pointer.");
		messagewindow.exec();
		return;
	}

  	if(mainwindow->annot_editor_active)
		mainwindow->annotationEditDock->set_selected_annotation(annotation);	//   ...(annotationblock *annotation)


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



void UI_Annotationswindow::annotation_selected(QListWidgetItem *item)
{
	if( item == 0 ) return;

	long long new_viewtime;

	double fraction,
	       fraction_2,
	       duration,
	       window_length;

	// select the right annotation:
	annotation = (annotationblock*) item->data(Qt::UserRole).value<void*>();

	if( annotation == 0 ) return;


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



void UI_Annotationswindow::deselect()
{
	QListWidget::setCurrentRow(-1, QItemSelectionModel::Clear);	// Sets the row in the QList
	annotation = 0;
}



void UI_Annotationswindow::setCurrentRow(int row, QItemSelectionModel::SelectionFlags command)
{
	QListWidgetItem *item;

	void *annot;

	if(row < 0)
	{
		deselect();
		return;
	}
	else
	{
		QListWidget::setCurrentRow(row, command);	// Sets the row in the QList
	}

	item = currentItem();

	if(item == 0)	// Invalid item selected.
	{
		deselect();
		return;
	}

	annot = item->data(Qt::UserRole).value<void*>();

	if(annot == 0)
	{
		deselect();
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "setCurrentRow() : Selected annotation is NULL pointer.");
		messagewindow.exec();
		return;
	}

	annotation = (annotationblock*) annot;
}



void UI_Annotationswindow::backupAnnotations()
{
//	std::cout << "Backing up " << std::endl;
	UI_ExportAnnotationswindow exportAnnotsDialog(mainwindow, &annotationlist[0]);
	exportAnnotsDialog.backup( docklist->windowTitle().toLatin1().data() );
}



