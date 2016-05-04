/****************************************************************************
*
*
****************************************************************************/



#include "annotations_dock.h"
#include "Event.h"
class Event;



UI_AnnotationEditwindow::UI_AnnotationEditwindow(int file_number, QWidget *w_parent) : default_custom_annotation("")
{
	mainwindow = (UI_Mainwindow *)w_parent;

	file_num = file_number;

	annotation = mainwindow->annotationlist[file_num];

	dockedit = new QDockWidget(mainwindow);
	dockedit->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	dockedit->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

	annot_edit_widget = new QWidget;
	annot_edit_widget->setMinimumSize(640, 45);
	dockedit->setWidget(annot_edit_widget);
	dockedit->hide();

	QObject::connect(dockedit, SIGNAL(visibilityChanged(bool)), this, SLOT(open_close_dock(bool)));	//
}



void UI_AnnotationEditwindow::setup()
{
	// ### signal type detector ###
	signaltypes = new Signaltypes(this);
	// ### signal type detector ###


	// ### Annotation ###
	annot_descript_lineEdit = new QLineEdit();
	annot_descript_lineEdit->setGeometry(90, 10, 150, 25);
	annot_descript_lineEdit->setPlaceholderText("Enter Annotation Text");

	onsetLabel = new QLabel();
	onsetLabel->setGeometry(250, 10, 48, 25);
	onsetLabel->setText("Onset");

	posNegTimebox = new QComboBox();
	posNegTimebox->setGeometry(300, 10, 35, 25);
	posNegTimebox->setEditable(false);
	posNegTimebox->addItem("+");
	posNegTimebox->addItem("-");

	onset_daySpinbox = new QSpinBox();
	onset_daySpinbox->setGeometry(335, 10, 45, 25);
	onset_daySpinbox->setRange(0, 99);
	onset_daySpinbox->setSingleStep(1);
	onset_daySpinbox->setValue(0);

	onset_timeEdit = new QTimeEdit();
	onset_timeEdit->setGeometry(380, 10, 100, 25);
	onset_timeEdit->setDisplayFormat("hh:mm:ss.zzz");
	onset_timeEdit->setMinimumTime(QTime(-1, 0, 0, 0));

	durationLabel = new QLabel();
	durationLabel->setGeometry(490, 10, 58, 25);
	durationLabel->setText("Duration");

	duration_spinbox = new QDoubleSpinBox();
	duration_spinbox->setGeometry(550, 10, 120, 25);
	duration_spinbox->setRange(-1.0, 10000.0);
	duration_spinbox->setSingleStep(1.0);
	duration_spinbox->setDecimals(3);
	duration_spinbox->setSuffix(" sec");
	duration_spinbox->setValue(-1.0);
	// ### Annotation ###


	// ### Annotation container ###
	QHBoxLayout *annot_specs = new QHBoxLayout();
	annot_specs->addWidget(annot_descript_lineEdit);
	annot_specs->addWidget(onsetLabel);
	annot_specs->addWidget(posNegTimebox);
	annot_specs->addWidget(onset_daySpinbox);
	annot_specs->addWidget(onset_timeEdit);
	annot_specs->addWidget(durationLabel);
	annot_specs->addWidget(duration_spinbox);
	// ### Annotation container ###
	//
	QGridLayout* grid = new QGridLayout(annot_edit_widget);
	grid->addLayout(annot_specs, 0, 0);
}



void UI_AnnotationEditwindow::open_close_dock(bool set_visible)
{
	if(set_visible)
	{
		signaltypes->registerSignaltypes(false);	// ask = false
		mainwindow->show_annot_markers = 1;
		if(mainwindow->annotationlist_backup == NULL)
			mainwindow->annotationlist_backup = edfplus_annotation_copy_list(&mainwindow->annotationlist[0]);
	}
}



