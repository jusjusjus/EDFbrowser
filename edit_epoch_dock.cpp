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



#include "edit_epoch_dock.h"
#include "epochs_dock.h"



UI_EpochEditwindow::UI_EpochEditwindow(QWidget *w_parent)
{

  mainwindow = (UI_Mainwindow *)w_parent;
  annot_num = -1;
  annotation = NULL;

  dockedit = new QDockWidget(w_parent);                       // this is the dock
  dockedit->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);              // can be on bottom or on top
  dockedit->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); // ...

  annot_edit_widget = new QWidget;      // This widget constitutes the button part of the dock.
  annot_edit_widget->setMinimumSize(640, 45);
  dockedit->setWidget(annot_edit_widget);

  QObject::connect(dockedit,  SIGNAL(visibilityChanged(bool)), this,  SLOT(open_close_dock(bool))); //
}



void UI_EpochEditwindow::setup() // load xml scoring information (here only presets)
{
  QPushButton *new_button;
  load_xml();

  // ### create buttons
  stage_buttons = new QButtonGroup();
  QHBoxLayout *stage_layout = new QHBoxLayout();

  for(unsigned id=0; id<stages.size(); id++)
  {
    new_button = new QPushButton( *(stages[id]) );  // Create new button that says *stages[id].
    new_button->setEnabled(false);      // Per se, buttons are disabled (enabled upon stage selection).

    if(not (keys[id] == NULL) )
    {
      new_button->setShortcut(*(keys[id]));
    }

    stage_buttons->addButton( new_button, id ); // Store in the box for buttons.
    stage_layout->addWidget( new_button );    // Put it on the screen.
  }

  QGridLayout* grid = new QGridLayout(annot_edit_widget);
  grid->addLayout(stage_layout, 0, 0);

  QObject::connect(stage_buttons, SIGNAL(buttonClicked(int)), this, SLOT(button_pressed(int))); //
}


void UI_EpochEditwindow::load_xml() // load xml scoring information (here only presets)
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
  {
    QMessageBox::warning(mainwindow, tr("Epochsconfig"), tr("Cannot read file %1:\n%2.").arg(cfg_path).arg(file.errorString()));
    load_default();
    return;
  }

  if(! domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn) )
  {
    QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
    load_default();
    return;
  }


  root = domDocument.documentElement();
  if( root.tagName() != "Epochsconfig" )
  {
    QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("%1 is not a Epochsconfig.xml file.").arg(cfg_path) );
    load_default();
    return;
  }

  else if( root.hasAttribute("version") && root.attribute("version") != "0.0.1" )
  {
    QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("%1 is not Epochsconfig.xml version 0.0.1 file.").arg(cfg_path) );
    load_default();
    return;
  }

  root = root.firstChildElement("Epochtypes");
  if( root.isNull() )
  {
    QMessageBox::information(mainwindow, tr("Epochsconfig"), tr("No epochtypes defined in %1.").arg(cfg_path) );
    load_default();
    return;
  }

  for(child = root.firstChildElement("Epochtype"); not child.isNull(); child = child.nextSiblingElement("Epochtype"))
  {
    stages.push_back( new QString(child.attribute("Annotation")) );

    if( child.hasAttribute("Shortcut") )
      keys.push_back( new QKeySequence( child.attribute("Shortcut") ) );
    else                      // no shortcut
      keys.push_back( NULL );
  }
}



void UI_EpochEditwindow::load_default() // Loads some default epoch labels.
{
  stages.push_back( new QString("Wake") );
  keys.push_back( new QKeySequence("W") );

  stages.push_back( new QString("N1") );
  keys.push_back( new QKeySequence("1") );

  stages.push_back( new QString("N2") );
  keys.push_back( new QKeySequence("2") );

  stages.push_back( new QString("N3") );
  keys.push_back( new QKeySequence("3") );

  stages.push_back( new QString("R") );
  keys.push_back( new QKeySequence("R") );
}



void UI_EpochEditwindow::button_pressed(int id)     // id: the identity of the pushed button.  (stages[id])->toLatin1().data(): nullterminated char* naming the stage that was selected.
{
  int currentRow;

  mainwindow->epochs_dock->setSelectedText(*stages[id]);
  mainwindow->epochs_dock->setCurrentRow(mainwindow->epochs_dock->currentRow()+1);

  currentRow = mainwindow->epochs_dock->currentRow();

  if(currentRow == -1) deselect();
}



void UI_EpochEditwindow::open_close_dock(bool visible)
{
  if(visible)
  {
    mainwindow->epoch_editor_active = 1;
    mainwindow->show_annot_markers = 1;
//    if(mainwindow->annotationlist_backup == 0)      // create backup annotation list.
//      mainwindow->annotationlist_backup = edfplus_annotation_copy_list( &mainwindow->epochlist[0] );
  }
  else
  {
    mainwindow->epoch_editor_active = 0;
  }
}



void UI_EpochEditwindow::set_selected_annotation(int annot_nr)
{
  annot_num = annot_nr; // select annotation number

  struct annotationblock *annot = edfplus_annotation_item(&mainwindow->epochlist[0], annot_num);  // retreive the selected annotation

  set_selected_annotation(annot);
}



void UI_EpochEditwindow::set_selected_annotation(struct annotationblock *annot)
{
  if(annot == 0) return;  // if it is not a good annotation. don't continue
  annotation = annot;

  QList<QAbstractButton*> buttons = stage_buttons->buttons();
  for(int i=0; i<buttons.size(); i++)
  {
    static_cast<QPushButton*>(buttons.at(i))->setEnabled(true);   // Annotation can be modified.
  }
}



void UI_EpochEditwindow::deselect()
{
  annotation = 0;

  QList<QAbstractButton*> buttons = stage_buttons->buttons();
  for(int i=0; i<buttons.size(); i++)
  {
    static_cast<QPushButton*>(buttons.at(i))->setEnabled(false);    // Annotation can be modified.
  }
}



