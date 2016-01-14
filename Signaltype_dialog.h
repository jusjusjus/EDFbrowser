



#ifndef SIGNALTYPE_DIALOG_H
#define SIGNALTYPE_DIALOG_H



#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QTime>
#include <QSpinBox>
#include <QMessageBox>

#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "Signaltypes.h"
#include "popup_messagewindow.h"



class UI_Mainwindow;
class Signaltypes;



class UI_Signaltype_dialog : public QObject
{
	Q_OBJECT


public:
	UI_Signaltype_dialog(Signaltypes *parent);
	Signaltypes *parent;
	UI_Mainwindow *mainwindow;


private:
	QDialog	*dialog;

	std::vector<QComboBox*> Choices;

	QGridLayout *layout;

	QListWidget *typelist;

	QPushButton *update,
			*close;


private slots:
	void updateit();
};



#endif // SIGNALTYPE_DIALOG_H


