#ifndef SIMPLESTEPPINGACTION_H
#define SIMPLESTEPPINGACTION_H
// File modified by Deepak Samuel on 25 Sep 2019
#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include  <QList>
#include "G4LogicalVolumeModel.hh"

#include "g4root.hh"
#include "G4CsvAnalysisManager.hh"
#include "G4RootAnalysisManager.hh"

class SimpleSteppingAction : public G4UserSteppingAction
{
public:
    SimpleSteppingAction();

    virtual ~SimpleSteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);
    G4RootAnalysisManager* analysisManager;
    G4CsvAnalysisManager* csvanalysisManager;

    ulong eid;
    G4LogicalVolume* current_volume;
    G4String current_volume_name;
    //void setListofSensitiveDetector(QList <G4LogicalVolume*> sensitive_det_list){SD=sensitive_det_list;}
};


#endif // SIMPLESTEPPINGACTION_H
