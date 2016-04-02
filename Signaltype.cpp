


#include "Signaltype.h"



Signaltype::Signaltype(QObject* theparent, const char *thename) : name(thename)
{
	parent = (Signaltypes*)theparent;
	contextmenu = new QMenu(parent->parent->mainwindow);
	events = new QActionGroup(parent->parent->mainwindow);
	customevent = new Event("Custom ...", this);
	contextmenu->addAction(customevent);

	QObject::connect(customevent, SIGNAL(triggered()), parent->parent, SLOT(annotEditSetAnnotation()));
	QObject::connect(events, SIGNAL(triggered(QAction*)), parent->parent, SLOT(annotEditSetAnnotation(QAction*)));
}



Signaltype::Signaltype(QObject* theparent, QDomElement *signaltype) : name("Unspecified")
{
	parent = (Signaltypes*)theparent;
	contextmenu = new QMenu(parent->parent->mainwindow);
	events = new QActionGroup(parent->parent->mainwindow);
	customevent = new Event("Custom ...", this);
	contextmenu->addAction(customevent);

	QObject::connect(customevent, SIGNAL(triggered()), parent->parent, SLOT(annotEditSetAnnotation()));
	QObject::connect(events, SIGNAL(triggered(QAction*)), parent->parent, SLOT(annotEditSetAnnotation(QAction*)));

	if ( not signaltype->hasAttribute("Name") ) { QMessageBox::information(parent->parent->mainwindow, tr("Signaltype"), tr("%1 has no Name attribute.").arg(signaltype->tagName()) ); return; }
	name = signaltype->attribute("Name");

	for(QDomElement child = signaltype->firstChildElement("Event");
			not child.isNull();
			child = child.nextSiblingElement("Event"))
	{
		if ( not child.hasAttribute("Name") ) { QMessageBox::information(parent->parent->mainwindow, tr("Signaltype"), tr("%1 has no Name attribute.").arg(child.tagName()) ); continue; }

		if( child.hasAttribute("min") )
		{
			if( child.hasAttribute("max") )
				addAction( child.attribute("Name").toLatin1().data(), atof(child.attribute("min").toLatin1().data()), atof(child.attribute("max").toLatin1().data()) );
			else
				addAction( child.attribute("Name").toLatin1().data(), atof(child.attribute("min").toLatin1().data()) );
		}
		else
		{
			addAction( child.attribute("Name").toLatin1().data() );
		}
	}
}



QDomDocument Signaltype::toQDom()
{
	QDomDocument tree;
	QDomElement type = tree.createElement("Signaltype");
	type.setAttribute("Name", name);

	QList<QAction*> actions = events->actions();
	QList<QAction*>::iterator action;

	for(action=actions.begin(); action!=actions.end(); action++)
	{
		QDomElement act = tree.createElement("Event");
		act.setAttribute( "Name", (*action)->text() );
		type.appendChild(act);
	}

	return tree;
}



void Signaltype::addAction(const char* name, double min, double max)
{
	Event *newevent = new Event(name, this, min, max);

	if( events->actions().length() != 0 )
	{
		events->addAction(newevent);
		contextmenu->insertAction(events->actions().first(), newevent);
	}
	else
	{
		if( contextmenu ) delete contextmenu;
		contextmenu = new QMenu(parent->parent->mainwindow);
		contextmenu->addAction(newevent);
		contextmenu->addSeparator();
		contextmenu->addAction(customevent);

		events->addAction(newevent);				// add it to the group
		return;
	}
}



bool Signaltype::isinstance(QString* signalDescriptor)
{
	return signalDescriptor->contains(name);
}



void Signaltype::exec(QPoint& point)
{
	contextmenu->exec(point);
}
