#include "PhysicsList_MuDecayWithSpin.hh"
#include "F04PhysicsListMessenger.hh"

#include "F04ExtraPhysics.hh"
//#include "F04OpticalPhysics.hh"

#include "G4DecayPhysics.hh"

//#include "PhysListEmStandard.hh"
//#include "PhysListEmStandardSS.hh"
//#include "PhysListEmStandardNR.hh"
//#include "PhysListEmLivermore.hh"
//#include "PhysListEmPenelope.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "MyG4EmStandardPhysics_option4.hh"

//Hiromi Add 2009/08/12
#include "PhysListEmStandard.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"

#include "G4HadronElasticPhysics.hh"
#include "G4HadronDElasticPhysics.hh"
//#include "G4HadronQElasticPhysics.hh" // obsolete
#include "G4HadronHElasticPhysics.hh"

#include "G4NeutronTrackingCut.hh"
//#include "G4QStoppingPhysics.hh" // obsolete
#include "G4StoppingPhysics.hh"
//#include "G4LHEPStoppingPhysics.hh" // obsolete
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonPhysics.hh"

//#include "HadronPhysicsFTFC.hh"
//#include "HadronPhysicsFTFP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTF_BIC.hh"

// obsolete
//#include "G4HadronPhysicsLHEP.hh"
//#include "HadronPhysicsLHEP_BERT.hh"
//#include "G4HadronPhysicsLHEP_EMV.hh" 
//#include "G4HadronPhysicsLHEP_PRECO_HP.hh"

#include "G4HadronInelasticQBBC.hh"

// obsolete
//#include "HadronPhysicsQGSC.hh"
//#include "G4HadronPhysicsQGSC_BERT.hh" 
//#include "HadronPhysicsQGSC_EFLOW.hh"

//#include "G4HadronPhysicsQGSP.hh" // obsolete
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

//#include "G4HadronInelasticQLHEP.hh"

#include "G4IonPhysics.hh"

#include "G4LossTableManager.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "F04StepMax.hh"

#include "G4ProcessTable.hh"

#include "G4PionDecayMakeSpin.hh"
#include "G4DecayWithSpin.hh"

#include "G4DecayTable.hh"
#include "G4MuonDecayChannelWithSpin.hh"
#include "G4MuonRadiativeDecayChannelWithSpin.hh"


#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

//#include "G4Pythia6Decayer.hh"
//#include "Pythia6Decayer.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4LossTableManager.hh"

PhysicsList_MuDecayWithSpin::PhysicsList_MuDecayWithSpin(G4String physicsList) : G4VModularPhysicsList() 
{
    G4LossTableManager::Instance();

    //defaultCutValue  = 1.*mm;
    //defaultCutValue  = 300.*um;//houiken 1007
    defaultCutValue  = 0.001*mm;// by ku
    //defaultCutValue  = 0.1*mm;// by ku
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;

    fDump            = false;

    fMessenger = new F04PhysicsListMessenger(this);

    SetVerboseLevel(1);

    fEMPhysics     = new PhysicsListVector();
    fHadronPhysics = new PhysicsListVector();

    // Particles
    decayPhysicsList = new G4DecayPhysics("decays");

    // Add External Decayer to the G4Decay

    // Create Geant4 external decayer
//    Pythia6Decayer* pythia6Decayer = new Pythia6Decayer();
//    pythia6Decayer->Init();

//    G4Pythia6Decayer* extDecayer = new G4Pythia6Decayer(pythia6Decayer);
//    extDecayer->SetVerboseLevel(2);

//    G4DecayPhysics* decayPhysics = (G4DecayPhysics*)(decayPhysicsList);
//    if(decayPhysics)decayPhysics->GetDecayProcess()->SetExtDecayer(extDecayer);

    // EM physics
    //AddPhysicsList("emstandard");//motomoto atta hitomi1123
    //AddPhysicsList("standard");//changed by ku
    AddPhysicsList("emstandard_opt4");
    //Hiromi Add SyncRad 2009/08/13
    /*
      emName = G4String("standard");
      emPhysicsList = new PhysListEmStandard(emName);
    */
    //Hiromi Add SyncRad 2009/08/13 END

    // Set the default hadronic physics.
    AddPhysicsList(physicsList);

    stepMaxProcess = new F04StepMax();
}

