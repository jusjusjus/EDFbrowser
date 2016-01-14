

#ifndef EVENT_H
#define EVENT_H



#include <QApplication>
#include <QObject>
#include <QAction>
#include <QDockWidget>
#include <QByteArray>
#include <QString>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


#define LONGDURATION 1.e8


class Event : public QAction
{

public:
	Event(const char* name, QObject* parent, double min=0., double max=LONGDURATION);

	bool check(double duration);

private:
	double minDuration,
	       maxDuration;

};

#endif




