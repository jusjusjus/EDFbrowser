

#include "color_options.h"




ColorOptions::ColorOptions(QWidget *w_parent) : QWidget() // tab_number=0
{

	mainwindow = (UI_Mainwindow *)w_parent;

	QVBoxLayout *vbox = new QVBoxLayout;
	this->setLayout(vbox);

	QGridLayout *grid = new QGridLayout;
	vbox->addLayout(grid);

	label_bg = new QLabel("Background color");

	BgColorButton = new SpecialButton;
	BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

	label_sruler = new QLabel("Small ruler color");

	SrColorButton = new SpecialButton(this);
	SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

	label3 = new QLabel("Big ruler color");

	BrColorButton = new SpecialButton;
	BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

	label4 = new QLabel("Mouse rectangle color");

	MrColorButton = new SpecialButton;
	MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

	label5 = new QLabel("Text color");

	TxtColorButton = new SpecialButton;
	TxtColorButton->setColor(mainwindow->maincurve->text_color);

	label_sigcolor = new QLabel("Signals color");

	SigColorButton = new SpecialButton;
	SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);

	QHBoxLayout *label7_layout = new QHBoxLayout;
	label7 = new QLabel("Baseline color");

	chbox_show_baselines = new QCheckBox;
	chbox_show_baselines->setTristate(false);
	if(mainwindow->show_baselines)
	{
		chbox_show_baselines->setCheckState(Qt::Checked);
	}
	else
	{
		chbox_show_baselines->setCheckState(Qt::Unchecked);
	}

	label7_layout->addWidget(label7);
	label7_layout->addWidget(chbox_show_baselines);

	BaseColorButton = new SpecialButton;
	BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

	label8 = new QLabel("Crosshair color");

	Crh1ColorButton = new SpecialButton;
	Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);

	label9 = new QLabel("2nd Crosshair color");

	Crh2ColorButton = new SpecialButton;
	Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);

	label10 = new QLabel("Floating ruler color");

	FrColorButton = new SpecialButton;
	FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);

	QHBoxLayout *label_amarker_layout = new QHBoxLayout;
	label_amarker = new QLabel("Annotation marker");

	checkbox2 = new QCheckBox;
	checkbox2->setTristate(false);
	if(mainwindow->show_annot_markers)
	{
		checkbox2->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox2->setCheckState(Qt::Unchecked);
	}

	label_amarker_layout->addWidget(label_amarker);
	label_amarker_layout->addWidget(checkbox2);

	AnnotMkrButton = new SpecialButton(this);
	AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

	label11 = new QLabel("Print in grayscale");

	checkbox1 = new QCheckBox;
	checkbox1->setTristate(false);
	if(mainwindow->maincurve->blackwhite_printing)
	{
		checkbox1->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox1->setCheckState(Qt::Unchecked);
	}

	label13 = new QLabel("Clip signals to pane");

	checkbox4 = new QCheckBox;
	checkbox4->setTristate(false);
	if(mainwindow->clip_to_pane)
	{
		checkbox4->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox4->setCheckState(Qt::Unchecked);
	}


	grid->addWidget(label_bg, 0, 1);
	grid->addWidget(BgColorButton, 0, 2);
	grid->addWidget(label_sruler, 1, 1);
	grid->addWidget(SrColorButton, 1, 2);
	grid->addWidget(label3, 2, 1);
	grid->addWidget(BrColorButton, 2, 2);
	grid->addWidget(label4, 3, 1);
	grid->addWidget(MrColorButton, 3, 2);
	grid->addWidget(label5, 4, 1);
	grid->addWidget(TxtColorButton, 4, 2);
	grid->addWidget(label_sigcolor, 5, 1);
	grid->addWidget(SigColorButton, 5, 2);
	grid->addLayout(label7_layout, 6, 1);
	grid->addWidget(BaseColorButton, 6, 2);
	grid->addWidget(label8, 7, 1);
	grid->addWidget(Crh1ColorButton, 7, 2);
	grid->addWidget(label9, 8, 1);
	grid->addWidget(Crh2ColorButton, 8, 2);
	grid->addWidget(label10, 9, 1);
	grid->addWidget(FrColorButton, 9, 2);
	grid->addLayout(label_amarker_layout, 10, 1);
	grid->addWidget(AnnotMkrButton, 10, 2);
	grid->addWidget(label11, 11, 1);
	grid->addWidget(checkbox1, 11, 2);
	grid->addWidget(label13, 12, 1);
	grid->addWidget(checkbox4, 12, 2);

	colorSchema_Dark_Button = new QPushButton("Color scheme \"Dark\"");

	colorSchema_NK_Button = new QPushButton("Color scheme \"NK\"");

	DefaultButton = new QPushButton("Default color scheme");

	saveColorSchemaButton = new QPushButton("Save color scheme");

	loadColorSchemaButton = new QPushButton("Load color scheme");

	vbox->addWidget(colorSchema_Dark_Button);
	vbox->addWidget(colorSchema_NK_Button);
	vbox->addWidget(saveColorSchemaButton);
	vbox->addWidget(loadColorSchemaButton);

	QObject::connect(BgColorButton,			SIGNAL(clicked(SpecialButton *)), this, SLOT(BgColorButtonClicked(SpecialButton *)));
	QObject::connect(SrColorButton,			SIGNAL(clicked(SpecialButton *)), this, SLOT(SrColorButtonClicked(SpecialButton *)));
	QObject::connect(BrColorButton,			SIGNAL(clicked(SpecialButton *)), this, SLOT(BrColorButtonClicked(SpecialButton *)));
	QObject::connect(MrColorButton,			SIGNAL(clicked(SpecialButton *)), this, SLOT(MrColorButtonClicked(SpecialButton *)));
	QObject::connect(TxtColorButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(TxtColorButtonClicked(SpecialButton *)));
	QObject::connect(SigColorButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(SigColorButtonClicked(SpecialButton *)));
	QObject::connect(BaseColorButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(BaseColorButtonClicked(SpecialButton *)));
	QObject::connect(Crh1ColorButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh1ColorButtonClicked(SpecialButton *)));
	QObject::connect(Crh2ColorButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh2ColorButtonClicked(SpecialButton *)));
	QObject::connect(FrColorButton,			SIGNAL(clicked(SpecialButton *)), this, SLOT(FrColorButtonClicked(SpecialButton *)));
	QObject::connect(AnnotMkrButton,		SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotMkrButtonClicked(SpecialButton *)));
	QObject::connect(checkbox1,			SIGNAL(stateChanged(int)),		this, SLOT(bw_printing(int)));
	QObject::connect(checkbox2,			SIGNAL(stateChanged(int)),		this, SLOT(show_annot_markers(int)));
	QObject::connect(chbox_show_baselines,		SIGNAL(stateChanged(int)),		this, SLOT(show_baselines(int)));
	QObject::connect(checkbox4,			SIGNAL(stateChanged(int)),		this, SLOT(checkbox4Clicked(int)));
	QObject::connect(saveColorSchemaButton,		SIGNAL(clicked()),			this, SLOT(saveColorSchemaButtonClicked()));
	QObject::connect(loadColorSchemaButton,		SIGNAL(clicked()),			this, SLOT(loadColorSchemaButtonClicked()));
	QObject::connect(DefaultButton,			SIGNAL(clicked()),			this, SLOT(DefaultButtonClicked()));
	QObject::connect(colorSchema_NK_Button,		SIGNAL(clicked()),			this, SLOT(loadColorSchema_NK()));
	QObject::connect(colorSchema_Dark_Button,	SIGNAL(clicked()),			this, SLOT(loadColorSchema_Dark()));

}


