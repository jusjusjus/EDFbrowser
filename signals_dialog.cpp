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



#include "signals_dialog.h"



UI_Signalswindow::UI_Signalswindow(QWidget *w_parent) : QDialog(w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  setMinimumSize(800, 500);
  setMaximumSize(800, 500);
  setWindowTitle("Signals");
  setModal(true);
//  setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(this);
  filelist->setGeometry(10, 10, 780, 75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  label1 = new QLabel(this);
  label1->setGeometry(10, 95, 760, 20);

  label2 = new QLabel(this);
  label2->setGeometry(10, 120, 760, 20);

  label3 = new QLabel(this);
  label3->setGeometry(10, 145, 250, 20);

  label4 = new QLabel(this);
  label4->setGeometry(270, 145, 250, 20);

  label5 = new QLabel(this);
  label5->setGeometry(10, 190, 120, 20);
  label5->setText("Signals in file");

  label6 = new QLabel(this);
  label6->setGeometry(430, 190, 120, 20);
  label6->setText("Signal Composition");

  colorlabel = new QLabel(this);
  colorlabel->setGeometry(320, 380, 100, 20);
  colorlabel->setText("   Color");

  signallist = new QListWidget(this);
  signallist->setGeometry(10, 210, 300, 225);
  signallist->setFont(*mainwindow->monofont);
  signallist->setSelectionBehavior(QAbstractItemView::SelectRows);
  signallist->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CloseButton = new QPushButton(this);
  CloseButton->setGeometry(690, 455, 100, 25);
  CloseButton->setText("&Close");

  SelectAllButton = new QPushButton(this);
  SelectAllButton->setGeometry(10, 455, 100, 25);
  SelectAllButton->setText("&Select All");

  HelpButton = new QPushButton(this);
  HelpButton->setGeometry(690, 120, 100, 25);
  HelpButton->setText("&Help");

  DisplayButton = new QPushButton(this);
  DisplayButton->setGeometry(150, 455, 160, 25);
  DisplayButton->setText("&Add signal(s)");

  DisplayCompButton = new QPushButton(this);
  DisplayCompButton->setGeometry(430, 455, 160, 25);
  DisplayCompButton->setText("&Make derivation");

  AddButton = new QPushButton(this);
  AddButton->setGeometry(320, 250, 100, 25);
  AddButton->setText("Add->");

  SubtractButton = new QPushButton(this);
  SubtractButton->setGeometry(320, 285, 100, 25);
  SubtractButton->setText("Subtract->");

  RemoveButton = new QPushButton(this);
  RemoveButton->setGeometry(320, 320, 100, 25);
  RemoveButton->setText("Remove<-");

  ColorButton = new SpecialButton(this);
  ColorButton->setGeometry(320, 405, 100, 25);
  ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);

  compositionlist = new QListWidget(this);
  compositionlist->setGeometry(430, 210, 360, 225);
  compositionlist->setFont(*mainwindow->monofont);
  compositionlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  compositionlist->setSelectionMode(QAbstractItemView::ExtendedSelection);

  smp_per_record = 0;

  QObject::connect(CloseButton,       SIGNAL(clicked()),                this,   SLOT(close()));
  QObject::connect(SelectAllButton,   SIGNAL(clicked()),                this,   SLOT(SelectAllButtonClicked()));
  QObject::connect(HelpButton,        SIGNAL(clicked()),                this,   SLOT(HelpButtonClicked()));
  QObject::connect(DisplayButton,     SIGNAL(clicked()),                this,   SLOT(DisplayButtonClicked()));
  QObject::connect(DisplayCompButton, SIGNAL(clicked()),                this,   SLOT(DisplayCompButtonClicked()));
  QObject::connect(AddButton,         SIGNAL(clicked()),                this,   SLOT(AddButtonClicked()));
  QObject::connect(SubtractButton,    SIGNAL(clicked()),                this,   SLOT(SubtractButtonClicked()));
  QObject::connect(RemoveButton,      SIGNAL(clicked()),                this,   SLOT(RemoveButtonClicked()));
  QObject::connect(ColorButton,       SIGNAL(clicked(SpecialButton *)), this,   SLOT(ColorButtonClicked(SpecialButton *)));
  QObject::connect(filelist,          SIGNAL(currentRowChanged(int)),   this,   SLOT(show_signals(int)));

  curve_color = mainwindow->maincurve->signal_color;

  filelist->setCurrentRow(mainwindow->files_open - 1);

  signallist->setFocus();

  exec();
}


void UI_Signalswindow::ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color);

  if(color < 0)  return;

  ColorButton->setColor((Qt::GlobalColor)color);

  curve_color = color;
}



