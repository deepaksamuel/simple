#ifndef SIMPLERUNACTION_H
#define SIMPLERUNACTION_H
// File modified by Deepak Samuel on 25 Sep 2019

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4root.hh"
//#include "g4csv.hh"
#include "G4UserRunAction.hh"

#include "G4CsvAnalysisManager.hh"

class SimpleRunAction : public G4UserRunAction
{
public:
    SimpleRunAction();
    virtual ~SimpleRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
   // void setRecordParameters(bool rec_pos=true, bool rec_time=true, bool rec_energy=true, bool rec_mom=true, bool rec_process=true);



    G4RootAnalysisManager* rootanalysisManager;
    G4CsvAnalysisManager* csvanalysisManager;



    bool save_position, save_energy, save_momentum, save_time, save_process;
    int pidFilter;
};
#endif // SIMPLERUNACTION_H