void ColorOptions::spinBox4_3ValueChanged(int filesize)
{
	mainwindow->maxfilesize_to_readin_annotations = (long long)filesize * 1048576LL;
}


void ColorOptions::spinBox4_2ValueChanged(int stepsize)
{
	mainwindow->mousewheelsens = stepsize;
}


void ColorOptions::combobox4_1IndexChanged(int index)
{
	if(index == 0) mainwindow->powerlinefreq = 50;
	if(index == 1) mainwindow->powerlinefreq = 60;
}


void ColorOptions::combobox4_2IndexChanged(int index)
{
	mainwindow->viewtime_indicator_type = index;
	mainwindow->setup_viewbuf();
}


void ColorOptions::combobox4_3IndexChanged(int index)
{
	mainwindow->mainwindow_title_type = index;
	mainwindow->setMainwindowTitle(mainwindow->edfheaderlist[mainwindow->sel_viewtime]);
}



void ColorOptions::spinBox4_1ValueChanged(int interval)
{
	mainwindow->live_stream_update_interval = interval;
}



void ColorOptions::calibrate_checkbox_stateChanged(int state)
{
	if(state == Qt::Checked)
	{
		spinbox2_1->setEnabled(true);
		spinbox2_2->setEnabled(true);
		ApplyButton->setEnabled(true);
		mainwindow->auto_dpi = 0;
	}
	else
	{
		spinbox2_1->setEnabled(false);
		spinbox2_2->setEnabled(false);
		ApplyButton->setEnabled(false);
		mainwindow->auto_dpi = 1;
		mainwindow->pixelsizefactor = 1.0 / ((double)mainwindow->dpiy / 2.54);
		mainwindow->x_pixelsizefactor = 1.0 / ((double)mainwindow->dpix / 2.54);

		mainwindow->maincurve->drawCurve_stage_1();
	}
}