void UI_Signalswindow::DisplayCompButtonClicked()
{
  int i, j, n;

  char str[512],
       str2[128];

  struct signalcompblock *newsignalcomp;

  QListWidgetItem *item;


  n = compositionlist->count();

  if(!n)
  {
    return;
  }

  newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
  if(newsignalcomp==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
    messagewindow.exec();
    close();
    return;
  }

  newsignalcomp->num_of_signals = n;
  newsignalcomp->filenum = filelist->currentRow();
  newsignalcomp->edfhdr = mainwindow->edfheaderlist[newsignalcomp->filenum];
  newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
  newsignalcomp->voltpercm = 100.0;
  newsignalcomp->color = curve_color;
  newsignalcomp->hasruler = 0;
  newsignalcomp->polarity = 1;
  newsignalcomp->type = -1;     // undetermined default type

  for(i=0; i<n; i++)
  {
    strcpy(str, compositionlist->item(i)->text().toLatin1().data());

    for(j=0; j<newsignalcomp->edfhdr->edfsignals; j++)
    {
      if(!strncmp(newsignalcomp->edfhdr->edfparam[j].label, str + 5, 16))
      {
        if(j != compositionlist->item(i)->data(Qt::UserRole).toInt())  continue;

        newsignalcomp->edfsignal[i] = j;
        newsignalcomp->factor[i] = str[23] - 48;
        if(str[3]=='-')
        {
          newsignalcomp->factor[i] = -(newsignalcomp->factor[i]);
          strcat(newsignalcomp->signallabel, "- ");
        }
        else
        {
          if(i)
          {
            strcat(newsignalcomp->signallabel, "+ ");
          }
        }
        strcpy(str2, newsignalcomp->edfhdr->edfparam[j].label);
        strip_types_from_label(str2);
        strcat(newsignalcomp->signallabel, str2);
        remove_trailing_spaces(newsignalcomp->signallabel);
        strcat(newsignalcomp->signallabel, " ");

        if(newsignalcomp->edfhdr->edfparam[j].bitvalue < 0.0)
        {
          newsignalcomp->voltpercm = -100.0;
        }
        newsignalcomp->sensitivity[i] = newsignalcomp->edfhdr->edfparam[j].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->pixelsizefactor);
      }
    }
  }

  remove_trailing_spaces(newsignalcomp->signallabel);
  newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

  strcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension);
  remove_trailing_spaces(newsignalcomp->physdimension);

  mainwindow->add_signalcomp(newsignalcomp);

  while(compositionlist->count())
  {
    item = compositionlist->takeItem(0);
    delete item;
  }

  mainwindow->setup_viewbuf();

  smp_per_record = 0;
}


void UI_Signalswindow::DisplayButtonClicked()
{
  int i, n, s;

  struct signalcompblock *newsignalcomp;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(n == 0)      // This request is probably not necessary.
  {
    close();
    return;
  }

  for(i=0; i<n; i++)
  {
    newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
    if(newsignalcomp == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
      messagewindow.exec();
      return;
    }

    newsignalcomp->num_of_signals = 1;
    newsignalcomp->filenum = filelist->currentRow();
    newsignalcomp->edfhdr = mainwindow->edfheaderlist[newsignalcomp->filenum];
    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
    newsignalcomp->voltpercm = 100.0;
    newsignalcomp->color = curve_color;
    newsignalcomp->hasruler = 0;
    newsignalcomp->polarity = 1;
    newsignalcomp->type = -1; // undetermined default type

    item = selectedlist.at(i);        // Get the i-th selection.
    s = item->data(Qt::UserRole).toInt();   // Get the index of the i-th selection ..
    newsignalcomp->edfsignal[0] = s;              // .. and set it ..
    newsignalcomp->factor[0] = 1;               // .. with factor 1.
    if(newsignalcomp->edfhdr->edfparam[s].bitvalue < 0.0)
    {
      newsignalcomp->voltpercm = -100.0;
    }
    newsignalcomp->sensitivity[0] = newsignalcomp->edfhdr->edfparam[s].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->pixelsizefactor);

    strcpy(newsignalcomp->signallabel, newsignalcomp->edfhdr->edfparam[s].label);
    strip_types_from_label(newsignalcomp->signallabel);
    remove_trailing_spaces(newsignalcomp->signallabel);

    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

    newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

    strcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[s].physdimension);
    remove_trailing_spaces(newsignalcomp->physdimension);

    mainwindow->add_signalcomp(newsignalcomp);
  }

  mainwindow->setup_viewbuf();
}


void UI_Signalswindow::RemoveButtonClicked()
{
  int i, n, row;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = compositionlist->selectedItems();

  n = selectedlist.size();

  if(!n)  return;

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    row = compositionlist->row(item);
    item = compositionlist->takeItem(row);
    delete item;
  }
}


