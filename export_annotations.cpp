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



#include "export_annotations.h"


#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__)

#define fopeno fopen

#else

#define fseeko fseeko64
#define ftello ftello64
#define fopeno fopen64

#endif



UI_ExportAnnotationswindow::UI_ExportAnnotationswindow(QWidget *w_parent, struct annotationblock **annotationlist)
{

  mainwindow = (UI_Mainwindow *)w_parent;

  this->annotationlist = annotationlist;


  ExportAnnotsDialog = new QDialog;

  ExportAnnotsDialog->setMinimumSize(QSize(800, 570));
  ExportAnnotsDialog->setMaximumSize(QSize(800, 570));
  ExportAnnotsDialog->setWindowTitle("Export annotations");
  ExportAnnotsDialog->setModal(true);
  ExportAnnotsDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(ExportAnnotsDialog);
  filelist->setGeometry(QRect(10, 10, 780, 75));
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);

  for(int i=0; i<mainwindow->files_open; i++)
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);

  formatGroupBox = new QGroupBox("output format", ExportAnnotsDialog);
  formatGroupBox->setGeometry(10, 120, 200, 135);

  CSVRadioButton = new QRadioButton("ASCII/CSV");

  EDFplusRadioButton = new QRadioButton("EDFplus");

  XMLRadioButton = new QRadioButton("XML");

  formatVBoxLayout = new QVBoxLayout;
  formatVBoxLayout->addWidget(CSVRadioButton);
  formatVBoxLayout->addWidget(EDFplusRadioButton);
  formatVBoxLayout->addWidget(XMLRadioButton);
  formatGroupBox->setLayout(formatVBoxLayout);

  fileGroupBox = new QGroupBox("export annotations", ExportAnnotsDialog);
  fileGroupBox->setGeometry(320, 120, 200, 90);

  selectRadioButton = new QRadioButton("from selected file");
  selectRadioButton->setChecked(true);

  mergeRadioButton = new QRadioButton("from all files (merge)");

  fileVBoxLayout = new QVBoxLayout;
  fileVBoxLayout->addWidget(selectRadioButton);
  fileVBoxLayout->addWidget(mergeRadioButton);
  fileGroupBox->setLayout(fileVBoxLayout);

  asciiSettingsGroupBox = new QGroupBox("ASCII settings", ExportAnnotsDialog);
  asciiSettingsGroupBox->setGeometry(10, 270, 300, 290);

  separatorLabel = new QLabel(ExportAnnotsDialog);
  separatorLabel->setText("separator:");

  separatorBox = new QComboBox(ExportAnnotsDialog);
  separatorBox->addItem("comma");
  separatorBox->addItem("tab");

  asciiSecondsRadioButton = new QRadioButton("seconds relative to start of file");
  asciiSecondsRadioButton->setChecked(true);

  asciiISOtimeRadioButton = new QRadioButton("hh:mm:ss");

  asciiISOtimedateRadioButton = new QRadioButton("yyyy-mm-ddThh:mm:ss");

  asciiISOtimeFractionRadioButton = new QRadioButton("hh:mm:ss.xxx");

  asciiISOtimedateFractionRadioButton = new QRadioButton("yyyy-mm-ddThh:mm:ss.xxx");

  asciiTimedateRadioButton = new QRadioButton("mm/dd/yyyy hh:mm:ss");			// Non-ISO format of Daniels program.

  asciiTimedateFractionRadioButton = new QRadioButton("mm/dd/yyyy hh:mm:ss.xxx");		// Non-ISO format of Daniels program.

  durationCheckBox = new QCheckBox("Duration column", ExportAnnotsDialog);
  durationCheckBox->setTristate(false);
  durationCheckBox->setCheckState(Qt::Unchecked);

  //endCheckBox = new QCheckBox("End column", ExportAnnotsDialog);
  //endCheckBox->setTristate(false);
  //endCheckBox->setCheckState(Qt::Unchecked);

	durationGroup = new QButtonGroup(ExportAnnotsDialog);
	durationGroup->addButton(durationCheckBox);
	//durationGroup->addButton(endCheckBox);
	durationGroup->setExclusive(false);

  asciiSettingsHBoxLayout = new QHBoxLayout;
  asciiSettingsHBoxLayout->addWidget(separatorLabel);
  asciiSettingsHBoxLayout->addWidget(separatorBox);

  asciiSettingsVBoxLayout = new QVBoxLayout;
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout);
  asciiSettingsVBoxLayout->addWidget(asciiSecondsRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiTimedateRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiTimedateFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(durationCheckBox);
  //asciiSettingsVBoxLayout->addWidget(endCheckBox);
  asciiSettingsGroupBox->setLayout(asciiSettingsVBoxLayout);

  ExportButton = new QPushButton(ExportAnnotsDialog);
  ExportButton->setGeometry(QRect(500, 530, 100, 25));
  ExportButton->setText("Export");

  CloseButton = new QPushButton(ExportAnnotsDialog);
  CloseButton->setGeometry(QRect(690, 530, 100, 25));
  CloseButton->setText("Close");

  separatorBox->setCurrentIndex(mainwindow->export_annotations_var->separator);

  switch(mainwindow->export_annotations_var->format)
  {
    case 0 : EDFplusRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 8 : XMLRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 1 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiSecondsRadioButton->setChecked(true);
             break;

    case 2 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeRadioButton->setChecked(true);
             break;

    case 3 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateRadioButton->setChecked(true);
             break;

    case 4 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeFractionRadioButton->setChecked(true);
             break;

    case 5 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateFractionRadioButton->setChecked(true);
             break;
  }

  if(mainwindow->export_annotations_var->duration)	durationCheckBox->setChecked(true);
  else							durationCheckBox->setChecked(false);

  QObject::connect(CloseButton,    SIGNAL(clicked()),     ExportAnnotsDialog, SLOT(close()));
  QObject::connect(ExportButton,   SIGNAL(clicked()),     this,               SLOT(ExportButtonClicked()));
  QObject::connect(CSVRadioButton, SIGNAL(toggled(bool)), this,               SLOT(outputformatRadioButtonClicked(bool)));

	QObject::connect(durationCheckBox, SIGNAL(stateChanged(int)), this, SLOT(DurationCheckBoxChanged(int)));
	//QObject::connect(endCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EndCheckBoxChanged(int)));

  filelist->setCurrentRow(mainwindow->files_open - 1);
}


