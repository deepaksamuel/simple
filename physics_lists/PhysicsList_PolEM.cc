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
/// \file polarisation/Pol01/src/PhysicsList_PolEM.cc
/// \brief Implementation of the PhysicsList_PolEM class
//
// 
// $Id: PhysicsList_PolEM.cc 100257 2016-10-17 08:00:06Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList_PolEM.hh"
//#include "PhysicsList_PolEMMessenger.hh"
 
#include "G4EmStandardPhysics.hh"
#include "PhysListEmPolarized.hh"

#include "G4EmParameters.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList_PolEM::PhysicsList_PolEM() 
: G4VModularPhysicsList(),
  fEmPhysicsList_PolEM(0), fEmName("polarized"), fMessenger(0)
{
  //fMessenger = new PhysicsList_PolEMMessenger(this);

  G4EmParameters::Instance();

  SetVerboseLevel(1);

  fEmPhysicsList_PolEM = new PhysListEmPolarized();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList_PolEM::~PhysicsList_PolEM()
{
  //delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_PolEM::ConstructParticle()
{
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4EmProcessOptions.hh"

void PhysicsList_PolEM::ConstructProcess()
{
  // Transportation
  //
  AddTransportation();

  // Electromagnetic physics list
  //
  fEmPhysicsList_PolEM->ConstructProcess();
    
  // step limitation (as a full process)
  //  
  AddStepMax();      
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_PolEM::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>0) {
   // G4cout << "PhysicsList_PolEM::AddPhysicsList_PolEM: <" << name << ">" << G4endl;
  }
  
  if (name == fEmName) return;

  if (name == "standard") {

    fEmName = name;
    delete fEmPhysicsList_PolEM;
    fEmPhysicsList_PolEM = new G4EmStandardPhysics();
            
  } else if (name == "polarized") {

    fEmName = name;
    delete fEmPhysicsList_PolEM;
    fEmPhysicsList_PolEM = new PhysListEmPolarized();

  } else {
   // G4cout << "PhysicsList_PolEM::AddPhysicsList_PolEM: <" << name << ">"
    //       << " is not defined"
    //       << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ProcessManager.hh"
#include "StepMax.hh"

void PhysicsList_PolEM::AddStepMax()
{
  // Step limitation seen as a process
  StepMax* stepMaxProcess = new StepMax();

  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while ((*particleIterator)()){
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (stepMaxProcess->IsApplicable(*particle) && pmanager)
          pmanager ->AddDiscreteProcess(stepMaxProcess);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