void UI_Signalswindow::AddButtonClicked()
{
  int i, j, k, n, s, row, duplicate;

  char str[256];

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)  return;

  if(!compositionlist->count())
  {
    smp_per_record = 0;
    physdimension[0] = 0;
    bitvalue = 0.0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    row = filelist->currentRow();

    if(smp_per_record)
    {
      if(smp_per_record!=mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record)
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      smp_per_record = mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record;
    }

    if(physdimension[0])
    {
      if(strcmp(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension))
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      strcpy(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension);
    }

    if(bitvalue!=0.0)
    {
      if(bitvalue!=mainwindow->edfheaderlist[row]->edfparam[s].bitvalue)
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      bitvalue = mainwindow->edfheaderlist[row]->edfparam[s].bitvalue;
    }

    duplicate = 0;

    k = compositionlist->count();
    for(j=0; j<k; j++)
    {
      item = compositionlist->item(j);
      strcpy(str, item->text().toLatin1().data());
      if(!strncmp(mainwindow->edfheaderlist[row]->edfparam[s].label, str + 5, 16))
      {
        if(str[3]=='+')
        {
          if(s == item->data(Qt::UserRole).toInt())
          {
            duplicate = 1;
            break;
          }
        }
      }
    }
    if(duplicate)
    {
      if(str[23]==57)  continue;

      str[23] += 1;
      item->setText(str);
    }
    else
    {
      snprintf(str, 200, "%-2i + ", row + 1);
      strcat(str, mainwindow->edfheaderlist[row]->edfparam[s].label);
      strcat(str, " x1");
      snprintf(str + strlen(str), 50, " %5.fHz", mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record / mainwindow->edfheaderlist[row]->data_record_duration);
      item = new QListWidgetItem;
      item->setData(Qt::UserRole, QVariant(s));
      item->setText(str);
      compositionlist->addItem(item);
    }
  }
}


void UI_Signalswindow::SubtractButtonClicked()
{
  int i, j, k, n, s, row, duplicate;

  char str[256];

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)  return;

  if(!compositionlist->count())
  {
    smp_per_record = 0;
    physdimension[0] = 0;
    bitvalue = 0.0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    row = filelist->currentRow();

    if(smp_per_record)
    {
      if(smp_per_record!=mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record)
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      smp_per_record = mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record;
    }

    if(physdimension[0])
    {
      if(strcmp(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension))
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      strcpy(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension);
    }

    if(bitvalue!=0.0)
    {
      if(bitvalue!=mainwindow->edfheaderlist[row]->edfparam[s].bitvalue)
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning",
                                  "It is only possible to make combinations/derivations with signals which:\n"
                                  " - are from the same file\n"
                                  " - have the same samplerate\n"
                                  " - have the same physical dimension (e.g. uV)\n"
                                  " - have the same sensitivity (e.g. uV/bit)");
        messagewindow.exec();

        continue;
      }
    }
    else
    {
      bitvalue = mainwindow->edfheaderlist[row]->edfparam[s].bitvalue;
    }

    duplicate = 0;

    k = compositionlist->count();
    for(j=0; j<k; j++)
    {
      item = compositionlist->item(j);
      strcpy(str, item->text().toLatin1().data());
      if(!strncmp(mainwindow->edfheaderlist[row]->edfparam[s].label, str + 5, 16))
      {
        if(str[3]=='-')
        {
          duplicate = 1;
          break;
        }
      }
    }
    if(duplicate)
    {
      if(str[23]==57)  continue;

      str[23] += 1;
      item->setText(str);
    }
    else
    {
      snprintf(str, 200, "%-2i - ", row + 1);
      strcat(str, mainwindow->edfheaderlist[row]->edfparam[s].label);
      strcat(str, " x1");
      snprintf(str + strlen(str), 50, " %5.fHz", mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record / mainwindow->edfheaderlist[row]->data_record_duration);
      item = new QListWidgetItem;
      item->setData(Qt::UserRole, QVariant(s));
      item->setText(str);
      compositionlist->addItem(item);
    }
  }
}