int UI_ExportAnnotationswindow::execute()
{
	check_list();
	return ExportAnnotsDialog->exec();
}



void UI_ExportAnnotationswindow::check_list()
{
	if(annotationlist[0] == NULL)
	{
		QMessageBox messagewindow(QMessageBox::Critical, "Error", "Attempting to export an empty list.");
		messagewindow.exec();
	}
}



void UI_ExportAnnotationswindow::outputformatRadioButtonClicked(bool checked)
{
	asciiSettingsGroupBox->setEnabled( checked );
}



void UI_ExportAnnotationswindow::ExportButtonClicked()
{
  int i, j, n,
      len,
      csv_format=0,
      hdl,
      annot_cnt,
      remainder,
      remainder_end,
      include_duration,
      include_end;

  long long end;

  double onset_from_start,
	 end_from_start;

  char path[MAX_PATH_LENGTH],
       str[1024],
       separator;

  FILE *annotationfile=NULL;

  struct annotationblock *annot, *annot_list;

  struct edfhdrblock *hdr;

  struct date_time_struct tm, tm_end;


  include_duration = 0;
  include_end = 0;


	if(CSVRadioButton->isChecked() == true)
	{
	       	if(asciiSecondsRadioButton->isChecked() == true)		csv_format = 1;
	       	if(asciiISOtimeRadioButton->isChecked() == true)		csv_format = 2;
	       	if(asciiISOtimedateRadioButton->isChecked() == true)		csv_format = 3;
	       	if(asciiISOtimeFractionRadioButton->isChecked() == true)	csv_format = 4;
	       	if(asciiISOtimedateFractionRadioButton->isChecked() == true)	csv_format = 5;
	       	if(asciiTimedateRadioButton->isChecked() == true)		csv_format = 6;
	       	if(asciiTimedateFractionRadioButton->isChecked() == true)	csv_format = 7;
	}

  if(EDFplusRadioButton->isChecked() == true)	csv_format = 0;
  if(XMLRadioButton->isChecked() == true)	csv_format = 8;

   mainwindow->export_annotations_var->format = csv_format;

  if(separatorBox->currentIndex() == 0)
  {
    separator = ',';
    mainwindow->export_annotations_var->separator = 0;
  }
  else
  {
    separator = '\t';
    mainwindow->export_annotations_var->separator = 1;
  }

  if(durationCheckBox->checkState() == Qt::Checked)	include_duration = 1;
  //else if(endCheckBox->checkState() == Qt::Checked)	include_end = 1;

  mainwindow->export_annotations_var->duration = include_duration;
  mainwindow->export_annotations_var->end = include_end;

  if(!mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  if(filelist->count() < 1)
  {
    ExportAnnotsDialog->close();
    return;
  }

  ExportButton->setEnabled(false);
  CloseButton->setEnabled(false);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  if(i==mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  n = i;

  if(mergeRadioButton->isChecked() == true)
  {
    n = mainwindow->sel_viewtime;
  }

  hdr = mainwindow->edfheaderlist[n];

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[n]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  if((csv_format > 0) && (csv_format < 8))
  {
    strcat(path, "_annotations.txt");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT *.csv *.CSV)").toLocal8Bit().data());
  }

  if(csv_format == 8)
  {
    strcat(path, "_annotations.xml");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "XML files (*.xml *.XML)").toLocal8Bit().data());
  }

  if(csv_format == 0)
  {
    strcat(path, "_annotations.edf");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());
  }

  if(!strcmp(path, ""))
  {
    ExportAnnotsDialog->close();
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Export annotations", "Error, selected file is in use.");
    messagewindow.exec();
    return;
  }

  annot_list = edfplus_annotation_copy_list(annotationlist);


  annot_cnt = edfplus_annotation_count(&annot_list);

