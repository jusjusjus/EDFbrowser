


#include "Event.h"



Event::Event(const char *name, QObject* parent, double min, double max) : QAction(name, parent), minDuration(min), maxDuration(max)
{
}



bool Event::check(double duration)
{
	QMessageBox *msgbox=NULL;
	bool answer=true;

	if(duration < this->minDuration)	// too short
		msgbox = new QMessageBox(QMessageBox::Warning, "Event::check", "Duration to short to be of chosen type.", 0, parentWidget());

	else if(duration > this->maxDuration)	// too long
		msgbox = new QMessageBox(QMessageBox::Warning, "Event::check", "Duration to long to be of chosen type.", 0, parentWidget());

	else				// everything ok
		return answer;	// which is true

	msgbox->addButton(tr("Ignore"), QMessageBox::AcceptRole);
	msgbox->addButton(tr("Cancel"), QMessageBox::RejectRole);

	answer = (msgbox->exec() == QMessageBox::AcceptRole);

	if(msgbox != NULL) delete msgbox;	// cleanup; check for NULL is actually not needed.

	return answer;
}

