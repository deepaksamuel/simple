#include "SimpleRunAction.h"


#include "G4CsvAnalysisManager.hh"
//#include "G4RootAnalysisManager.hh"
#include "SimpleDetectorConstruction.h"
#include <G4coutDestination.hh>
#include <g4root.hh>



SimpleRunAction::SimpleRunAction() : G4UserRunAction()
{

    G4RunManager::GetRunManager()->SetPrintProgress(10000);
}


SimpleRunAction::~SimpleRunAction()
{}

//void SimpleRunAction::setRecordParameters(bool rec_pos, bool rec_time, bool rec_energy, bool rec_mom, bool rec_process)
//{
////    save_position = rec_pos;
////    save_time = rec_time;
////    save_energy = rec_energy;
////    save_momentum = rec_mom;
////    save_process = rec_process;

//}
void SimpleRunAction::BeginOfRunAction(const G4Run*)
{
   //setRecordParameters();
    const SimpleDetectorConstruction* det
            = static_cast<const SimpleDetectorConstruction*>   (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->SetNtupleMerging(true); // very important
//    enum rec{eid, pid, det_id, is_primary,
//             posx, posy, posz,
//             del_posx, del_posy, del_posz,
//             ni_edep, tot_edep, tot_energy, tot_ke,
//             momx, momy, momz,
//             px,py,pz,
//             del_t, globTime, propTime, locTime,
//             process};
    // Open an output file
    man->OpenFile("data/temp-det-output");
    man->CreateNtuple("simple","out_data");

    man->CreateNtupleDColumn("eid"); // column Id = 0
    man->CreateNtupleDColumn("pid"); // column Id = 1
    man->CreateNtupleSColumn("detectorId"); // column Id = 2
    man->CreateNtupleDColumn("is_primary"); // column Id = 3

    if(det->recordPosition()){
        man->CreateNtupleDColumn("posx"); // column Id = 4
        man->CreateNtupleDColumn("posy"); // column Id = 5
        man->CreateNtupleDColumn("posz"); // column Id = 6
        man->CreateNtupleDColumn("del_posx"); // column Id = 7
        man->CreateNtupleDColumn("del_posy"); // column Id = 8
        man->CreateNtupleDColumn("del_posz"); // column Id = 9
    }
    if(det->recordTime()){
        man->CreateNtupleDColumn("del_t"); // column Id = 10
        man->CreateNtupleDColumn("globTime"); // column Id = 11
        man->CreateNtupleDColumn("propTime"); // column Id = 12
        man->CreateNtupleDColumn("localTime"); // column Id = 13
    }

    if(det->recordEnergy())
    {
        man->CreateNtupleDColumn("ni_edep"); // column Id = 14
        man->CreateNtupleDColumn("tot_edep"); // column Id = 15
        man->CreateNtupleDColumn("tot_E"); // column Id =16
        man->CreateNtupleDColumn("tot_KE"); // column Id=17
    }
    if(det->recordMomentum())
    {
        man->CreateNtupleDColumn("momx"); // column Id = 18
        man->CreateNtupleDColumn("momy"); // column Id = 19
        man->CreateNtupleDColumn("momz"); // column Id = 20
        man->CreateNtupleDColumn("px"); // column Id = 21
        man->CreateNtupleDColumn("py"); // column Id = 22
        man->CreateNtupleDColumn("pz"); // column Id = 23
    }

    if(det->recordProcess())
    {
        man->CreateNtupleSColumn("process"); // columnId =24
    }
    man->FinishNtuple();



G4cout<<"Begin run...\n";

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SimpleRunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
    G4cout<<"End of run...\n";

}