///////////////////////////// CSV (text) export //////////////////////////////////////

  if((csv_format > 0) && (csv_format < 8))
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open annotationfile for writing.");
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_delete_list(&annot_list);
      return;
    }

    if(include_duration)	fprintf(annotationfile, "#Onset%cDuration%cAnnotation", separator, separator);
    else if(include_end)	fprintf(annotationfile, "#Onset%cEnd%cAnnotation", separator, separator);
    else			fprintf(annotationfile, "#Onset%cAnnotation", separator);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
    fprintf(annotationfile, "\r\n");
#endif

    for(j=0; j < annot_cnt; j++)		// for all annotations
    {
	annot = edfplus_annotation_item(&annot_list, j);
	if(annot == NULL) break;

	strncpy(str, annot->annotation, 1024);
	str[1023] = 0;
	utf8_to_latin1(str);

	len = strlen(str);
	for(i=0; i<len; i++)
	{
		if( (((unsigned char *)str)[i] < 32) or (((unsigned char *)str)[i] == ',') )
		{
			str[i] = '.';
		}
	}


	end = annot->onset + atof(annot->duration) * TIME_DIMENSION;
	onset_from_start = (double)(annot->onset - hdr->starttime_offset) / TIME_DIMENSION;
	end_from_start = (double)(end - hdr->starttime_offset) / TIME_DIMENSION;

	remainder = annot->onset % TIME_DIMENSION;
	if(remainder < 0)
	{
		utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);
		remainder = TIME_DIMENSION + remainder;
	}
	else
	{
		utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
	}

	remainder_end = end % TIME_DIMENSION;
	if(remainder_end < 0)
	{
		utc_to_date_time(hdr->utc_starttime + (end / TIME_DIMENSION) - 1, &tm_end);
		remainder_end = TIME_DIMENSION + remainder_end;
	}
	else
	{
		utc_to_date_time(hdr->utc_starttime + (end / TIME_DIMENSION), &tm_end);
	}

	if(csv_format == 1)	 fprintf(annotationfile, "%+.7f", onset_from_start);
	else if(csv_format == 2) fprintf(annotationfile, "%02i:%02i:%02i", tm.hour, tm.minute, tm.second);
	else if(csv_format == 3) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
	else if(csv_format == 4) fprintf(annotationfile, "%02i:%02i:%02i.%07i", tm.hour, tm.minute, tm.second, remainder);
	else if(csv_format == 5) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, remainder);
	else if(csv_format == 6) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i", tm.month, tm.day, tm.year, tm.hour, tm.minute, tm.second);
	else if(csv_format == 7) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i.%07i", tm.month, tm.day, tm.year, tm.hour, tm.minute, tm.second, remainder);


	fprintf(annotationfile, "%c", separator);


	if(include_duration) fprintf(annotationfile, "%s%c", annot->duration, separator);

	else if(include_end)
	{
		if(csv_format == 1)	 fprintf(annotationfile, "%+.7f", end_from_start);
		else if(csv_format == 2) fprintf(annotationfile, "%02i:%02i:%02i", tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 3) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i", tm_end.year, tm_end.month, tm_end.day, tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 4) fprintf(annotationfile, "%02i:%02i:%02i.%07i", tm_end.hour, tm_end.minute, tm_end.second, remainder);
		else if(csv_format == 5) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i", tm_end.year, tm_end.month, tm_end.day, tm_end.hour, tm_end.minute, tm_end.second, remainder);
		else if(csv_format == 6) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i", tm_end.month, tm_end.day, tm_end.year, tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 7) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i.%07i", tm_end.month, tm_end.day, tm_end.year, tm_end.hour, tm_end.minute, tm_end.second, remainder);
		fprintf(annotationfile, "%c", separator);
	}

	fprintf(annotationfile, "%s", str);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
	fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
	fprintf(annotationfile, "\r\n");
