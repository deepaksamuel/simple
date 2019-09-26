#ifndef SIMPLESTOREDVIS_H
#define SIMPLESTOREDVIS_H
// File modified by Deepak Samuel on 25 Sep 2019

#include "G4OpenGLQt.hh"
#include <QGLWidget>

class SimpleStoredVis: public G4OpenGLQt {
public:
  SimpleStoredVis ();
  G4VSceneHandler* CreateSceneHandler (const G4String& name = "");
  G4VViewer*  CreateViewer  (G4VSceneHandler&, const G4String& name = "");

};



#endif // SIMPLESTOREDVIS_H