void ColorOptions::ApplyButtonClicked()
{
	int i, j;


	mainwindow->pixelsizefactor = 1.0 / (4450.0 / spinbox2_1->value());

	mainwindow->x_pixelsizefactor = 1.0 / (3550.0 / spinbox2_2->value());

	for(i=0; i<mainwindow->signalcomps; i++)
	{
		for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
		{
			mainwindow->signalcomp[i]->sensitivity[j] =
			 mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[j]].bitvalue
			 / ((double)mainwindow->signalcomp[i]->voltpercm
			 * mainwindow->pixelsizefactor);
		}
	}

	mainwindow->maincurve->drawCurve_stage_1();
}



void ColorOptions::bw_printing(int state)
{
	if(state==Qt::Checked)
	{
		mainwindow->maincurve->blackwhite_printing = 1;
	}

	if(state==Qt::Unchecked)
	{
		mainwindow->maincurve->blackwhite_printing = 0;
	}
}



void ColorOptions::show_annot_markers(int state)
{
	if(state==Qt::Checked)
	{
		mainwindow->show_annot_markers = 1;
	}

	if(state==Qt::Unchecked)
	{
		mainwindow->show_annot_markers = 0;
	}

	mainwindow->maincurve->update();
}



void ColorOptions::show_baselines(int state)
{
	if(state==Qt::Checked)
	{
		mainwindow->show_baselines = 1;
	}

	if(state==Qt::Unchecked)
	{
		mainwindow->show_baselines = 0;
	}

	mainwindow->maincurve->update();
}



void ColorOptions::checkbox4Clicked(int state)
{
	if(state==Qt::Checked)
	{
		mainwindow->clip_to_pane = 1;
	}

	if(state==Qt::Unchecked)
	{
		mainwindow->clip_to_pane = 0;
	}

	mainwindow->maincurve->update();
}



void ColorOptions::checkbox4_1Clicked(int state)
{
	if(state==Qt::Checked)
	{
		mainwindow->auto_reload_mtg = 1;
	}

	if(state==Qt::Unchecked)
	{
		mainwindow->auto_reload_mtg = 0;
	}
}



void ColorOptions::DefaultButtonClicked()
{
	int i;

	QPalette palette;

	mainwindow->maincurve->backgroundcolor = Qt::gray;
	BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

	mainwindow->maincurve->small_ruler_color = Qt::black;
	SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

	mainwindow->maincurve->big_ruler_color = Qt::darkGray;
	BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

	mainwindow->maincurve->mouse_rect_color = Qt::black;
	MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

	mainwindow->maincurve->text_color = Qt::black;
	TxtColorButton->setColor(mainwindow->maincurve->text_color);

	mainwindow->maincurve->signal_color = Qt::blue;
	SigColorButton->setColor(Qt::blue);

	mainwindow->maincurve->baseline_color = Qt::darkGray;
	BaseColorButton->setColor(Qt::darkGray);
	mainwindow->show_baselines = 1;
	chbox_show_baselines->setCheckState(Qt::Checked);

	mainwindow->maincurve->crosshair_1.color = Qt::red;
	Crh1ColorButton->setColor(Qt::red);

	mainwindow->maincurve->crosshair_2.color = Qt::cyan;
	Crh2ColorButton->setColor(Qt::cyan);

	mainwindow->maincurve->floating_ruler_color = Qt::red;
	FrColorButton->setColor(Qt::red);

	mainwindow->maincurve->annot_marker_color = Qt::white;
	AnnotMkrButton->setColor(Qt::white);
	mainwindow->show_annot_markers = 1;
	checkbox2->setCheckState(Qt::Checked);

	palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
	palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

	for(i=0; i<mainwindow->files_open; i++)
	{
		if(mainwindow->annotations_dock[i])
		{
			mainwindow->annotations_dock[i]->setPalette(palette);
		}
	}

	for(i=0; i<mainwindow->signalcomps; i++)
	{
		mainwindow->signalcomp[i]->color = mainwindow->maincurve->signal_color;
	}

	mainwindow->maincurve->blackwhite_printing = 1;

	checkbox1->setCheckState(Qt::Checked);

	checkbox4->setCheckState(Qt::Unchecked);

	mainwindow->maincurve->update();
}