PhysicsList_MuDecayWithSpin::~PhysicsList_MuDecayWithSpin()
{
    delete fMessenger;
    delete decayPhysicsList;

    ClearEMPhysics();
    ClearHadronPhysics();

    delete fEMPhysics;
    delete fHadronPhysics;

    delete stepMaxProcess;
}

void PhysicsList_MuDecayWithSpin::ClearEMPhysics()
{
    for (PhysicsListVector::iterator p  = fEMPhysics->begin();
                                     p != fEMPhysics->end(); ++p) {
        delete (*p);
    }
    fEMPhysics->clear();
}

void PhysicsList_MuDecayWithSpin::ClearHadronPhysics()
{
    for (PhysicsListVector::iterator p  = fHadronPhysics->begin();
                                     p != fHadronPhysics->end(); ++p) {
        delete (*p);
    }
    fHadronPhysics->clear();
}

void PhysicsList_MuDecayWithSpin::ConstructParticle()
{
    decayPhysicsList->ConstructParticle();

    G4DecayTable* MuonPlusDecayTable = new G4DecayTable();
/*
    MuonPlusDecayTable -> Insert(new
                           G4MuonDecayChannelWithSpin("mu+",0.986));
    MuonPlusDecayTable -> Insert(new
                           G4MuonRadiativeDecayChannelWithSpin("mu+",0.014));
    G4MuonPlus::MuonPlusDefinition() -> SetDecayTable(MuonPlusDecayTable);
*/
    MuonPlusDecayTable -> Insert(new
                           G4MuonDecayChannelWithSpin("mu+",1.000));//Hiromi
    MuonPlusDecayTable -> Insert(new
                           G4MuonRadiativeDecayChannelWithSpin("mu+",0.000));
    G4MuonPlus::MuonPlusDefinition() -> SetDecayTable(MuonPlusDecayTable);

    G4DecayTable* MuonMinusDecayTable = new G4DecayTable();
    MuonMinusDecayTable -> Insert(new
                            G4MuonDecayChannelWithSpin("mu-",0.986));
    MuonMinusDecayTable -> Insert(new
                            G4MuonRadiativeDecayChannelWithSpin("mu-",0.014));
    G4MuonMinus::MuonMinusDefinition() -> SetDecayTable(MuonMinusDecayTable);
}