#endif

    }

    fclose(annotationfile);

    edfplus_annotation_delete_list(&annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// XML export //////////////////////////////////////

  if(csv_format == 8)
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open annotationfile for writing.");
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_delete_list(&annot_list);
      return;
    }

    fprintf(annotationfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    fprintf(annotationfile, "<!-- Generated by " PROGRAM_NAME " " PROGRAM_VERSION " -->\n");

    fprintf(annotationfile, "<annotationlist>\n");

    utc_to_date_time(hdr->utc_starttime, &tm);

    fprintf(annotationfile, " <recording_start_time>%04i-%02i-%02iT%02i:%02i:%02i.%07i</recording_start_time>",
                            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, (int)hdr->starttime_offset);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
	fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
	fprintf(annotationfile, "\r\n");
#endif

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_item(&annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      remainder = annot->onset % TIME_DIMENSION;

      if(remainder < 0)
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);

        remainder = TIME_DIMENSION + remainder;
      }
      else
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
      }

      fprintf(annotationfile, " <annotation>\n"
                              "  <onset>%04i-%02i-%02iT%02i:%02i:%02i.%07i</onset>\n"
                              "  <duration>%s</duration>\n"
                              "  <description>",
                              tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, remainder, annot->duration);

      xml_fwrite_encode_entity(annotationfile, annot->annotation);

      fprintf(annotationfile, "</description>\n" " </annotation>\n");
    }

    fprintf(annotationfile, "</annotationlist>\n");

    fclose(annotationfile);

    edfplus_annotation_delete_list(&annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// EDFplus export //////////////////////////////////////

  if(csv_format == 0)
  {
    hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, 0);
    if(hdl < 0)
    {
      switch(hdl)
      {
        case EDFLIB_MALLOC_ERROR : strcpy(str, "EDFlib: malloc error");
                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY : strcpy(str, "EDFlib: no such file or directory");
                                  break;
        case EDFLIB_MAXFILES_REACHED : strcpy(str, "EDFlib: maximum files reached");
                                  break;
        case EDFLIB_FILE_ALREADY_OPENED : strcpy(str, "EDFlib: file already opened");
                                  break;
        case EDFLIB_NUMBER_OF_SIGNALS_INVALID : strcpy(str, "EDFlib: number of signals is invalid");
                                  break;
        default : strcpy(str, "EDFlib: unknown error");
                                  break;
      }

      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      edfplus_annotation_delete_list(&annot_list);
      ExportAnnotsDialog->close();
      return;
    }

    utc_to_date_time(hdr->utc_starttime, &tm);
    edf_set_startdatetime(hdl, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    if((hdr->edfplus) || (hdr->bdfplus))
    {
      edf_set_patientname(hdl, hdr->plus_patient_name);
      edf_set_patientcode(hdl, hdr->plus_patientcode);
      if(!(strcmp(hdr->plus_gender, "Male")))
      {
        edf_set_gender(hdl, 1);
      }
      if(!(strcmp(hdr->plus_gender, "Female")))
      {
        edf_set_gender(hdl, 0);
      }
      if(hdr->plus_birthdate[0] != 0)
      {
        if(!(strncmp(hdr->plus_birthdate + 3, "jan", 3)))  n = 1;
        if(!(strncmp(hdr->plus_birthdate + 3, "feb", 3)))  n = 2;
        if(!(strncmp(hdr->plus_birthdate + 3, "mar", 3)))  n = 3;
        if(!(strncmp(hdr->plus_birthdate + 3, "apr", 3)))  n = 4;
        if(!(strncmp(hdr->plus_birthdate + 3, "may", 3)))  n = 5;
        if(!(strncmp(hdr->plus_birthdate + 3, "jun", 3)))  n = 6;
        if(!(strncmp(hdr->plus_birthdate + 3, "jul", 3)))  n = 7;
        if(!(strncmp(hdr->plus_birthdate + 3, "aug", 3)))  n = 8;
        if(!(strncmp(hdr->plus_birthdate + 3, "sep", 3)))  n = 9;
        if(!(strncmp(hdr->plus_birthdate + 3, "oct", 3)))  n = 10;
        if(!(strncmp(hdr->plus_birthdate + 3, "nov", 3)))  n = 11;
        if(!(strncmp(hdr->plus_birthdate + 3, "dec", 3)))  n = 12;
        edf_set_birthdate(hdl, atoi(hdr->plus_birthdate + 7), n, atoi(hdr->plus_birthdate));
      }
      edf_set_patient_additional(hdl, hdr->plus_patient_additional);
      edf_set_admincode(hdl, hdr->plus_admincode);
      edf_set_technician(hdl, hdr->plus_technician);
      edf_set_equipment(hdl, hdr->plus_equipment);
      edf_set_recording_additional(hdl, hdr->plus_recording_additional);
    }
    else
    {
      edf_set_patientname(hdl, hdr->patient);
      edf_set_recording_additional(hdl, hdr->recording);
    }

    int hasdot,
        decimals;

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_item(&annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      if(annot->duration[0] == 0)
      {
        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, -1LL, annot->annotation);
      }
      else
      {
        strcpy(str, annot->duration);

        len = strlen(str);

        hasdot = 0;

        for(i=0; i<len; i++)
        {
          if(str[i] == '.')
          {
            hasdot = 1;

            for(decimals=0; decimals<4; decimals++)
            {
              str[i] = str[i+1];

              if(str[i] == 0)
              {
                for( ; decimals<4; decimals++)
                {
                  str[i] = '0';

                  i++;
                }

                i--;
              }

              i++;
            }

            str[i] = 0;

            break;
          }
        }

        if(!hasdot)
        {
          strcat(str, "0000");
        }

        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, atoi(str), annot->annotation);
      }
    }

    if(edfclose_file(hdl) != 0)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred: edfclose_file()");
      messagewindow.exec();
    }

    edfplus_annotation_delete_list(&annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();
  }
}



