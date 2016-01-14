/*
***************************************************************************
*
*
***************************************************************************
*/
/*
 *
 * Signaltype
 *
 */




#ifndef SIGNALTYPE_H
#define SIGNALTYPE_H



#include <QApplication>
#include <QObject>
#include <QPoint>
#include <QMenu>
#include <QActionGroup>
#include <QDockWidget>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QActionGroup>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <QDomDocument>

#include "global.h"
#include "Signaltypes.h"
#include "Event.h"



class Signaltypes;
class Event;



class Signaltype : public QObject
{
	Q_OBJECT

public:
	Signaltype(QObject *theparent);
	Signaltype(QObject *theparent, const char* name);
	Signaltype(QObject *theparent, QDomElement*);
	Signaltype();

	Signaltypes *parent;

	void addAction(const char*, double min=0., double max=1.e8);	
	void exec(QPoint&);

	QString name;			// Names the types.  E.g.: "EEG"

private: 
	Event* customevent;
	QMenu* contextmenu;		// context menus for type.
	QActionGroup* events;


public slots:
	bool isinstance(QString*);
};

#endif




