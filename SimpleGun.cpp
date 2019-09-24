#include "SimpleGun.h"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4GeneralParticleSource.hh"
#include <QDebug>
#include <G4Run.hh>

int SimpleGun::nevent=0;

SimpleGun::SimpleGun() : QObject(new QObject), G4VUserPrimaryGeneratorAction()
{

    G4int n_particle = 1;
    fParticleGun  = new G4GeneralParticleSource();
    G4String particleName;
    //   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    //   G4ParticleDefinition* particle = particleTable->FindParticle(particleName="e-");
    //   fParticleGun->SetParticleDefinition(particle);
}



SimpleGun::~SimpleGun()
{

    delete fParticleGun;
}

void SimpleGun::GeneratePrimaries(G4Event* anEvent)
{

    //if(anEvent->GetEventID()==0){
    //const G4Run *thisRun = static_cast<const G4Run*>   (G4RunManager::GetRunManager()->GetCurrentRun());
    //  nevents =  ;
    // }

    fParticleGun->GeneratePrimaryVertex(anEvent);


    //nextEvent(100*(anEvent->GetEventID()+1)/thisRun->GetNumberOfEventToBeProcessed());
    //nextEvent(nevent++);

    if(nevent%1000==0){
        //qDebug()<<nevent<<" processed..."<<endl;
        G4cout<<nevent<<" processed";
    }


}

