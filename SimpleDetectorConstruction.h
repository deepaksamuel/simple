#ifndef SIMPLEDETECTORCONSTRUCTION_H
#define SIMPLEDETECTORCONSTRUCTION_H
// File modified by Deepak Samuel on 25 Sep 2019
#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "globals.hh"
#include <QList>
#include <G4Color.hh>

#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "SimpleSteppingAction.h"
//#include <G4GDMLParser.hh>
#include "G4GlobalMagFieldMessenger.hh"
#include "SimpleObject.h"
#include "G4GDMLParser.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class SimpleDetectorConstruction : public G4VUserDetectorConstruction
{

public:
    SimpleDetectorConstruction();
    virtual ~SimpleDetectorConstruction();
    virtual G4VPhysicalVolume* Construct();
    void CreateEmptyWorld();
    void Draw(SimpleObject* obj);
    void SetObjectList(QList <SimpleObject*> objList);
    void SetParser(QString fileName);
    QList <SimpleObject*> GetObjects() const {return objects;}
    void output(QString out){G4cout<<out.toLatin1().data();}
    void appendData(const G4Step *step, ulong event_id) const;
    void setRecordParameter(bool rec_pos, bool rec_time, bool rec_energy, bool rec_mom, bool rec_process);
    bool recordPosition()const{return save_position;}
    bool recordTime()const{return save_time;}
    bool recordEnergy()const{return save_energy;}
    bool recordMomentum()const{return save_momentum;}
    bool recordProcess()const{return save_process;}
    bool hasRecordingObjects() const;
    void setMagneticFields();
    void print(SimpleObject* obj);
   QStringList sensitiveVolumes;


   QList <SimpleObject*> objects;

private:
    G4Box* solidWorld;
    G4LogicalVolume* logicWorld;
    G4VPhysicalVolume* physWorld;
    G4String current_volume_name;
    bool save_position, save_energy, save_momentum, save_time, save_process;
    bool is_gdml;
    G4GDMLParser parser;
};



#endif // SIMPLEDETECTORCONSTRUCTION_H
