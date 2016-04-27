/*
 *
 * Signaltypes:
 *
 * This class assigns a type to each channelcomposition.  All valid types
 * should be determined before the study.  The type assignment is stored in the
 * composition to simplify all other functions such as reoordering of traces.
 *
 */



#ifndef SIGNALTYPES_H
#define SIGNALTYPES_H



#include <QApplication>
#include <QObject>
#include <QDockWidget>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDomDocument>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "global.h"
#include "mainwindow.h"
#include "Signaltype.h"
#include "Signaltype_dialog.h"



class UI_AnnotationEditwindow;
class UI_Signaltype_dialog;
class Signaltype;



class Signaltypes : public QObject
{
	Q_OBJECT

public:
	Signaltypes(QObject *parent);
	~Signaltypes();

	UI_AnnotationEditwindow *parent;

	std::vector<Signaltype*> types;

	void reload_types(void);

private:

	QDomElement* getRootElement();
	void load_types(void);
	int get_type_from_label(char *label);
	void save_types(void);
	void default_types(void);
	void ask_for_types();
	void reset();


public slots:
	void registerSignaltypes(bool ask=true);
	void exec(int, int, int);
};

#endif




