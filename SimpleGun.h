#ifndef SIMPLEGUN_H
#define SIMPLEGUN_H
// File modified by Deepak Samuel on 25 Sep 2019

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include <QString>
#include <QObject>
#include "G4GeneralParticleSource.hh"
class G4ParticleGun;
class G4Event;

class SimpleGun : public QObject, public G4VUserPrimaryGeneratorAction
{
    Q_OBJECT

public:
    SimpleGun();
    virtual ~SimpleGun();
    virtual void GeneratePrimaries(G4Event*);
    //void setNumberOfPrimaries(int n){nevent=n;}
    void initialize(){nevent=1;}
    int getCurrentEventId(){return nevent;}

signals:
    void nextEvent( int count);

private:
    G4GeneralParticleSource*  fParticleGun; // pointer a to G4 gun class
    G4Box* fEnvelopeBox;
    static int nevent;

    // double energy;
    // QString G4CADparticle;
    // float px,py,pz;
    //const int nevents; // total number of particles to be shot in a run

};

#endif // SIMPLEGUN_H