void PhysicsList_MuDecayWithSpin::ConstructProcess()
{
    AddTransportation();

    for (PhysicsListVector::iterator p  = fEMPhysics->begin();
                                     p != fEMPhysics->end(); ++p) {
        (*p)->ConstructProcess();
    }

    decayPhysicsList->ConstructProcess();

    G4DecayWithSpin* decayWithSpin = new G4DecayWithSpin();

    G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();

    G4VProcess* decay;
    decay = processTable->FindProcess("Decay",G4MuonPlus::MuonPlus());

    G4ProcessManager* fManager;
    fManager = G4MuonPlus::MuonPlus()->GetProcessManager();

    if (fManager) {
      if (decay) fManager->RemoveProcess(decay);
      fManager->AddProcess(decayWithSpin);
      // set ordering for PostStepDoIt and AtRestDoIt
      fManager ->SetProcessOrdering(decayWithSpin, idxPostStep);
      fManager ->SetProcessOrdering(decayWithSpin, idxAtRest);
    }

    decay = processTable->FindProcess("Decay",G4MuonMinus::MuonMinus());

    fManager = G4MuonMinus::MuonMinus()->GetProcessManager();

    if (fManager) {
      if (decay) fManager->RemoveProcess(decay);
      fManager->AddProcess(decayWithSpin);
      // set ordering for PostStepDoIt and AtRestDoIt
      fManager ->SetProcessOrdering(decayWithSpin, idxPostStep);
      fManager ->SetProcessOrdering(decayWithSpin, idxAtRest);
    }


    G4PionDecayMakeSpin* poldecay = new G4PionDecayMakeSpin();

    decay = processTable->FindProcess("Decay",G4PionPlus::PionPlus());

    fManager = G4PionPlus::PionPlus()->GetProcessManager();

    if (fManager) {
      if (decay) fManager->RemoveProcess(decay);
      fManager->AddProcess(poldecay);
      // set ordering for PostStepDoIt and AtRestDoIt
      fManager ->SetProcessOrdering(poldecay, idxPostStep);
      fManager ->SetProcessOrdering(poldecay, idxAtRest);
    }

    decay = processTable->FindProcess("Decay",G4PionMinus::PionMinus());

    fManager = G4PionMinus::PionMinus()->GetProcessManager();

    if (fManager) {
      if (decay) fManager->RemoveProcess(decay);
      fManager->AddProcess(poldecay);
      // set ordering for PostStepDoIt and AtRestDoIt
      fManager ->SetProcessOrdering(poldecay, idxPostStep);
      fManager ->SetProcessOrdering(poldecay, idxAtRest);
    }

    for (PhysicsListVector::iterator p  = fHadronPhysics->begin();
                                     p != fHadronPhysics->end();++p) {
        (*p)->ConstructProcess();
    }

    AddStepMax();
}

void PhysicsList_MuDecayWithSpin::RemoveFromEMPhysicsList(const G4String& name)
{
    G4bool success = false;
    for (PhysicsListVector::iterator p  = fEMPhysics->begin();
                                     p != fEMPhysics->end(); ++p) {
        G4VPhysicsConstructor* e = (*p);
        if (e->GetPhysicsName() == name) {
           fEMPhysics->erase(p);
           success = true;
           break;
        }
    }
    if (!success) {
       std::ostringstream message;
       message << "PhysicsList::RemoveFromEMPhysicsList "<< name << "not found";
       //G4Exception(message.str().c_str());  //changed 2012.11.13
       exit(0);
    }
}

void PhysicsList_MuDecayWithSpin::RemoveFromHadronPhysicsList(const G4String& name)
{
    G4bool success = false;
    for (PhysicsListVector::iterator p  = fHadronPhysics->begin();
                                     p != fHadronPhysics->end(); ++p) {
        G4VPhysicsConstructor* e = (*p);
        if (e->GetPhysicsName() == name) {
           fHadronPhysics->erase(p);
           success = true;
           break;
        }
    }
    if (!success) {
       G4String message("F04PhysicsList::RemoveFromHadronPhysicsList \"");
       message += name;
       message += " not found \"";
       //G4Exception(message); //changed 2012.11.13
       exit(0);
    }
}

