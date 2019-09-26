// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleSteppingAction.h"
#include "SimpleDetectorConstruction.h"
#include <G4RunManager.hh>
#include <SimpleRunAction.h>
#include <g4root.hh>
#include <G4CsvAnalysisManager.hh>
#include <QDebug>

SimpleSteppingAction::SimpleSteppingAction() : G4UserSteppingAction ()
{

}

SimpleSteppingAction::~SimpleSteppingAction()
{

}

void SimpleSteppingAction::UserSteppingAction(const G4Step* step)
{
    const SimpleDetectorConstruction* detectorConstruction
            = static_cast<const SimpleDetectorConstruction*>   (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    if(!detectorConstruction->hasRecordingObjects()) return;

    // get volume of the current step
    // current_volume_name  = step->GetPreStepPoint()->GetTouchableHandle() ->GetVolume()->GetLogicalVolume()->GetName();
    eid = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    detectorConstruction->appendData(step,eid);
    //    int columnId=0;
    //    if(current_volume_name=="Box-001")
    //    {
    //        eid = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    //        G4AnalysisManager* man = G4AnalysisManager::Instance();
    //        man->FillNtupleDColumn(columnId++, eid);
    //        man->FillNtupleDColumn(columnId++, step->GetTrack()->GetDefinition()->GetPDGEncoding());
    //        man->FillNtupleSColumn(columnId++, "Box-001");
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetParentID());

    //        man->FillNtupleDColumn(columnId++,step->GetPreStepPoint()->GetPosition().x());
    //        man->FillNtupleDColumn(columnId++,step->GetPreStepPoint()->GetPosition().y());
    //        man->FillNtupleDColumn(columnId++,step->GetPreStepPoint()->GetPosition().z());
    //        man->FillNtupleDColumn(columnId++,step->GetDeltaPosition().x());
    //        man->FillNtupleDColumn(columnId++,step->GetDeltaPosition().y());
    //        man->FillNtupleDColumn(columnId++,step->GetDeltaPosition().z());

    //        man->FillNtupleDColumn(columnId++,step->GetDeltaTime());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetGlobalTime());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetProperTime());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetLocalTime());

    //        man->FillNtupleDColumn(columnId++,step->GetNonIonizingEnergyDeposit());
    //        man->FillNtupleDColumn(columnId++,step->GetTotalEnergyDeposit());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetTotalEnergy());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetKineticEnergy());

    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentum().x());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentum().y());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentum().z());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentumDirection().x());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentumDirection().y());
    //        man->FillNtupleDColumn(columnId++,step->GetTrack()->GetMomentumDirection().z());

    //        const G4VProcess* proc = step->GetPreStepPoint()->GetProcessDefinedStep();

    //        if(proc!=NULL)
    //            man->FillNtupleSColumn(columnId++,proc->GetProcessName());
    //        else
    //            man->FillNtupleSColumn(columnId++,"Unknown");

    //         man->AddNtupleRow();
    //    }

}
