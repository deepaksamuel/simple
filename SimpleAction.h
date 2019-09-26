#ifndef SIMPLEACTION_H
#define SIMPLEACTION_H
// File modified by Deepak Samuel on 25 Sep 2019

#include "G4VUserActionInitialization.hh"
#include "SimpleGun.h"
#include "SimpleRunAction.h"
#include "SimpleSteppingAction.h"

class SimpleAction : public G4VUserActionInitialization
{
public:
    SimpleAction();

    virtual ~SimpleAction();
    virtual void BuildForMaster() const;
    virtual void Build() const;
    SimpleGun* getGunPointer();

    SimpleGun* pGunPointer;

};

#endif // SIMPLEACTION_H
