#include "SimpleAction.h"
// File modified by Deepak Samuel on 25 Sep 2019
SimpleAction::SimpleAction() : G4VUserActionInitialization()
{
    pGunPointer = new SimpleGun;
}

SimpleAction::~SimpleAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SimpleAction::BuildForMaster() const
{
    SetUserAction(new SimpleRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SimpleAction::Build() const
{

    SetUserAction(pGunPointer);
    SetUserAction(new SimpleRunAction);
    SetUserAction(new SimpleSteppingAction);

}

SimpleGun* SimpleAction::getGunPointer()
{
    return pGunPointer;
}

