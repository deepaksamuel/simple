//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
// 
// John Allison  5th April 2001
// A template for a simplest possible graphics driver.
//?? Lines or sections marked like this require specialisation for your driver.

#include "SimpleVis.h"
#include "G4OpenGLImmediateSceneHandler.hh"
#include "G4VisFeaturesOfOpenGL.hh"
#include "G4OpenGLViewerMessenger.hh"
#include "G4UImanager.hh"
#include "G4UIbatch.hh"

#include "G4VisFeaturesOfOpenGL.hh"
#include "G4VSceneHandler.hh"
#include "G4OpenGLSceneHandler.hh"
#include "G4OpenGLViewer.hh"
#include "G4OpenGLImmediateQt.hh"
#include "G4OpenGLViewerMessenger.hh"
#include "G4OpenGLImmediateSceneHandler.hh"
#include "G4OpenGLImmediateQtViewer.hh"
#include "G4OpenGLStoredSceneHandler.hh"
#include "G4OpenGLStoredQtViewer.hh"
#include "SimpleOpenGLImmediateQtviewer.h"

SimpleVis::SimpleVis ():
    G4OpenGLQt ("OpenGLImmediateQt",
                "OGLIQt",
                G4VisFeaturesOfOpenGLIQt (),
                G4VGraphicsSystem::threeD)
{
    //   G4cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SimpleVIs+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    G4OpenGLViewerMessenger::GetInstance();
}

G4VSceneHandler* SimpleVis::CreateSceneHandler
(const G4String& name) {
    G4VSceneHandler* pScene = new G4OpenGLImmediateSceneHandler (*this, name);
    //  G4cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Creating scene handler+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    return    pScene;
}

G4VViewer* SimpleVis::CreateViewer
(G4VSceneHandler& scene, const G4String& name) {

    // Important lines DS
    //G4cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Creating viewer+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    SimpleOpenGLImmediateQtViewer* sViewer =new SimpleOpenGLImmediateQtViewer((G4OpenGLImmediateSceneHandler&) scene, name);
    G4VViewer* pView = sViewer;
    glWidget = sViewer;



    //  SimpleViewer* sViewer =new SimpleViewer((G4OpenGLImmediateSceneHandler&) scene, name);
    //  G4VViewer* pView = sViewer;
    //  glWidget = sViewer;

    //  pView = new SimpleViewer
    //    ((G4OpenGLImmediateSceneHandler&) scene, name);
    if (pView) {

        if (pView -> GetViewId () < 0) {
            G4cerr << "SimpleVis::CreateViewer: error flagged by negative"
                      " view id in SimpleVisViewer creation."
                      "\n Destroying view and returning null pointer."
                   << G4endl;
            delete pView;
            pView = 0;

        }
    }
    else {
        G4cerr << "SimpleVis::CreateViewer: null pointer on"
                  " new SimpleVisViewer." << G4endl;
    }
    return pView;
}