void ColorOptions::BgColorButtonClicked(SpecialButton *)
{
	int i;

	QColor temp;

	QPalette palette;

	temp = QColorDialog::getColor(mainwindow->maincurve->backgroundcolor, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->backgroundcolor = temp;

		BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

		palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
		palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

		for(i=0; i<mainwindow->files_open; i++)
		{
			if(mainwindow->annotations_dock[i])
			{
				mainwindow->annotations_dock[i]->setPalette(palette);
			}
		}

		mainwindow->maincurve->update();
	}
}



void ColorOptions::SrColorButtonClicked(SpecialButton *)
{
	QColor temp;

	temp = QColorDialog::getColor(mainwindow->maincurve->small_ruler_color, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->small_ruler_color = temp;

		SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

		mainwindow->maincurve->update();
	}
}



void ColorOptions::BrColorButtonClicked(SpecialButton *)
{
	QColor temp;

	temp = QColorDialog::getColor(mainwindow->maincurve->big_ruler_color, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->big_ruler_color = temp;

		BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

		mainwindow->maincurve->update();
	}
}



void ColorOptions::MrColorButtonClicked(SpecialButton *)
{
	QColor temp;

	temp = QColorDialog::getColor(mainwindow->maincurve->mouse_rect_color, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->mouse_rect_color = temp;

		MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

		mainwindow->maincurve->update();
	}
}



void ColorOptions::TxtColorButtonClicked(SpecialButton *)
{
	int i;

	QColor temp;

	QPalette palette;

	temp = QColorDialog::getColor(mainwindow->maincurve->text_color, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->text_color = temp;

		TxtColorButton->setColor(mainwindow->maincurve->text_color);

		palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
		palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

		for(i=0; i<mainwindow->files_open; i++)
		{
			if(mainwindow->annotationlist[i])
			{
				mainwindow->annotations_dock[i]->setPalette(palette);
			}
		}

		mainwindow->maincurve->update();
	}
}



void ColorOptions::SigColorButtonClicked(SpecialButton *)
{
	int i, color;

	UI_ColorMenuDialog colormenudialog(&color);

	if(color < 0)	return;

	SigColorButton->setColor((Qt::GlobalColor)color);

	mainwindow->maincurve->signal_color = color;

	for(i=0; i<mainwindow->signalcomps; i++)
	{
		mainwindow->signalcomp[i]->color = color;
	}

	mainwindow->maincurve->update();
}



void ColorOptions::BaseColorButtonClicked(SpecialButton *)
{
	QColor temp;

	temp = QColorDialog::getColor(mainwindow->maincurve->baseline_color, this);

	if(temp.isValid())
	{
		mainwindow->maincurve->baseline_color = temp;

		BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

		mainwindow->maincurve->update();
	}
}



void ColorOptions::Crh1ColorButtonClicked(SpecialButton *)
{
	int color;

	UI_ColorMenuDialog colormenudialog(&color);

	if(color < 0)	return;

	Crh1ColorButton->setColor((Qt::GlobalColor)color);

	mainwindow->maincurve->crosshair_1.color = color;

	mainwindow->maincurve->update();
}



void ColorOptions::Crh2ColorButtonClicked(SpecialButton *)
{
	int color;

	UI_ColorMenuDialog colormenudialog(&color);

	if(color < 0)	return;

	Crh2ColorButton->setColor((Qt::GlobalColor)color);

	mainwindow->maincurve->crosshair_2.color = color;

	mainwindow->maincurve->update();
}


