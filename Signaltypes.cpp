


#include "Signaltypes.h"



Signaltypes::Signaltypes(QObject *myparent)
{
  parent = (UI_AnnotationEditwindow*)myparent;
  load_types();
}



Signaltypes::~Signaltypes()
{
  reset();
}



void Signaltypes::reset()
{
  struct signalcompblock *signalcomp;
  unsigned signalcomps;

  signalcomps = parent->mainwindow->signalcomps;

  // Delete existing types.
  for(unsigned i=0; i < types.size(); i++) delete types[i];
  types.resize(0);

  // Clear existing signalcomp hashes to avoid seg-faults.
  for(unsigned i=0; i<signalcomps; i++)
  {
    signalcomp = parent->mainwindow->signalcomp[i];   // current signalcomp
    signalcomp->type = 0; // -1 : type default value.  0 : Unspecified
  }
}



QDomElement* Signaltypes::getRootElement()
{
  char cfg_path[MAX_PATH_LENGTH];
  QString errorStr;
  int errorLine,
      errorColumn;
  QDomDocument document;
  bool  success;

  configpath(cfg_path, "Signaltypes.xml");
  QFile file(cfg_path);
  success = file.open(QFile::ReadOnly | QFile::Text);
  if(not success)
  {
    QMessageBox::warning(parent->mainwindow, tr("Signaltypes"), tr("Cannot read file %1:\n%2.").arg(cfg_path).arg(file.errorString()));
    default_types();
    return NULL;
  }

  success = document.setContent(&file, true, &errorStr, &errorLine, &errorColumn);

  if(not success)
  {
    QMessageBox::information(parent->mainwindow,
                             tr("Signaltypes"),
                             tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
    default_types();
    file.close();
    return NULL;
  }
  file.close();

  QDomElement *root = new QDomElement(document.documentElement());
  if( root->tagName() != "Signaltypes" )
  {
    QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("%1 is not a Signaltypes.xml file.").arg(cfg_path) );
    default_types();
    delete root;
    return NULL;
  }

  else if( root->hasAttribute("version") && root->attribute("version") != "0.0.1" )
  {
    QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("%1 is not Signaltypes.xml version 0.0.1 file.").arg(cfg_path) );
    default_types();
    delete root;
    return NULL;
  }
  return root;
}



void Signaltypes::reload_types()
{
  bool exists;
  QDomElement *root;

  root = getRootElement();
  if(root == NULL) return;  // probably already threw the appropriate error.

  for(QDomElement new_type = root->firstChildElement("Signaltype");
      not new_type.isNull();
      new_type = new_type.nextSiblingElement("Signaltype"))
  {
    exists = false;
    if ( not new_type.hasAttribute("Name") )
    {
      QMessageBox::information(parent->mainwindow, tr("Signaltypes"), tr("%1 has no Name attribute.").arg(new_type.tagName()) );
      continue;
    }

    for(unsigned i=0; i<types.size(); i++)
    {
      if(types[i]->name == new_type.attribute("Name"))
      {
        exists = true;
        delete types[i];
        types[i] = new Signaltype(this, &new_type);
        break;
      }
    }
    if(not exists)
    {
      types.push_back( new Signaltype(this, &new_type) );
    }
  }
}



void Signaltypes::load_types()
{
  reset();

  QDomElement *root = getRootElement();
  if(root == NULL) return;  // probably already threw the appropriate error.

  for(QDomElement child = root->firstChildElement("Signaltype");
      not child.isNull();
      child = child.nextSiblingElement("Signaltype"))
  {
    types.push_back( new Signaltype(this, &child) );
  }
  delete root;
}



void Signaltypes::save_types()
{
  char cfg_path[MAX_PATH_LENGTH];
  bool success;
  QDomDocument document;

  // Creating xml-document part
  QDomElement root = document.createElement("Signaltypes");
  root.setAttribute("version", "0.0.1");

  for(unsigned i=0; i<types.size(); i++)
  {
    QDomNode type = document.importNode( types[i]->toQDom(), true );  // true : Import the whole tree.
    root.appendChild(type);
  }


  // Writing-to-file part
  configpath(cfg_path, "Signaltypes.xml");
  QFile file(cfg_path);
  success = file.open( QIODevice::WriteOnly | QIODevice::Text );
  if(not success)
  {
    QMessageBox::warning(parent->mainwindow, tr("Signaltypes"), tr("Cannot write file %1:\n%2.").arg(cfg_path).arg(file.errorString()));
    default_types();
    return;
  }

  QTextStream stream( &file );

  stream << document.toString();

  file.close();
}



void Signaltypes::default_types()
{
  Signaltype *signaltype;

  signaltype = new Signaltype(this, "EEG");
  signaltype->addAction("Spindle");
  types.push_back(signaltype);

  signaltype = new Signaltype(this, "EMG");
  signaltype->addAction("Bruxism");
  types.push_back(signaltype);

  signaltype = new Signaltype(this, "EOG");
  signaltype->addAction("Rapid eye movement");
  types.push_back(signaltype);

  signaltype = new Signaltype(this, "EKG");
  signaltype->addAction("Ectopic beat");
  types.push_back(signaltype);
}



void Signaltypes::exec(int type_number, int x, int y)
{
  QPoint point(x, y);
  types[type_number]->exec( point );
}


void Signaltypes::registerSignaltypes(bool ask)   // ask=true
{
  struct signalcompblock *signalcomp;
  struct edfparamblock *edfparam;
  unsigned signalcomps = parent->mainwindow->signalcomps;


  for(unsigned i=0; i<signalcomps; i++)
  {
    signalcomp = parent->mainwindow->signalcomp[i];   // current signalcomp
    if(signalcomp->type < 1)        // -1 : type default value.  0 : Unspecified
    {
      edfparam   = signalcomp->edfhdr->edfparam;    // edfparam of signal composition.

      signalcomp->type = get_type_from_label(edfparam[signalcomp->edfsignal[0]].label);

      for(int j=1; j<signalcomp->num_of_signals; j++) // inconsistencies in type, e.g. units, are only problematic with signal combinations (num_of_signals>1).
      {
        if( signalcomp->type != get_type_from_label(edfparam[signalcomp->edfsignal[j]].label) )
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                    "In Signaltypes::registerSignaltypes() : Your signal composition consists of incompatible types.  Setting to Unspecified.");
          messagewindow.exec();
          signalcomp->type = 0;
        }
      }
      //if(signalcomp->type == 0) ask = true;
    }
  }
  if(ask)
  {
    ask_for_types();    // if not all types were determined, fire up the user interface.
  }
}



int Signaltypes::get_type_from_label(char *label)
{
  QString qlabel = QString(label);

  for(unsigned i=0; i<types.size(); i++)
  {
    if( types[i]->isinstance(&qlabel) ) return (int)i;
  }
  return 0; // Unspecified.
}



void Signaltypes::ask_for_types()
{
  UI_Signaltype_dialog win(this);
}





