// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleDetectorConstruction.h"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include <QFile>
#include <QDebug>
#include <QColor>
#include <QRgb>
#include <QMessageBox>
#include <G4GeometryManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4SolidStore.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalSkinSurface.hh>

//#include <G4GDMLParser.hh>

SimpleDetectorConstruction::SimpleDetectorConstruction() : G4VUserDetectorConstruction()
{
    is_gdml = false;
}

SimpleDetectorConstruction::~SimpleDetectorConstruction()
{

}


void SimpleDetectorConstruction::CreateEmptyWorld()
{
    is_gdml = false;
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

    G4VisAttributes att;
    att.SetForceSolid();
    G4Color col = G4Color::White();
    att.SetColor(col.GetRed(),col.GetGreen(),col.GetBlue(),0.2);
    att.SetVisibility(true);

    solidWorld = new G4Box("World",10, 10, 10);     //its size
    logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");            //its name
    physWorld = new G4PVPlacement(0,  G4ThreeVector(), logicWorld, "World", 0,false, 0,true);        //overlaps checking

    logicWorld->SetVisAttributes(att);

}


void SimpleDetectorConstruction::SetObjectList(QList<SimpleObject *> objList)
{
    is_gdml = false;
    objects = objList;
    sensitiveVolumes.clear();
    foreach(SimpleObject* o, objects){
        if(o->storeData())
            sensitiveVolumes.append(o->GetObjectName());
    }

}

void SimpleDetectorConstruction::print(SimpleObject *obj)
{
    qDebug()<<"-----------------------------------";
    qDebug()<<obj->GetObjectName();
    qDebug()<<"Dimensions (mm)"<<obj->GetDimensions();
    qDebug()<<"Position (mm)"<<obj->GetPosition().x()<<obj->GetPosition().y()<<obj->GetPosition().z();
    qDebug()<<"Rotation (deg)"<<obj->GetRotation().x()<<obj->GetRotation().y()<<obj->GetRotation().z();
    qDebug()<<"Mag. Field (G)"<<obj->GetMagField().x()<<obj->GetMagField().y()<<obj->GetMagField().z();
    qDebug()<<"Material"<<obj->GetG4Material()->GetName();
    if(obj->GetG4Material()->GetMaterialPropertiesTable()!=nullptr)
        obj->GetG4Material()->GetMaterialPropertiesTable()->DumpTable();
    qDebug()<<"-----------------------------------";
}
void SimpleDetectorConstruction::Draw(SimpleObject *obj)
{
    if(obj==nullptr){
        G4cerr<<"Null object cannot be drawn...";
        return;
    }
    G4RotationMatrix* myRotation = new G4RotationMatrix();
    myRotation->rotateX(obj->GetRotation().x()*deg);
    myRotation->rotateY(obj->GetRotation().y()*deg);
    myRotation->rotateZ(obj->GetRotation().z()*deg);

    G4CSGSolid* sV;
    G4LogicalVolume* logicVol;
    G4VPhysicalVolume* physVol;


    if(obj->GetObjectType()=="Box"){
        G4Box* solidBox = new G4Box(obj->GetObjectName().toLatin1().data(),
                                    obj->GetDimension(0)/2*mm,
                                    obj->GetDimension(1)/2*mm,
                                    obj->GetDimension(2)/2*mm);

        sV = solidBox;
        if(obj->GetObjectName()=="World")
            solidWorld=solidBox;

    }

    else if(obj->GetObjectType()=="Sphere"){
        G4Sphere* solidSphere = new G4Sphere(obj->GetObjectName().toLatin1().data(),
                                             obj->GetDimension(0)*mm,
                                             obj->GetDimension(1)*mm,
                                             obj->GetDimension(2)*deg,
                                             obj->GetDimension(3)*deg,
                                             obj->GetDimension(4)*deg,
                                             obj->GetDimension(5)*deg);
        sV = solidSphere;
    }

    else if(obj->GetObjectType()=="Cylinder"){
        G4Tubs* solidTubs = new G4Tubs(obj->GetObjectName().toLatin1().data(),
                                       obj->GetDimension(0)*mm,
                                       obj->GetDimension(1)*mm,
                                       obj->GetDimension(2)/2*mm,
                                       obj->GetDimension(3)*deg,
                                       obj->GetDimension(4)*deg);
        sV = solidTubs;
    }

    else if(obj->GetObjectType()=="Wedge"){
        G4Trap* solidWedge = new G4Trap(obj->GetObjectName().toLatin1().data(),
                                        obj->GetDimension(0)*mm,
                                        obj->GetDimension(1)*mm,
                                        obj->GetDimension(2)*mm,
                                        obj->GetDimension(3)*mm
                                        );
        sV = solidWedge;
    }

    logicVol = new G4LogicalVolume(sV, obj->GetG4Material(), obj->GetObjectName().toLatin1().data());
    if(obj->GetObjectName()=="World"){
        logicWorld = logicVol;
    }
    if(obj->GetObjectName()=="World"){
        physVol = new G4PVPlacement(myRotation,  G4ThreeVector(obj->GetPosition().x()*mm,obj->GetPosition().y()*mm,obj->GetPosition().z()*mm), logicVol, obj->GetObjectName().toLatin1().data(), 0,false, 0,true);
        physWorld  = physVol;
    }
    else
        physVol = new G4PVPlacement(myRotation,  G4ThreeVector(obj->GetPosition().x()*mm,obj->GetPosition().y()*mm,obj->GetPosition().z()*mm), logicVol, obj->GetObjectName().toLatin1().data(), logicWorld,false, 0,true);


    G4VisAttributes att;
    QColor col(obj->GetColor());
    att.SetColor(col.red(),col.green(),col.blue(),0.2);
    att.SetVisibility(obj->GetVisibility());
    att.SetForceSolid();
    logicVol->SetVisAttributes(att);

    obj->SetG4Volumes(sV,logicVol,physVol);

}