void PhysicsList_MuDecayWithSpin::AddPhysicsList(const G4String& name)
{
    if (verboseLevel>1) {
        G4cout << "F04PhysicsList::AddPhysicsList: <" << name 
               << ">" << G4endl;
    }

    if (name == "standard") {

       ClearEMPhysics();
       fEMPhysics->push_back(new PhysListEmStandard(name)); //ku
       //fEMPhysics->push_back(new F04ExtraPhysics()); //ku
       //fEMPhysics->push_back(new F04OpticalPhysics()); //ku

    } else if (name == "emstandard") {

       ClearEMPhysics();
       fEMPhysics->push_back(new G4EmStandardPhysics());
       fEMPhysics->push_back(new F04ExtraPhysics());

//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "emstandard_opt1") {

       ClearEMPhysics();
//       fEMPhysics->push_back(new G4EmStandardPhysics_option1());
       fEMPhysics->push_back(new F04ExtraPhysics());//Sep2010hiromi
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "emstandard_opt2") {

       ClearEMPhysics();
       fEMPhysics->push_back(new G4EmStandardPhysics_option2());
       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "emstandard_opt3") {

       ClearEMPhysics();
       fEMPhysics->push_back(new G4EmStandardPhysics_option3());
       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "emstandard_opt4") {
      
      ClearEMPhysics();
      fEMPhysics->push_back(new MyG4EmStandardPhysics_option4());
      fEMPhysics->push_back(new F04ExtraPhysics());
      
    } else if (name == "standardSS") {

       ClearEMPhysics();
//       fEMPhysics->push_back(new PhysListEmStandardSS(name));
//       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "standardNR") {

       ClearEMPhysics();
//       fEMPhysics->push_back(new PhysListEmStandardNR(name));
//       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "livermore") {

       ClearEMPhysics();
//       fEMPhysics->push_back(new PhysListEmLivermore());
//       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "penelope") {

       ClearEMPhysics();
//       fEMPhysics->push_back(new PhysListEmPenelope());
//       fEMPhysics->push_back(new F04ExtraPhysics());
//       fEMPhysics->push_back(new F04OpticalPhysics());

    } else if (name == "FTFC") {

       SetBuilderList1();
       //fHadronPhysics->push_back(new HadronPhysicsFTFC("hadron",true));
       //fDump = true;

    } else if (name == "FTFP") {

       SetBuilderList1();
       //fHadronPhysics->push_back(new HadronPhysicsFTFP("hadron",true));
       //fDump = true;

    } else if (name == "FTFP_BERT") {

       SetBuilderList1();
       //fHadronPhysics->push_back(new HadronPhysicsFTFP_BERT("hadron",true)); // obsolete
       fDump = true;

    } else if (name == "FTFP_EMV") {

       AddPhysicsList("emstandard_opt1");
       AddPhysicsList("FTFP");

    } else if (name == "FTF_BIC") {

       SetBuilderList0();
       //fHadronPhysics->push_back(new HadronPhysicsFTF_BIC("hadron",true)); // obsolete
       fDump = true;

    }
    /* obsolete
       else if (name == "LHEP") {

       SetBuilderList2();
       //fHadronPhysics->push_back(new HadronPhysicsLHEP("hadron")); // obsolete
       fDump = true;
 
    } else if (name == "LHEP_BERT") {

       SetBuilderList3();
       //fHadronPhysics->push_back(new HadronPhysicsLHEP_BERT("hadron"));
       //fDump = true;

    } else if (name == "LHEP_EMV") {

       AddPhysicsList("emstandard_opt1");
       SetBuilderList3();
       fHadronPhysics->push_back(new HadronPhysicsLHEP_EMV("hadron"));
       fDump = true;

    } else if (name == "LHEP_PRECO_HP") {

       SetBuilderList3(true);
       //fHadronPhysics->push_back(new HadronPhysicsLHEP_PRECO_HP("hadron"));
       //fDump = true;

       }*/
    else if (name == "QBBC") {

       SetBuilderList0();
       fHadronPhysics->push_back(new G4HadronInelasticQBBC("QBBC",
                                                          verboseLevel,
                                                          false,false,
                                                          false,false,true));

    } else if (name == "QBBC_DEL") {

       SetBuilderList5();
       fHadronPhysics->push_back(new G4HadronInelasticQBBC("QBBC",
                                                          verboseLevel,
                                                          false,false,
                                                          false,false,true));

    } else if (name == "QBBC_HEL") {

       SetBuilderList6();
       fHadronPhysics->push_back(new G4HadronInelasticQBBC("QBBC",
                                                          verboseLevel,
                                                          false,false,
                                                          false,false,true));

    } else if (name == "QBBC_HP") {

       SetBuilderList0();
       fHadronPhysics->push_back(new G4HadronInelasticQBBC("QBBC",
                                                          verboseLevel,
                                                          false,false,
                                                          false,true,true));

    } else if (name == "QGSC") {

       SetBuilderList4();
       //fHadronPhysics->push_back(new HadronPhysicsQGSC("hadron",true));
       //fDump = true;

    }/* else if (name == "QGSC_BERT") {

       SetBuilderList4();
       fHadronPhysics->push_back(new HadronPhysicsQGSC_BERT("hadron",true));
       fDump = true;

    } else if (name == "QGSC_EFLOW") {

       SetBuilderList4();
       //fHadronPhysics->push_back(new HadronPhysicsQGSC_EFLOW("hadron",true));
       //fDump = true;

    } else if (name == "QGSC_EMV") {

       AddPhysicsList("emstandard_opt1");
       AddPhysicsList("QGSC");

       }*/
    /* else if (name == "QGSP") {

       SetBuilderList1();
       fHadronPhysics->push_back(new HadronPhysicsQGSP("hadron",true));
       fDump = true;

       }*/
    else if (name == "QGSP_BERT") {

       SetBuilderList1();
       fHadronPhysics->push_back(new G4HadronPhysicsQGSP_BERT("hadron",true));
       fDump = true;

    } else if (name == "QGSP_BERT_EMV") {

       AddPhysicsList("emstandard_opt1");
       AddPhysicsList("QGSP_BERT");

    } else if (name == "QGSP_BERT_HP") {

       SetBuilderList1();
       fHadronPhysics->push_back(new G4HadronPhysicsQGSP_BERT_HP("hadron",true));

    } else if (name == "QGSP_BIC") {

       SetBuilderList0();
       fHadronPhysics->push_back(new G4HadronPhysicsQGSP_BIC("hadron",true));
       fDump = true;

    } else if (name == "QGSP_BIC_HP") {

       SetBuilderList0();
       fHadronPhysics->push_back(new G4HadronPhysicsQGSP_BIC_HP("hadron",true));
       fDump = true;

    }/* else if (name == "QGSP_DIF") {

       SetBuilderList1();
       HadronPhysicsQGSP * hp = new HadronPhysicsQGSP("hadron");
       hp->SetQuasiElastic(true);
       hp->SetProjectileDiffraction(true);
       fHadronPhysics->push_back(hp);
       fDump = true;

       }*/
    else if (name == "QGSP_EMV") {

       AddPhysicsList("emstandard_opt1");
       AddPhysicsList("QGSP");
       fDump = true;

    } else if (name == "QGSP_EMX") {

       AddPhysicsList("emstandard_opt2");
       AddPhysicsList("QGSP");
       fDump = true;

    }/* else if (name == "QGSP_NQE") {

       SetBuilderList1();
       fHadronPhysics->push_back(new HadronPhysicsQGSP("hadron",false));
       fDump = true;

    } else if (name == "QGSP_QEL") {

       SetBuilderList4();
       fHadronPhysics->push_back(new HadronPhysicsQGSP("hadron",true));
       fDump = true;

       }*/ else {

       G4cout << "F04PhysicsList::AddPhysicsList: <" << name << ">"
              << " is not defined" << G4endl;
    }
}
void PhysicsList_MuDecayWithSpin::SetBuilderList0()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    fHadronPhysics->push_back(new G4HadronElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel));
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ionBIC"));
    fHadronPhysics->push_back(new G4NeutronTrackingCut("nTackingCut",
                                                     verboseLevel));
}