void ColorOptions::FrColorButtonClicked(SpecialButton *)
{
	int color;

	UI_ColorMenuDialog colormenudialog(&color);

	if(color < 0)	return;

	FrColorButton->setColor((Qt::GlobalColor)color);

	mainwindow->maincurve->floating_ruler_color = color;

	mainwindow->maincurve->update();
}


void ColorOptions::AnnotMkrButtonClicked(SpecialButton *)
{
	QColor temp;

	temp = QColorDialog::getColor(mainwindow->maincurve->annot_marker_color, this);

	if( temp.isValid() )
	{
		mainwindow->maincurve->annot_marker_color = temp;

		AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

		mainwindow->maincurve->update();
	}
}


void ColorOptions::saveColorSchemaButtonClicked()
{
	char path[MAX_PATH_LENGTH];

	FILE *colorfile;

	strcpy(path, mainwindow->recent_colordir);
	strcat(path, "/my_colorscheme.color");

	strcpy(path, QFileDialog::getSaveFileName(0, "Save color scheme", QString::fromLocal8Bit(path), "Color scheme files (*.color *.COLOR)").toLocal8Bit().data());

	if(!strcmp(path, ""))
	{
		return;
	}

	if(strlen(path) > 4)
	{
		if(strcmp(path + strlen(path) - 6, ".color"))	// check if the ending is '.color'
		{
			strcat(path, ".color");				// if not : append it to the string.
		}
	}

	get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

	colorfile = fopen(path, "wb");
	if(colorfile == NULL)
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for writing.");
		messagewindow.exec();

		return;
	}

	fprintf(colorfile, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_colorschema>\n");

	fprintf(colorfile, " <backgroundcolor>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </backgroundcolor>\n",
									mainwindow->maincurve->backgroundcolor.red(),
									mainwindow->maincurve->backgroundcolor.green(),
									mainwindow->maincurve->backgroundcolor.blue());

	fprintf(colorfile, " <small_ruler_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </small_ruler_color>\n",
									mainwindow->maincurve->small_ruler_color.red(),
									mainwindow->maincurve->small_ruler_color.green(),
									mainwindow->maincurve->small_ruler_color.blue());

	fprintf(colorfile, " <big_ruler_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </big_ruler_color>\n",
									mainwindow->maincurve->big_ruler_color.red(),
									mainwindow->maincurve->big_ruler_color.green(),
									mainwindow->maincurve->big_ruler_color.blue());

	fprintf(colorfile, " <mouse_rect_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </mouse_rect_color>\n",
									mainwindow->maincurve->mouse_rect_color.red(),
									mainwindow->maincurve->mouse_rect_color.green(),
									mainwindow->maincurve->mouse_rect_color.blue());

	fprintf(colorfile, " <text_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </text_color>\n",
									mainwindow->maincurve->text_color.red(),
									mainwindow->maincurve->text_color.green(),
									mainwindow->maincurve->text_color.blue());

	fprintf(colorfile, " <baseline_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </baseline_color>\n",
									mainwindow->maincurve->baseline_color.red(),
									mainwindow->maincurve->baseline_color.green(),
									mainwindow->maincurve->baseline_color.blue());

	fprintf(colorfile, " <annot_marker_color>\n"
									"	<red>%i</red>\n"
									"	<green>%i</green>\n"
									"	<blue>%i</blue>\n"
									" </annot_marker_color>\n",
									mainwindow->maincurve->annot_marker_color.red(),
									mainwindow->maincurve->annot_marker_color.green(),
									mainwindow->maincurve->annot_marker_color.blue());

	fprintf(colorfile, " <signal_color>%i</signal_color>\n",
									mainwindow->maincurve->signal_color);

	fprintf(colorfile, " <crosshair_1_color>%i</crosshair_1_color>\n",
									mainwindow->maincurve->crosshair_1.color);

	fprintf(colorfile, " <crosshair_2_color>%i</crosshair_2_color>\n",
									mainwindow->maincurve->crosshair_2.color);

	fprintf(colorfile, " <floating_ruler_color>%i</floating_ruler_color>\n",
									mainwindow->maincurve->floating_ruler_color);

	fprintf(colorfile, " <blackwhite_printing>%i</blackwhite_printing>\n",
									mainwindow->maincurve->blackwhite_printing);

	fprintf(colorfile, " <show_annot_markers>%i</show_annot_markers>\n",
									mainwindow->show_annot_markers);

	fprintf(colorfile, " <show_baselines>%i</show_baselines>\n",
									mainwindow->show_baselines);

	fprintf(colorfile, " <clip_to_pane>%i</clip_to_pane>\n",
									mainwindow->clip_to_pane);




	fprintf(colorfile, "</" PROGRAM_NAME "_colorschema>\n");

	fclose(colorfile);
}