void UI_AnnotationEditwindow::modifyButtonClicked()
{
	annotation = edfplus_annotation_item(&mainwindow->annotationlist[file_num], annot_num);

	annotation->onset =	onset_daySpinbox->value() * 86400 * TIME_DIMENSION;
	annotation->onset += onset_timeEdit->time().hour() * 3600 * TIME_DIMENSION;
	annotation->onset += onset_timeEdit->time().minute() * 60 * TIME_DIMENSION;
	annotation->onset += onset_timeEdit->time().second() * TIME_DIMENSION;
	annotation->onset += onset_timeEdit->time().msec() * (TIME_DIMENSION / 1000);

	if(posNegTimebox->currentIndex() == 1)
	{
		annotation->onset = -(annotation->onset);
	}

	annotation->onset += mainwindow->edfheaderlist[file_num]->starttime_offset;

	if(duration_spinbox->value() > 0.)
	{
		snprintf(annotation->duration, 16, "%f", duration_spinbox->value());
	}
	else
	{
		annotation->duration[0] = 0;
	}

	strncpy(annotation->annotation, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

	annotation->annotation[MAX_ANNOTATION_LEN] = 0;

	annotation->modified = 1;

	annotation->selected = 1;

	mainwindow->annotations_edited = 1;

	mainwindow->annotations_dock[file_num]->updateList();

	mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::createButtonClicked()
{
	annotation = (struct annotationblock *)calloc(1, sizeof(struct annotationblock));

	annotation->file_num = file_num;

	annotation->onset =	onset_daySpinbox->value() * 86400 * TIME_DIMENSION;		// day
	annotation->onset += onset_timeEdit->time().hour() * 3600 * TIME_DIMENSION;		// hours
	annotation->onset += onset_timeEdit->time().minute() * 60 * TIME_DIMENSION;		// minutes
	annotation->onset += onset_timeEdit->time().second() * TIME_DIMENSION;		// seconds
	annotation->onset += onset_timeEdit->time().msec() * (TIME_DIMENSION / 1000);		// mseconds
	if(posNegTimebox->currentIndex() == 1) annotation->onset = -(annotation->onset);
	annotation->onset += mainwindow->edfheaderlist[file_num]->starttime_offset;

	if(duration_spinbox->value() > 0.0) snprintf(annotation->duration, 16, "%f", duration_spinbox->value());
	else annotation->duration[0] = 0;

	strncpy(annotation->annotation, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);
	annotation->annotation[MAX_ANNOTATION_LEN] = 0;

	annotation->former_annotation = 0;
	annotation->next_annotation   = 0;

	annotation->modified = 1;

	edfplus_annotation_add_item(&mainwindow->annotationlist[file_num], annotation);

	mainwindow->annotations_edited = 1;

	mainwindow->annotations_dock[file_num]->updateList();

	mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::annotEditSetOnset(long long onset)
{
	QTime ta;

	if(onset < 0LL)
	{
		onset_daySpinbox->setValue((-(onset)) / TIME_DIMENSION / 86400);

		ta.setHMS(((-(onset)) / TIME_DIMENSION / 3600) % 24,
						(((-(onset)) / TIME_DIMENSION) % 3600) / 60,
						((-(onset)) / TIME_DIMENSION) % 60,
						((-(onset)) % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(1);
	}
	else
	{
		onset_daySpinbox->setValue(onset / TIME_DIMENSION / 86400);

		ta.setHMS((onset / TIME_DIMENSION / 3600) % 24,
						((onset / TIME_DIMENSION) % 3600) / 60,
						(onset / TIME_DIMENSION) % 60,
						(onset % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(0);
	}

	onset_timeEdit->setTime(ta);
}



void UI_AnnotationEditwindow::annotEditSetDuration(long long duration)
{
	double tmp; 
	tmp = duration;
	tmp /= TIME_DIMENSION; 
	duration_spinbox->setValue(tmp);
}



void UI_AnnotationEditwindow::set_selected_annotation(int file_nr, int annot_nr)
{
	long long l_tmp;

	QTime ta;

	file_num = file_nr;
	annot_num = annot_nr;

	annotation = edfplus_annotation_item(&mainwindow->annotationlist[file_num], annot_num);	// retreive the selected annotation
	if(annotation == NULL) return;

	annot_descript_lineEdit->setText( QString::fromUtf8(annotation->annotation) );

	l_tmp = annotation->onset - mainwindow->edfheaderlist[annotation->file_num]->starttime_offset;

	if(l_tmp < 0LL)	// annotation before recording begin ...
	{
		onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

		ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
						(((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
						((-(l_tmp)) / TIME_DIMENSION) % 60,
						((-(l_tmp)) % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(1);		// ... gets a negative sign.
	}
	else					// annotation after recording begin ...
	{
		onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

		ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
						((l_tmp / TIME_DIMENSION) % 3600) / 60,
						(l_tmp / TIME_DIMENSION) % 60,
						(l_tmp % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(0);		// ... gets a positive sign.
	}
	onset_timeEdit->setTime(ta);

	if(strlen(annotation->duration))		// annotation->duration type is string.
	{
		duration_spinbox->setValue(atof(annotation->duration));
	}
	else
	{
		duration_spinbox->setValue(-1);		// if string length = 0, set -1 as value.
	}
}



void UI_AnnotationEditwindow::set_selected_annotation(struct annotationblock *annot)
{
	long long l_tmp;

	QTime ta;

	annotation = annot;

	file_num = annot->file_num;

	annot_descript_lineEdit->setText(QString::fromUtf8(annotation->annotation));

	l_tmp = annotation->onset - mainwindow->edfheaderlist[annotation->file_num]->starttime_offset;

	if(l_tmp < 0LL)
	{
		onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

		ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
				(((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
				((-(l_tmp)) / TIME_DIMENSION) % 60,
				((-(l_tmp)) % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(1);
	}
	else
	{
		onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

		ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
				((l_tmp / TIME_DIMENSION) % 3600) / 60,
				(l_tmp / TIME_DIMENSION) % 60,
				(l_tmp % TIME_DIMENSION) / 10000);

		posNegTimebox->setCurrentIndex(0);
	}
	onset_timeEdit->setTime(ta);

	if(strlen(annotation->duration))
	{
		duration_spinbox->setValue(atof(annotation->duration));
	}
	else
	{
		duration_spinbox->setValue(-1);
	}
}



void UI_AnnotationEditwindow::deselect()
{
	annotation = NULL;
}




void UI_AnnotationEditwindow::eventSelectionMenu(int signal_nr, int x, int y)
{
	this->signal_nr = signal_nr;		// set signal number of annotation.
	signaltypes->exec(signal_nr, x, y);	// ask for the type.  triggers
}



void UI_AnnotationEditwindow::annotEditSetAnnotation()
{
	bool ok;
    	default_custom_annotation =  QInputDialog::getText(mainwindow, tr("Set annotation"),
                                         	tr(""), QLineEdit::Normal,
                                         	default_custom_annotation, &ok);

	if(ok)
	{
		annot_descript_lineEdit->setText( default_custom_annotation );
		createButtonClicked();
	}
	mainwindow->remove_crosshairs();
}



void UI_AnnotationEditwindow::annotEditSetAnnotation(QAction* action)
{
	Event *event = static_cast<Event*>(action);
	if( not event->check(duration_spinbox->value()) ) return;

	annot_descript_lineEdit->setText( event->text() );
	createButtonClicked();
	mainwindow->remove_crosshairs();
}