void PhysicsList_MuDecayWithSpin::SetBuilderList1()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    fHadronPhysics->push_back(new G4HadronElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel)); // obsolete
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ion"));
    fHadronPhysics->push_back(new G4NeutronTrackingCut("nTackingCut",
                                                     verboseLevel));
}

void PhysicsList_MuDecayWithSpin::SetBuilderList2()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    fHadronPhysics->push_back(new G4HadronElasticPhysics(verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ion"));
}

void PhysicsList_MuDecayWithSpin::SetBuilderList3()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    fHadronPhysics->push_back(new G4HadronElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel)); // obsolete
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ion"));
}

void PhysicsList_MuDecayWithSpin::SetBuilderList4()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    //fHadronPhysics->push_back(new G4HadronQElasticPhysics("elastic",verboseLevel)); // obsolete
    fHadronPhysics->push_back(new G4HadronElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel)); // obsolete
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ion"));
    fHadronPhysics->push_back(new G4NeutronTrackingCut("nTackingCut",
                                                     verboseLevel));
}

void PhysicsList_MuDecayWithSpin::SetBuilderList5()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    //fHadronPhysics->push_back(new G4HadronDElasticPhysics(verboseLevel,
    //                                       flagHP)); //changed 2012.11.13
    fHadronPhysics->push_back(new G4HadronDElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel)); // obsolete
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ionBIC"));
    fHadronPhysics->push_back(new G4NeutronTrackingCut("nTackingCut",
                                                     verboseLevel));
}


