



#ifndef SIGNALTYPEOPTIONS_H
#define SIGNALTYPEOPTIONS_H



#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QColor>
#include <QSpinBox>
#include <QPalette>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QStringList>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QString>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextEdit>
#include <QTextStream>

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "special_button.h"
#include "signalcurve.h"



class UI_Mainwindow;



class SignaltypeOptions : public QWidget
{
  Q_OBJECT

public:
  SignaltypeOptions(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

  QTextEdit *document;

  QPushButton *reloadButton,
              *saveButton;

private slots:

  void load_xml();
  void save_xml();
  bool check_xml();
};



#endif // SIGNALTYPEOPTIONS_H