void UI_ExportAnnotationswindow::DurationCheckBoxChanged(int state)
{
//	bool checked = state == Qt::Checked;
//	if(checked) endCheckBox->setCheckState(Qt::Unchecked);

}



//void UI_ExportAnnotationswindow::EndCheckBoxChanged(int state)
//{
//	bool checked = state == Qt::Checked;
//	if(checked) durationCheckBox->setCheckState(Qt::Unchecked);
//}



void UI_ExportAnnotationswindow::backup(const char* extension)
{
  int i, j, n,
      len,
      csv_format=0,
      hdl,
      annot_cnt,
      remainder,
      remainder_end,
      include_duration,
      include_end;

  long long end;

  double onset_from_start,
	 end_from_start;

  char path[MAX_PATH_LENGTH],
       str[1024],
       separator;

  FILE *annotationfile=NULL;

  struct annotationblock *annot, *annot_list;

  struct edfhdrblock *hdr;

  struct date_time_struct tm, tm_end;





// ### Construct filename ###

  if(!mainwindow->files_open)	{ ExportAnnotsDialog->close(); return; }
  if(filelist->count() < 1)	{ ExportAnnotsDialog->close(); return; }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  if(i==mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  n = i;

  if(mergeRadioButton->isChecked() == true)
  {
    n = mainwindow->sel_viewtime;
  }

  hdr = mainwindow->edfheaderlist[n];

  path[0] = 0;
  if(mainwindow->recent_savedir[0] != 0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[n]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
 
 

	csv_format = mainwindow->export_annotations_var->format;
	if(mainwindow->export_annotations_var->separator == 0)
		separator = ',';
	else
		separator = '\t';
	include_duration = mainwindow->export_annotations_var->duration;
	include_end = mainwindow->export_annotations_var->end;

  strcat(path, "_");
  strcat(path, extension);
  strcat(path, "_backup");
  if((csv_format > 0) and (csv_format < 8))
  {
    strcat(path, ".txt");
  }

  if(csv_format == 8)
  {
    strcat(path, ".xml");
  }

  if(csv_format == 0)
  {
    strcat(path, ".edf");
  }

  if(!strcmp(path, ""))
  {
    ExportAnnotsDialog->close();
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Export annotations", "Annotation backup: backup file is in use.");
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
    messagewindow.exec();
    return;
  }

  annot_list = edfplus_annotation_copy_list(annotationlist);


  annot_cnt = edfplus_annotation_count(&annot_list);

// ### CSV (text) export ###

  if((csv_format > 0) && (csv_format < 8))
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Annotation backup: Cannot open annotationfile for writing.");
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_delete_list(&annot_list);
      return;
    }

    if(include_duration)	fprintf(annotationfile, "#Onset%cDuration%cAnnotation", separator, separator);
    else if(include_end)	fprintf(annotationfile, "#Onset%cEnd%cAnnotation", separator, separator);
    else			fprintf(annotationfile, "#Onset%cAnnotation", separator);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
    fprintf(annotationfile, "\r\n");
#endif

    for(j=0; j < annot_cnt; j++)		// for all annotations
    {
	annot = edfplus_annotation_item(&annot_list, j);
	if(annot == NULL) break;

	strncpy(str, annot->annotation, 1024);
	str[1023] = 0;
	utf8_to_latin1(str);

	len = strlen(str);
	for(i=0; i<len; i++)
	{
		if( (((unsigned char *)str)[i] < 32) or (((unsigned char *)str)[i] == ',') )
		{
			str[i] = '.';
		}
	}


	end = annot->onset + atof(annot->duration) * TIME_DIMENSION;
	onset_from_start = (double)(annot->onset - hdr->starttime_offset) / TIME_DIMENSION;
	end_from_start = (double)(end - hdr->starttime_offset) / TIME_DIMENSION;

	remainder = annot->onset % TIME_DIMENSION;
	if(remainder < 0)
	{
		utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);
		remainder = TIME_DIMENSION + remainder;
	}
	else
	{
		utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
	}

	remainder_end = end % TIME_DIMENSION;
	if(remainder_end < 0)
	{
		utc_to_date_time(hdr->utc_starttime + (end / TIME_DIMENSION) - 1, &tm_end);
		remainder_end = TIME_DIMENSION + remainder_end;
	}
	else
	{
		utc_to_date_time(hdr->utc_starttime + (end / TIME_DIMENSION), &tm_end);
	}

	if(csv_format == 1)	 fprintf(annotationfile, "%+.7f", onset_from_start);
	else if(csv_format == 2) fprintf(annotationfile, "%02i:%02i:%02i", tm.hour, tm.minute, tm.second);
	else if(csv_format == 3) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
	else if(csv_format == 4) fprintf(annotationfile, "%02i:%02i:%02i.%07i", tm.hour, tm.minute, tm.second, remainder);
	else if(csv_format == 5) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, remainder);
	else if(csv_format == 6) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i", tm.month, tm.day, tm.year, tm.hour, tm.minute, tm.second);
	else if(csv_format == 7) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i.%07i", tm.month, tm.day, tm.year, tm.hour, tm.minute, tm.second, remainder);


	fprintf(annotationfile, "%c", separator);


	if(include_duration) fprintf(annotationfile, "%s%c", annot->duration, separator);

	else if(include_end)
	{
		if(csv_format == 1)	 fprintf(annotationfile, "%+.7f", end_from_start);
		else if(csv_format == 2) fprintf(annotationfile, "%02i:%02i:%02i", tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 3) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i", tm_end.year, tm_end.month, tm_end.day, tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 4) fprintf(annotationfile, "%02i:%02i:%02i.%07i", tm_end.hour, tm_end.minute, tm_end.second, remainder);
		else if(csv_format == 5) fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i", tm_end.year, tm_end.month, tm_end.day, tm_end.hour, tm_end.minute, tm_end.second, remainder);
		else if(csv_format == 6) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i", tm_end.month, tm_end.day, tm_end.year, tm_end.hour, tm_end.minute, tm_end.second);
		else if(csv_format == 7) fprintf(annotationfile, "%02i/%02i/%04i %02i:%02i:%02i.%07i", tm_end.month, tm_end.day, tm_end.year, tm_end.hour, tm_end.minute, tm_end.second, remainder);
		fprintf(annotationfile, "%c", separator);
	}

	fprintf(annotationfile, "%s", str);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
	fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
	fprintf(annotationfile, "\r\n");
#endif

    }

    fclose(annotationfile);

    edfplus_annotation_delete_list(&annot_list);

