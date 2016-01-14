



#include "Signaltype_dialog.h"



UI_Signaltype_dialog::UI_Signaltype_dialog(Signaltypes *theparent)
{
	parent = theparent;
	mainwindow = (UI_Mainwindow *)(theparent->parent->mainwindow);

	dialog = new QDialog(mainwindow);

	dialog->setWindowTitle("Select signal types");
	dialog->setModal(true);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);

	layout = new QGridLayout(dialog);

	update = new QPushButton(dialog);
	update->setGeometry(690, 455, 100, 25);
	update->setText("Update");

	close = new QPushButton(dialog);
	close->setGeometry(430, 455, 160, 25);
	close->setText("Cancel");

	QStringList *items = new QStringList();
	for(unsigned j=0; j<parent->types.size(); j++)
	{
		*items << parent->types[j]->name;
	}

	struct signalcompblock *signalcomp;
	QLabel *label;
	QComboBox *choices;
	for(int i=0; i<mainwindow->signalcomps; i++)
	{
		signalcomp = mainwindow->signalcomp[i];

		QString str("");
		for(int j=0; j<signalcomp->num_of_signals; j++)
		{
			str += " ";
			str += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].label;
			str += ";";
		}
		label = new QLabel(str);
		layout->addWidget(label, i, 0);

		choices = new QComboBox;
		choices->addItems(*items);
		choices->setCurrentIndex(signalcomp->type);
		Choices.push_back(choices);
		layout->addWidget(choices, i, 1);

	}
	layout->addWidget(close, mainwindow->signalcomps, 0);
	layout->addWidget(update, mainwindow->signalcomps, 1);

	QObject::connect(close, SIGNAL(clicked()), dialog, SLOT(close()));
	QObject::connect(update, SIGNAL(clicked()), this, SLOT(updateit()));

	dialog->exec();
}


void UI_Signaltype_dialog::updateit()
{
	for(int i=0; i<mainwindow->signalcomps; i++)
	{
		mainwindow->signalcomp[i]->type = Choices[i]->currentIndex();
	}
		
	dialog->close();
}





