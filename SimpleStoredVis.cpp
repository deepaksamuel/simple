// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleStoredVis.h"


#include "G4VisFeaturesOfOpenGL.hh"
#include "G4VSceneHandler.hh"
#include "G4OpenGLSceneHandler.hh"
#include "G4OpenGLViewer.hh"
#include "G4OpenGLStoredQt.hh"
#include "G4OpenGLStoredQtSceneHandler.hh"
#include "SimpleOpenGLStoredQtScenehandler.h"
#include "G4OpenGLStoredQtViewer.hh"
#include "G4OpenGLViewerMessenger.hh"
#include "SimpleOpenGLStoredQtViewer.h"


// copied from G4OpenGLStoredQt.cc
SimpleStoredVis::SimpleStoredVis():
  G4OpenGLQt ("OpenGLStoredQt",
              "OGLSQt",
              G4VisFeaturesOfOpenGLSQt (),
              G4VGraphicsSystem::threeD)
{

  G4OpenGLViewerMessenger::GetInstance();
}

G4VSceneHandler* SimpleStoredVis::CreateSceneHandler
(const G4String& name) {
  G4VSceneHandler* pScene = new SimpleOpenGLStoredQtSceneHandler (*this, name);
  return    pScene;
}

G4VViewer* SimpleStoredVis::CreateViewer(G4VSceneHandler& scene, const G4String& name) {
    qDebug()<<"Creating viewer...";
    G4VViewer* pView = 0;
    pView = new SimpleOpenGLStoredQtViewer
      ((SimpleOpenGLStoredQtSceneHandler&) scene, name);
    if (pView) {
      if (pView -> GetViewId () < 0) {
        G4cerr << "G4OpenGLStoredQt::CreateViewer: error flagged by negative"
      " view id in G4OpenGLStoredQtViewer creation."
      "\n Destroying view and returning null pointer."
         << G4endl;
        delete pView;
        pView = 0;
      }
    } else {
      G4cerr << "G4OpenGLStoredQt::CreateViewer: null pointer on"
        " new G4OpenGLStoredQtViewer." << G4endl;
    }
    return pView;
}