void SimpleDetectorConstruction::SetParser(QString fileName)
{
    is_gdml=true;
    parser.Read(fileName.toLatin1().data());
}

G4VPhysicalVolume* SimpleDetectorConstruction::Construct()
{

    if(is_gdml){
        qDebug()<<"Constructing shapes from GDML file...";
        return parser.GetWorldVolume();
    }
    else{
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
        G4SolidStore::GetInstance()->Clean();
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
        qDebug()<<"Number of objects"<<objects.count();

        if(objects.count()==0){ //create an empty box if nothing is present
            // G4cout<<"No world volume found!";
            G4cout<<"Creating empty world volume";
            CreateEmptyWorld();
        }
        else{
            qDebug()<<"Number of objects"<<objects.count();

            bool magFieldChanged=false;
            for(int ii=0; ii<objects.count(); ii++){
                SimpleObject* obj = objects.at(ii);
                if(magFieldChanged==false)
                    magFieldChanged=obj->magFieldChanged();
                Draw(obj);
                //if(ii<100)
                print(obj);
            }
            if(magFieldChanged) // do this only if magnetic field changed
                setMagneticFields();
        }
        return physWorld;
    }


}

void SimpleDetectorConstruction::appendData(const G4Step *step, ulong event_id) const
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    foreach(SimpleObject* obj, objects)
    {
        G4String current_volume_name  = step->GetPreStepPoint()->GetTouchableHandle() ->GetVolume()->GetLogicalVolume()->GetName();
        if(current_volume_name=="World") break;
        else {
            obj->FillData(step,event_id);
        }

    }
}

// this will save some time.. the stepping action avoid skip many processing
bool SimpleDetectorConstruction::hasRecordingObjects() const
{
    if(save_position || save_time || save_energy || save_momentum || save_process)
        return true;
    else
        return false;

}
void SimpleDetectorConstruction::setRecordParameter(bool rec_pos, bool rec_time, bool rec_energy, bool rec_mom, bool rec_process)
{

    save_position = rec_pos;
    save_time = rec_time;
    save_energy = rec_energy;
    save_momentum = rec_mom;
    save_process = rec_process;

}

void SimpleDetectorConstruction::setMagneticFields()
{
    G4cout<<"Resetting all magnetic fields...";
    for(int ii=0; ii<objects.count();ii++){
        if(ii==0){// world volume
            G4ThreeVector fieldValue = G4ThreeVector(0.000000*gauss,0.000000*gauss,0.000000*gauss);
            G4GlobalMagFieldMessenger* fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
            fMagFieldMessenger->SetVerboseLevel(1);
        }
        else{
            G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(0.000000*gauss,0.000000*gauss,0.000000*gauss));
            G4FieldManager* fieldMgr    = new G4FieldManager(magField);
            fieldMgr->SetDetectorField(magField);
            fieldMgr->CreateChordFinder(magField);
            objects.at(ii)->GetLogicalVolume()->SetFieldManager(fieldMgr, true);
            G4cout<<"Setting magnetic field for "<<objects.at(ii)->GetObjectName().toLatin1().data();
        }
    }
    G4cout<<"Setting magnetic fields...";


    for(int ii=0; ii<objects.count();ii++){
        if(ii==0){// world volume
            if(objects.at(0)->GetMagField().x()!=0.000 ||
                    objects.at(0)->GetMagField().y()!=0.000 ||
                    objects.at(0)->GetMagField().z()!=0.000){
                G4ThreeVector fieldValue = G4ThreeVector(objects.at(0)->GetMagField().x()*gauss,
                                                         objects.at(0)->GetMagField().y()*gauss,
                                                         objects.at(0)->GetMagField().z()*gauss);
                G4GlobalMagFieldMessenger* fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
                fMagFieldMessenger->SetVerboseLevel(1);
                G4cout<<"Setting magnetic field for "<<objects.at(ii)->GetObjectName().toLatin1().data();
                break; // if magnetic field is set for world, do not set for individual volumes
            }
        }
        else{ // set magnetic field for part of hierarchy
            //http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Detector/electroMagneticField.html?highlight=magnetic%20field
            //part of volume hierarchy
            if(objects.at(ii)->GetMagField().x()!=0.000 ||
                    objects.at(ii)->GetMagField().y()!=0.000 ||
                    objects.at(ii)->GetMagField().z()!=0.000 ){
                G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(objects.at(ii)->GetMagField().x()*gauss,
                                                                                  objects.at(ii)->GetMagField().y()*gauss,
                                                                                  objects.at(ii)->GetMagField().z()*gauss));
                //G4FieldManager* fieldMgr    = G4TransportationManager::GetTransportationManager()->GetFieldManager(); //use this for global
                G4FieldManager* fieldMgr    = new G4FieldManager(magField);

                fieldMgr->SetDetectorField(magField);
                fieldMgr->CreateChordFinder(magField);
                objects.at(ii)->GetLogicalVolume()->SetFieldManager(fieldMgr, true);
                G4cout<<"Setting magnetic field for "<<objects.at(ii)->GetObjectName().toLatin1().data();

            }
        }

    }
}
//void SimpleDetectorConstruction::SetMaterialProperties(G4CADObjectStruct obj, G4Material *mat)
//{
//    // This is done as a work around for demonstration of cerenkov detectors
//    if(obj.material == "G4_WATER")
//    {
//        const G4int num = 2;
//        G4double ephoton[num] = {2.034*eV, 4.136*eV};