void ColorOptions::loadColorSchemaButtonClicked()
{
	char path[MAX_PATH_LENGTH],
			 scratchpad[2048],
			 *result;

	struct xml_handle *xml_hdl;


	strcpy(path, QFileDialog::getOpenFileName(0, "Load colorschema", QString::fromLocal8Bit(mainwindow->recent_colordir), "Montage files (*.color *.COLOR)").toLocal8Bit().data());

	if(!strcmp(path, ""))
	{
		return;
	}

	get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

	xml_hdl = xml_get_handle(path);
	if(xml_hdl==NULL)
	{
		sprintf(scratchpad, "Can not open colorschema:\n%s", path);
		QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad));
		messagewindow.exec();
		return;
	}

	if(strcmp(xml_hdl->elementname, PROGRAM_NAME "_colorschema"))
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this colorschema.");
		messagewindow.exec();
		xml_close(xml_hdl);
		return;
	}

	mainwindow->get_rgbcolor_settings(xml_hdl, "backgroundcolor", 0, &mainwindow->maincurve->backgroundcolor);

	mainwindow->get_rgbcolor_settings(xml_hdl, "small_ruler_color", 0, &mainwindow->maincurve->small_ruler_color);

	mainwindow->get_rgbcolor_settings(xml_hdl, "big_ruler_color", 0, &mainwindow->maincurve->big_ruler_color);

	mainwindow->get_rgbcolor_settings(xml_hdl, "mouse_rect_color", 0, &mainwindow->maincurve->mouse_rect_color);

	mainwindow->get_rgbcolor_settings(xml_hdl, "text_color", 0, &mainwindow->maincurve->text_color);

	mainwindow->get_rgbcolor_settings(xml_hdl, "baseline_color", 0, &mainwindow->maincurve->baseline_color);

	mainwindow->get_rgbcolor_settings(xml_hdl, "annot_marker_color", 0, &mainwindow->maincurve->annot_marker_color);

	if(xml_goto_nth_element_inside(xml_hdl, "signal_color", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->maincurve->signal_color = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "floating_ruler_color", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->maincurve->floating_ruler_color = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "blackwhite_printing", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->maincurve->blackwhite_printing = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "show_annot_markers", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->show_annot_markers = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "show_baselines", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->show_baselines = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "clip_to_pane", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->clip_to_pane = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "crosshair_1_color", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->maincurve->crosshair_1.color = atoi(result);
	free(result);

	xml_go_up(xml_hdl);

	if(xml_goto_nth_element_inside(xml_hdl, "crosshair_2_color", 0))
	{
		xml_close(xml_hdl);
		return;
	}
	result = xml_get_content_of_element(xml_hdl);
	if(result==NULL)
	{
		xml_close(xml_hdl);
		return;
	}
	mainwindow->maincurve->crosshair_2.color = atoi(result);
	free(result);

	xml_close(xml_hdl);

	update_interface();
}


void ColorOptions::update_interface(void)
{
	int i;

	QPalette palette;


	BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

	SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

	BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

	MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

	TxtColorButton->setColor(mainwindow->maincurve->text_color);

	SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);

	if(mainwindow->show_baselines)
	{
		chbox_show_baselines->setCheckState(Qt::Checked);
	}
	else
	{
		chbox_show_baselines->setCheckState(Qt::Unchecked);
	}

	BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

	Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);

	Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);

	FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);

	if(mainwindow->show_annot_markers)
	{
		checkbox2->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox2->setCheckState(Qt::Unchecked);
	}

	AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

	if(mainwindow->maincurve->blackwhite_printing)
	{
		checkbox1->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox1->setCheckState(Qt::Unchecked);
	}

	if(mainwindow->clip_to_pane)
	{
		checkbox4->setCheckState(Qt::Checked);
	}
	else
	{
		checkbox4->setCheckState(Qt::Unchecked);
	}

	palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
	palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

	for(i=0; i<mainwindow->files_open; i++)
	{
		if(mainwindow->annotations_dock[i])
		{
			mainwindow->annotations_dock[i]->setPalette(palette);
		}
	}

	for(i=0; i<mainwindow->signalcomps; i++)
	{
		mainwindow->signalcomp[i]->color = mainwindow->maincurve->signal_color;
	}

	mainwindow->maincurve->update();
}