void PhysicsList_MuDecayWithSpin::SetBuilderList6()
{
    ClearHadronPhysics();

    fHadronPhysics->push_back(new G4EmExtraPhysics("extra EM"));
    fHadronPhysics->push_back(new G4HadronHElasticPhysics(verboseLevel));
    //fHadronPhysics->push_back(new  G4QStoppingPhysics("stopping",verboseLevel));
    fHadronPhysics->push_back(new G4StoppingPhysics("stopping", verboseLevel));
    fHadronPhysics->push_back(new G4IonBinaryCascadePhysics("ionBIC"));
    fHadronPhysics->push_back(new G4NeutronTrackingCut("nTackingCut",
                                                     verboseLevel));
}

void PhysicsList_MuDecayWithSpin::SetCuts()
{
    if (verboseLevel >0) {
        G4cout << "F04PhysicsList::SetCuts:";
        G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length")
               << G4endl;
    }

    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");

    if (verboseLevel>0) DumpCutValuesTable();
}

void PhysicsList_MuDecayWithSpin::SetCutForGamma(G4double cut)
{
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void PhysicsList_MuDecayWithSpin::SetCutForElectron(G4double cut)
{
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void PhysicsList_MuDecayWithSpin::SetCutForPositron(G4double cut)
{
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

void PhysicsList_MuDecayWithSpin::List()
{
    G4cout << "### PhysicsLists available: FTFC FTFP FTFP_BERT FTFP_EMV FTF_BIC LHEP LHEP_BERT LHEP_EMV "
           << G4endl;
    G4cout << "                            LHEP_PRECO_HP QBBC QBBC_DEL QBBC_HEL QBBC_HP QGSC "
           << G4endl;
    G4cout << "                            QGSC_BERT QGSC_EFLOW QGSC_EMV QGSP QGSP_BERT QGSP_BER_EMV "
           << G4endl;
    G4cout << "                            QGSP_BERT_HP QGSP_BIC QGSP_BIC_HP QGSP_DIF "
           << G4endl;
    G4cout << "                            QGSP_EMV QGSP_EMX QGSP_NQE QGSP_QEL "
           << G4endl;
}

void PhysicsList_MuDecayWithSpin::SetStepMax(G4double step)
{
  MaxChargedStep = step ;
  stepMaxProcess->SetStepMax(MaxChargedStep);
}

F04StepMax* PhysicsList_MuDecayWithSpin::GetStepMaxProcess()
{
  return stepMaxProcess;
}

void PhysicsList_MuDecayWithSpin::AddStepMax()
{
  // Step limitation seen as a process
   
  auto theParticleIterator = GetParticleIterator();
  theParticleIterator->reset();

  while ( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    
    if (stepMaxProcess->IsApplicable(*particle) && !particle->IsShortLived())
      {
	if (pmanager) pmanager ->AddDiscreteProcess(stepMaxProcess);
      }
  }
  /*
  //ku
  G4StepLimiter* stepLimiter = new G4StepLimiter();
  G4UserSpecialCuts* userCuts = new G4UserSpecialCuts();
  theParticleIterator->reset();
  while ((*theParticleIterator)()){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    pmanager ->AddDiscreteProcess(stepLimiter);
    pmanager ->AddDiscreteProcess(userCuts);
  }
  */
}