void UI_Signalswindow::show_signals(int row)
{
  int i, j, len, skip, signal_cnt;

  char str[256];

  long long file_duration;

  QListWidgetItem *item;

  struct date_time_struct date_time;


  if(row<0)  return;

  while(signallist->count())
  {
    delete signallist->item(0);
  }

  if((mainwindow->edfheaderlist[row]->edfplus)||(mainwindow->edfheaderlist[row]->bdfplus))
  {
    strcpy(str, "Subject    ");
    strcat(str, mainwindow->edfheaderlist[row]->plus_patient_name);
    strcat(str, "  ");
    strcat(str, mainwindow->edfheaderlist[row]->plus_birthdate);
    strcat(str, "  ");
    strcat(str, mainwindow->edfheaderlist[row]->plus_patientcode);
    label1->setText(str);
    strcpy(str, "Recording  ");
    strcat(str, mainwindow->edfheaderlist[row]->plus_admincode);
    label2->setText(str);
  }
  else
  {
    strcpy(str, "Subject    ");
    strcat(str, mainwindow->edfheaderlist[row]->patient);
    len = strlen(str);
    for(j=0; j<len; j++)
    {
      if(str[j]=='_')
      {
        str[j] = ' ';
      }
    }
    label1->setText(str);
    strcpy(str, "Recording  ");
    strcat(str, mainwindow->edfheaderlist[row]->recording);
    len = strlen(str);
    for(j=0; j<len; j++)
    {
      if(str[j]=='_')
      {
        str[j] = ' ';
      }
    }
    label2->setText(str);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str, 256, "Start      %i %s %i   %2i:%02i:%02i",
           date_time.day,
           date_time.month_str,
           date_time.year,
           date_time.hour,
           date_time.minute,
           date_time.second);

  label3->setText(str);

  strcpy(str, "Duration   ");
  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;
  snprintf(str + 11, 240,
           "%2i:%02i:%02i",
           (int)((file_duration / TIME_DIMENSION)/ 3600LL),
           (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
           (int)((file_duration / TIME_DIMENSION) % 60LL));
  label4->setText(str);

  skip = 0;

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  for(i=0; i<signal_cnt; i++)
  {
    if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
    {
      for(j=0; j<mainwindow->edfheaderlist[row]->nr_annot_chns; j++)
      {
        if(i==mainwindow->edfheaderlist[row]->annot_ch[j])
        {
          skip = 1;
          break;
        }
      }
      if(skip)
      {
        skip = 0;
        continue;
      }
    }

    snprintf(str, 256, "%-3i ", i + 1);
    strcat(str, mainwindow->edfheaderlist[row]->edfparam[i].label);
    sprintf(str + strlen(str), "   %f", mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record / mainwindow->edfheaderlist[row]->data_record_duration);
    remove_trailing_zeros(str);
    strcat(str, " Hz");
    item = new QListWidgetItem;
    item->setText(str);
    item->setData(Qt::UserRole, QVariant(i));
    signallist->addItem(item);
  }

  SelectAllButtonClicked();
}


void UI_Signalswindow::SelectAllButtonClicked()
{
  int row, i;

  QListWidgetItem *item;

  row = signallist->count();

  for(i=0; i<row; i++)
  {
    item = signallist->item(i);
    item->setSelected(true);
  }
}


void UI_Signalswindow::HelpButtonClicked()
{
  UI_Messagewindow popup_help("Help",

                              "On top you will see a list of opened files.\n"
                              "Select (highlight) the file from which you want to add signals. At the left part of the dialog you see a list of\n"
                              "all the signals which are in the selected file. Select one or more signals and click on the \"Add signals\"\n"
                              "button. Now you will see the selected signals on the screen.\n"

                              "\nWhen you want to make a combination (derivation) of two or more signals, do as follows.\n"
                              "Select the file from which you want to add signals. For example, we choose an EEG file which contains\n"
                              "the signals \"P3\" and \"C3\" and we want to subtract \"C3\" from \"P3\".\n"
                              "- Select (highlight) the signal \"P3\".\n"
                              "- Click on the \"Add\" button.\n"
                              "- Select (highlight) the signal \"C3\".\n"
                              "- Click on the \"Subtract\" button.\n"
                              "- Click on the \"Make derivation\" button.\n"
                              "- The result of \"P3\" minus \"C3\" will appear on the screen.");
}


void UI_Signalswindow::strip_types_from_label(char *label)
{
  int i,
      len;


  len = strlen(label);
  if(len<16)
  {
    return;
  }

  if((!(strncmp(label, "EEG ", 4)))
      ||(!(strncmp(label, "ECG ", 4)))
      ||(!(strncmp(label, "EOG ", 4)))
      ||(!(strncmp(label, "ERG ", 4)))
      ||(!(strncmp(label, "EMG ", 4)))
      ||(!(strncmp(label, "MEG ", 4)))
      ||(!(strncmp(label, "MCG ", 4))))
  {
    if(label[4]!=' ')
    {
      for(i=0; i<(len-4); i++)
      {
        label[i] = label[i+4];
      }

      for(; i<len; i++)
      {
        label[i] = ' ';
      }
    }
  }
}









