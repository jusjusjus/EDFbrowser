

#include "signaltype_options.h"




SignaltypeOptions::SignaltypeOptions(QWidget *w_parent) : QWidget() // tab_number=0
{
	mainwindow = (UI_Mainwindow *)w_parent;

	QVBoxLayout *vbox = new QVBoxLayout;
	this->setLayout(vbox);

	document = new QTextEdit;
	document->setLineWrapMode(QTextEdit::NoWrap);
	vbox->addWidget(document);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	vbox->addLayout(buttonLayout);

	reloadButton = new QPushButton("Reload");
	buttonLayout->addWidget(reloadButton);

	saveButton   = new QPushButton("Save");
	buttonLayout->addWidget(saveButton);

	QObject::connect(reloadButton,	SIGNAL(clicked()),	this, SLOT(load_xml()));
	QObject::connect(saveButton,	SIGNAL(clicked()),	this, SLOT(save_xml()));

	this->load_xml();
}



void SignaltypeOptions::load_xml()
{
	char cfg_path[MAX_PATH_LENGTH];
	bool success;
	
	document->clear();			// Empty the file.

	// open the file to read
	configpath(cfg_path, "Signaltypes.xml");
	QFile file(cfg_path);
	success = file.open(QFile::ReadOnly | QFile::Text); 
	if(not success) { QMessageBox::warning(mainwindow, tr("SignaltypeOptions"), tr("Cannot read file %1:\n%2.").arg(cfg_path).arg(file.errorString())); return; }

	// Read the file into the document.
	QTextStream stream( &file );
	
	document->setText( stream.readAll() );

	file.close();
}



void SignaltypeOptions::save_xml()
{
	char cfg_path[MAX_PATH_LENGTH];
	bool success;

	success = this->check_xml();

	if(not success) return;

	// Writing-to-file part
	configpath(cfg_path, "Signaltypes.xml");
	QFile file(cfg_path);
	success = file.open( QIODevice::WriteOnly | QIODevice::Text ); 
	if(not success) { QMessageBox::warning(mainwindow, tr("SignaltypeOptions"), tr("Cannot write file %1:\n%2.").arg(cfg_path).arg(file.errorString())); return; } 
	QTextStream stream( &file );
	
	stream << document->toPlainText();

	file.close();

//	if(mainwindow->annotation_editor_active)
//	{
//		mainwindow->annotationEditWindow
//	}
}



bool SignaltypeOptions::check_xml()
{
	QString errorStr;
	int	errorLine,
		errorColumn;
	QDomDocument xmlDoc;

	if(not xmlDoc.setContent( document->toPlainText(), true, &errorStr, &errorLine, &errorColumn) )
						{ QMessageBox::information(mainwindow, tr("SignaltypeOptions"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr)); return false; }

	return true;
}
