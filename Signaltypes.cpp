


#include "Signaltypes.h"



Signaltypes::Signaltypes(QObject *myparent)
{
	parent = (UI_AnnotationEditwindow*)myparent;
	load_types();
}



Signaltypes::~Signaltypes()
{
	for(unsigned i=0; i<types.size(); i++) delete types[i];
}



void Signaltypes::load_types()
{
	char cfg_path[MAX_PATH_LENGTH];
	QString errorStr;
	int errorLine,
		errorColumn;
	QDomDocument domDocument;

	types.push_back( new Signaltype(this, "Unspecified") );			// Include the default type.

	configpath(cfg_path, "Signaltypes.xml");
	QFile file(cfg_path);
	if(! file.open(QFile::ReadOnly | QFile::Text) )
						{ QMessageBox::warning(parent->mainwindow, tr("Signaltypes"), tr("Cannot read file %1:\n%2.").arg(cfg_path).arg(file.errorString())); return; }

	if(! domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn) )
						{ QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr)); return; }


	QDomElement root = domDocument.documentElement();
	if( root.tagName() != "Signaltypes" )
						{ QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("%1 is not a Signaltypes.xml file.").arg(cfg_path) ); return; }

	else if( root.hasAttribute("version") && root.attribute("version") != "0.0.1" )
						{ QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("%1 is not Signaltypes.xml version 0.0.1 file.").arg(cfg_path) ); return; }


	for(QDomElement child = root.firstChildElement("Signaltype"); not child.isNull(); child = child.nextSiblingElement("Signaltype"))
		types.push_back( new Signaltype(this, &child) );

}



void Signaltypes::exec(int type_number, int x, int y)
{
	QPoint point(x, y);
	types[type_number]->exec( point );
}


void Signaltypes::registerSignaltypes()
{
	struct signalcompblock *signalcomp;
	struct edfparamblock *edfparam;
	unsigned signalcomps = parent->mainwindow->signalcomps;
	bool ask=false;

	for(unsigned i=0; i<signalcomps; i++)
	{
		signalcomp = parent->mainwindow->signalcomp[i];		// current signalcomp
		if(signalcomp->type == -1)				// -1 : type default value
		{
			edfparam   = signalcomp->edfhdr->edfparam;		// edfparam of signal composition.

			signalcomp->type = get_type_from_label(edfparam[signalcomp->edfsignal[0]].label);
	
			for(int j=1; j<signalcomp->num_of_signals; j++)	// inconsistencies in type, e.g. units, are only problematic with signal combinations (num_of_signals>1).
			{
				if( signalcomp->type != get_type_from_label(edfparam[signalcomp->edfsignal[j]].label) )
				{
    					QMessageBox messagewindow(QMessageBox::Critical, "Error", "In Signaltypes::registerSignaltypes(): Your signal composition consists of incompatible types.  Setting to Unspecified.");
    					messagewindow.exec();
					signalcomp->type = 0;
				}
			}
			if(signalcomp->type == 0) ask = true;
		}
	}
	if(ask) ask_for_types();		// if not all types were determined, fire up the user interface.
}



int Signaltypes::get_type_from_label(char *label)
{
	QString qlabel = QString(label);

	for(unsigned i=0; i<types.size(); i++)
	{
		if( types[i]->isinstance(&qlabel) ) return (int)i;
	}
	return 0;	// No type?
}



void Signaltypes::ask_for_types()
{
	UI_Signaltype_dialog win(this); // segmentation fault!!!
}