void ColorOptions::loadColorSchema_NK()
{
	mainwindow->maincurve->backgroundcolor.setRed(255);
	mainwindow->maincurve->backgroundcolor.setGreen(255);
	mainwindow->maincurve->backgroundcolor.setBlue(255);

	mainwindow->maincurve->small_ruler_color.setRed(0);
	mainwindow->maincurve->small_ruler_color.setGreen(0);
	mainwindow->maincurve->small_ruler_color.setBlue(0);

	mainwindow->maincurve->big_ruler_color.setRed(255);
	mainwindow->maincurve->big_ruler_color.setGreen(255);
	mainwindow->maincurve->big_ruler_color.setBlue(0);

	mainwindow->maincurve->mouse_rect_color.setRed(0);
	mainwindow->maincurve->mouse_rect_color.setGreen(0);
	mainwindow->maincurve->mouse_rect_color.setBlue(0);

	mainwindow->maincurve->text_color.setRed(0);
	mainwindow->maincurve->text_color.setGreen(0);
	mainwindow->maincurve->text_color.setBlue(0);

	mainwindow->maincurve->baseline_color.setRed(128);
	mainwindow->maincurve->baseline_color.setGreen(128);
	mainwindow->maincurve->baseline_color.setBlue(128);

	mainwindow->maincurve->annot_marker_color.setRed(0);
	mainwindow->maincurve->annot_marker_color.setGreen(0);
	mainwindow->maincurve->annot_marker_color.setBlue(0);

	mainwindow->maincurve->signal_color = 2;

	mainwindow->maincurve->floating_ruler_color = 7;

	mainwindow->maincurve->blackwhite_printing = 1;

	mainwindow->show_annot_markers = 1;

	mainwindow->show_baselines = 1;

	mainwindow->maincurve->crosshair_1.color = 7;

	mainwindow->maincurve->crosshair_2.color = 9;

	mainwindow->clip_to_pane = 0;

	update_interface();
}


void ColorOptions::loadColorSchema_Dark()
{
	mainwindow->maincurve->backgroundcolor.setRed(64);
	mainwindow->maincurve->backgroundcolor.setGreen(64);
	mainwindow->maincurve->backgroundcolor.setBlue(64);

	mainwindow->maincurve->small_ruler_color.setRed(255);
	mainwindow->maincurve->small_ruler_color.setGreen(255);
	mainwindow->maincurve->small_ruler_color.setBlue(255);

	mainwindow->maincurve->big_ruler_color.setRed(128);
	mainwindow->maincurve->big_ruler_color.setGreen(128);
	mainwindow->maincurve->big_ruler_color.setBlue(128);

	mainwindow->maincurve->mouse_rect_color.setRed(255);
	mainwindow->maincurve->mouse_rect_color.setGreen(255);
	mainwindow->maincurve->mouse_rect_color.setBlue(255);

	mainwindow->maincurve->text_color.setRed(255);
	mainwindow->maincurve->text_color.setGreen(255);
	mainwindow->maincurve->text_color.setBlue(255);

	mainwindow->maincurve->baseline_color.setRed(128);
	mainwindow->maincurve->baseline_color.setGreen(128);
	mainwindow->maincurve->baseline_color.setBlue(128);

	mainwindow->maincurve->annot_marker_color.setRed(255);
	mainwindow->maincurve->annot_marker_color.setGreen(255);
	mainwindow->maincurve->annot_marker_color.setBlue(255);

	mainwindow->maincurve->signal_color = 12;

	mainwindow->maincurve->floating_ruler_color = 10;

	mainwindow->maincurve->blackwhite_printing = 1;

	mainwindow->show_annot_markers = 1;

	mainwindow->show_baselines = 1;

	mainwindow->maincurve->crosshair_1.color = 7;

	mainwindow->maincurve->crosshair_2.color = 10;

	mainwindow->clip_to_pane = 0;

	update_interface();
}