//    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
//    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
//	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
//    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// XML export //////////////////////////////////////

  if(csv_format == 8)
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Annotation backup: Cannot open annotationfile for writing.");
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_delete_list(&annot_list);
      return;
    }

    fprintf(annotationfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    fprintf(annotationfile, "<!-- Generated by " PROGRAM_NAME " " PROGRAM_VERSION " -->\n");

    fprintf(annotationfile, "<annotationlist>\n");

    utc_to_date_time(hdr->utc_starttime, &tm);

    fprintf(annotationfile, " <recording_start_time>%04i-%02i-%02iT%02i:%02i:%02i.%07i</recording_start_time>",
                            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, (int)hdr->starttime_offset);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
	fprintf(annotationfile, "\n");
#elif defined(Q_OS_WIN32)
	fprintf(annotationfile, "\r\n");
#endif

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_item(&annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      remainder = annot->onset % TIME_DIMENSION;

      if(remainder < 0)
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);

        remainder = TIME_DIMENSION + remainder;
      }
      else
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
      }

      fprintf(annotationfile, " <annotation>\n"
                              "  <onset>%04i-%02i-%02iT%02i:%02i:%02i.%07i</onset>\n"
                              "  <duration>%s</duration>\n"
                              "  <description>",
                              tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, remainder, annot->duration);

      xml_fwrite_encode_entity(annotationfile, annot->annotation);

      fprintf(annotationfile, "</description>\n" " </annotation>\n");
    }

    fprintf(annotationfile, "</annotationlist>\n");

    fclose(annotationfile);

    edfplus_annotation_delete_list(&annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Annotation backup completed.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// EDFplus export //////////////////////////////////////

  if(csv_format == 0)
  {
    hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, 0);
    if(hdl < 0)
    {
      switch(hdl)
      {
        case EDFLIB_MALLOC_ERROR : strcpy(str, "EDFlib: malloc error");
                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY : strcpy(str, "EDFlib: no such file or directory");
                                  break;
        case EDFLIB_MAXFILES_REACHED : strcpy(str, "EDFlib: maximum files reached");
                                  break;
        case EDFLIB_FILE_ALREADY_OPENED : strcpy(str, "EDFlib: file already opened");
                                  break;
        case EDFLIB_NUMBER_OF_SIGNALS_INVALID : strcpy(str, "EDFlib: number of signals is invalid");
                                  break;
        default : strcpy(str, "EDFlib: unknown error");
                                  break;
      }

      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
      messagewindow.exec();
      edfplus_annotation_delete_list(&annot_list);
      ExportAnnotsDialog->close();
      return;
    }

    utc_to_date_time(hdr->utc_starttime, &tm);
    edf_set_startdatetime(hdl, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    if((hdr->edfplus) || (hdr->bdfplus))
    {
      edf_set_patientname(hdl, hdr->plus_patient_name);
      edf_set_patientcode(hdl, hdr->plus_patientcode);
      if(!(strcmp(hdr->plus_gender, "Male")))
      {
        edf_set_gender(hdl, 1);
      }
      if(!(strcmp(hdr->plus_gender, "Female")))
      {
        edf_set_gender(hdl, 0);
      }
      if(hdr->plus_birthdate[0] != 0)
      {
        if(!(strncmp(hdr->plus_birthdate + 3, "jan", 3)))  n = 1;
        if(!(strncmp(hdr->plus_birthdate + 3, "feb", 3)))  n = 2;
        if(!(strncmp(hdr->plus_birthdate + 3, "mar", 3)))  n = 3;
        if(!(strncmp(hdr->plus_birthdate + 3, "apr", 3)))  n = 4;
        if(!(strncmp(hdr->plus_birthdate + 3, "may", 3)))  n = 5;
        if(!(strncmp(hdr->plus_birthdate + 3, "jun", 3)))  n = 6;
        if(!(strncmp(hdr->plus_birthdate + 3, "jul", 3)))  n = 7;
        if(!(strncmp(hdr->plus_birthdate + 3, "aug", 3)))  n = 8;
        if(!(strncmp(hdr->plus_birthdate + 3, "sep", 3)))  n = 9;
        if(!(strncmp(hdr->plus_birthdate + 3, "oct", 3)))  n = 10;
        if(!(strncmp(hdr->plus_birthdate + 3, "nov", 3)))  n = 11;
        if(!(strncmp(hdr->plus_birthdate + 3, "dec", 3)))  n = 12;
        edf_set_birthdate(hdl, atoi(hdr->plus_birthdate + 7), n, atoi(hdr->plus_birthdate));
      }
      edf_set_patient_additional(hdl, hdr->plus_patient_additional);
      edf_set_admincode(hdl, hdr->plus_admincode);
      edf_set_technician(hdl, hdr->plus_technician);
      edf_set_equipment(hdl, hdr->plus_equipment);
      edf_set_recording_additional(hdl, hdr->plus_recording_additional);
    }
    else
    {
      edf_set_patientname(hdl, hdr->patient);
      edf_set_recording_additional(hdl, hdr->recording);
    }

    int hasdot,
        decimals;

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_item(&annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      if(annot->duration[0] == 0)
      {
        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, -1LL, annot->annotation);
      }
      else
      {
        strcpy(str, annot->duration);

        len = strlen(str);

        hasdot = 0;

        for(i=0; i<len; i++)
        {
          if(str[i] == '.')
          {
            hasdot = 1;

            for(decimals=0; decimals<4; decimals++)
            {
              str[i] = str[i+1];

              if(str[i] == 0)
              {
                for( ; decimals<4; decimals++)
                {
                  str[i] = '0';

                  i++;
                }

                i--;
              }

              i++;
            }

            str[i] = 0;

            break;
          }
        }

        if(!hasdot)
        {
          strcat(str, "0000");
        }

        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, atoi(str), annot->annotation);
      }
    }

    if(edfclose_file(hdl) != 0)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred: edfclose_file()");
	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
      messagewindow.exec();
    }

    edfplus_annotation_delete_list(&annot_list);

//    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
//    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
//	QTimer::singleShot(2000, &messagewindow, SLOT(hide()));
//    messagewindow.exec();

    ExportAnnotsDialog->close();
  }
}