//        //OpticalWaterSurface
//        G4double refractiveIndex[num] = {1.35, 1.40};
//        G4double specularLobe[num]    = {0.3, 0.3};
//        G4double specularSpike[num]   = {0.2, 0.2};
//        G4double backScatter[num]     = {0.2, 0.2};

//        G4MaterialPropertiesTable* matTable = new G4MaterialPropertiesTable();

//        matTable->AddProperty("RINDEX",                ephoton, refractiveIndex, num);
//        matTable->AddProperty("SPECULARLOBECONSTANT",  ephoton, specularLobe,    num);
//        matTable->AddProperty("SPECULARSPIKECONSTANT", ephoton, specularSpike,   num);
//        matTable->AddProperty("BACKSCATTERCONSTANT",   ephoton, backScatter,     num);

//        mat->SetMaterialPropertiesTable(matTable);
//        qDebug()<<"GStudio: Added refractive index and optical properties for water"<<endl;
//        //G4cout << "Water Surface G4MaterialPropertiesTable" << G4endl;
//        //matTable->DumpTable();
//        //G4cout
//    }
//    // This is done as a work around for demonstration of scintillation detectors
//    if(obj.material == "G4_lXe")
//    {
//        G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
//        const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

//        G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
//        assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
//        G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
//        assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
//        G4double lxe_ABSL[]  = { 35.*cm, 35.*cm, 35.*cm};
//        assert(sizeof(lxe_ABSL) == sizeof(lxe_Energy));
//        G4MaterialPropertiesTable* fLXe_mt = new G4MaterialPropertiesTable();
//        fLXe_mt->AddProperty("FASTCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
//        fLXe_mt->AddProperty("SLOWCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
//        fLXe_mt->AddProperty("RINDEX",        lxe_Energy, lxe_RIND,  lxenum);
//        fLXe_mt->AddProperty("ABSLENGTH",     lxe_Energy, lxe_ABSL,  lxenum);
//        fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
//        fLXe_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
//        fLXe_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
//        fLXe_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
//        fLXe_mt->AddConstProperty("YIELDRATIO",1.0);
//        mat->SetMaterialPropertiesTable(fLXe_mt);
//        // Set the Birks Constant for the LXe scintillator

//        mat->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
//        mat->SetMaterialPropertiesTable(fLXe_mt);
//        qDebug()<<"GStudio: Added refractive index and optical (scintialltion) properties for liquid xenon"<<endl;
//    }

//    if((obj.material == "G4_Galactic") )
//    {
//        G4double vacuum_Energy[]={2.0*eV,7.0*eV,7.14*eV};
//        const G4int vacnum = sizeof(vacuum_Energy)/sizeof(G4double);
//        G4double vacuum_RIND[]={1.,1.,1.};
//        assert(sizeof(vacuum_RIND) == sizeof(vacuum_Energy));
//        G4MaterialPropertiesTable *vacuum_mt = new G4MaterialPropertiesTable();
//        vacuum_mt->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND,vacnum);
//        mat->SetMaterialPropertiesTable(vacuum_mt);
//        qDebug()<<"GStudio: Added refractive index and optical (scintialltion) properties for Vacuum (Ref. Index =1)"<<endl;
//    }
//}
