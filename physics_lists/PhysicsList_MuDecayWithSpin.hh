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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef PhysicsListMuDecayWithSpin_h
#define PhysicsListMuDecayWithSpin_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"

class G4VPhysicsConstructor;
class F04PhysicsListMessenger;

class F04StepMax;

class PhysicsList_MuDecayWithSpin: public G4VModularPhysicsList
{
public:

    PhysicsList_MuDecayWithSpin(G4String);
    virtual ~PhysicsList_MuDecayWithSpin();

    void ConstructParticle();
    
    void SetCuts();
    void SetCutForGamma(G4double);
    void SetCutForElectron(G4double);
    void SetCutForPositron(G4double);

    void SetAnalyticSR(G4bool val){SRType = val;}; //Hiromi Add 2009/08/12

    void SetStepMax(G4double);
    F04StepMax* GetStepMaxProcess();
    void AddStepMax();
    
    /// Add physics to the Physics List    
    void AddPhysicsList(const G4String& name);

    /// Remove specific EM physics from EM physics list.
    void RemoveFromEMPhysicsList(const G4String&);

    /// Remove specific Hadron physics from Hadron physics list.
    void RemoveFromHadronPhysicsList(const G4String&);

    /// Make sure that the EM physics list is empty.
    void ClearEMPhysics();

    /// Make sure that the hadron physics list is empty.
    void ClearHadronPhysics();

    void ConstructProcess();
    void List();
  
private:

    typedef std::vector<G4VPhysicsConstructor*>  PhysicsListVector;

    void SetBuilderList0();
    void SetBuilderList1();
    void SetBuilderList2();
    void SetBuilderList3();
    void SetBuilderList4();
    void SetBuilderList5();
    void SetBuilderList6();

    G4double fCutForGamma;
    G4double fCutForElectron;
    G4double fCutForPositron;

    G4VPhysicsConstructor*  decayPhysicsList;

    G4VPhysicsConstructor*  emPhysicsList;
    G4String emName;//Add hiromi 2009/08/12

    PhysicsListVector* fEMPhysics;
    PhysicsListVector* fHadronPhysics;

    G4double MaxChargedStep;
    F04StepMax* stepMaxProcess;
    
    F04PhysicsListMessenger* fMessenger;

    G4bool fDump;
    G4bool SRType;
};
#endif
