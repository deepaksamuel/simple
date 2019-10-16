// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleDetectorConstruction.h"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
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

}

SimpleDetectorConstruction::~SimpleDetectorConstruction()
{

}


void SimpleDetectorConstruction::CreateEmptyWorld()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

    G4VisAttributes att;
    att.SetForceSolid();
    G4Color col = G4Color::White();
    att.SetColor(col.GetRed(),col.GetGreen(),col.GetBlue(),0.2);
    att.SetVisibility(true);

    solidWorld = new G4Box("World0",10, 10, 10);     //its size
    logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World0");            //its name
    physWorld = new G4PVPlacement(0,  G4ThreeVector(), logicWorld, "World0", 0,false, 0,true);        //overlaps checking

    logicWorld->SetVisAttributes(att);

}


void SimpleDetectorConstruction::SetObjectList(QList<SimpleObject *> objList)
{
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


G4VPhysicalVolume* SimpleDetectorConstruction::Construct()
{
    /* double fExpHall_x, fExpHall_y,fExpHall_z;
    double fBubble_x, fBubble_y,fBubble_z;
    double fTank_x, fTank_y, fTank_z;
    fExpHall_x = fExpHall_y = fExpHall_z = 10.0*m;
    fTank_x    = fTank_y    = fTank_z    =  5.0*m;
    fBubble_x  = fBubble_y  = fBubble_z  =  0.5*m;

    // ------------- Materials -------------

    G4double a, z, density;
    G4int nelements;

    // Air
    //
    G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);

    G4Material* air = new G4Material("Air", density=1.29*mg/cm3, nelements=2);
    air->AddElement(N, 70.*perCent);
    air->AddElement(O, 30.*perCent);

    // Water
    //
    G4Element* H = new G4Element("Hydrogen", "H", z=1 , a=1.01*g/mole);

    G4Material* water = new G4Material("Water", density= 1.0*g/cm3, nelements=2);
    water->AddElement(H, 2);
    water->AddElement(O, 1);

    //
    // ------------ Generate & Add Material Properties Table ------------
    //
    G4double photonEnergy[] =
    { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
      2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
      2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
      2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
      2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
      3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
      3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
      3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV };

    const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);

    //
    // Water
    //
    G4double refractiveIndex1[] =
    { 1.3435, 1.344,  1.3445, 1.345,  1.3455,
      1.346,  1.3465, 1.347,  1.3475, 1.348,
      1.3485, 1.3492, 1.35,   1.3505, 1.351,
      1.3518, 1.3522, 1.3530, 1.3535, 1.354,
      1.3545, 1.355,  1.3555, 1.356,  1.3568,
      1.3572, 1.358,  1.3585, 1.359,  1.3595,
      1.36,   1.3608};

    assert(sizeof(refractiveIndex1) == sizeof(photonEnergy));

    G4double absorption[] =
    {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
     15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
     45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
     52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
     30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
     17.500*m, 14.500*m };

    assert(sizeof(absorption) == sizeof(photonEnergy));

    G4double scintilFast[] =
    { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00 };

    assert(sizeof(scintilFast) == sizeof(photonEnergy));

    G4double scintilSlow[] =
    { 0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00,
      7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 4.00,
      3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00,
      4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
      7.00, 6.00, 5.00, 4.00 };

    assert(sizeof(scintilSlow) == sizeof(photonEnergy));

    G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

    myMPT1->AddProperty("RINDEX",       photonEnergy, refractiveIndex1,nEntries)
            ->SetSpline(true);
    myMPT1->AddProperty("ABSLENGTH",    photonEnergy, absorption,     nEntries)
            ->SetSpline(true);
    myMPT1->AddProperty("FASTCOMPONENT",photonEnergy, scintilFast,     nEntries)
            ->SetSpline(true);
    myMPT1->AddProperty("SLOWCOMPONENT",photonEnergy, scintilSlow,     nEntries)
            ->SetSpline(true);

    myMPT1->AddConstProperty("SCINTILLATIONYIELD",50./MeV);
    myMPT1->AddConstProperty("RESOLUTIONSCALE",1.0);
    myMPT1->AddConstProperty("FASTTIMECONSTANT", 1.*ns);
    myMPT1->AddConstProperty("SLOWTIMECONSTANT",10.*ns);
    myMPT1->AddConstProperty("YIELDRATIO",0.8);

    G4double energy_water[] = {
        1.56962*eV, 1.58974*eV, 1.61039*eV, 1.63157*eV,
        1.65333*eV, 1.67567*eV, 1.69863*eV, 1.72222*eV,
        1.74647*eV, 1.77142*eV, 1.7971 *eV, 1.82352*eV,
        1.85074*eV, 1.87878*eV, 1.90769*eV, 1.93749*eV,
        1.96825*eV, 1.99999*eV, 2.03278*eV, 2.06666*eV,
        2.10169*eV, 2.13793*eV, 2.17543*eV, 2.21428*eV,
        2.25454*eV, 2.29629*eV, 2.33962*eV, 2.38461*eV,
        2.43137*eV, 2.47999*eV, 2.53061*eV, 2.58333*eV,
        2.63829*eV, 2.69565*eV, 2.75555*eV, 2.81817*eV,
        2.88371*eV, 2.95237*eV, 3.02438*eV, 3.09999*eV,
        3.17948*eV, 3.26315*eV, 3.35134*eV, 3.44444*eV,
        3.54285*eV, 3.64705*eV, 3.75757*eV, 3.87499*eV,
        3.99999*eV, 4.13332*eV, 4.27585*eV, 4.42856*eV,
        4.59258*eV, 4.76922*eV, 4.95999*eV, 5.16665*eV,
        5.39129*eV, 5.63635*eV, 5.90475*eV, 6.19998*eV
    };

    const G4int numentries_water = sizeof(energy_water)/sizeof(G4double);

    //assume 100 times larger than the rayleigh scattering for now.
    G4double mie_water[] = {
        167024.4*m, 158726.7*m, 150742  *m,
        143062.5*m, 135680.2*m, 128587.4*m,
        121776.3*m, 115239.5*m, 108969.5*m,
        102958.8*m, 97200.35*m, 91686.86*m,
        86411.33*m, 81366.79*m, 76546.42*m,
        71943.46*m, 67551.29*m, 63363.36*m,
        59373.25*m, 55574.61*m, 51961.24*m,
        48527.00*m, 45265.87*m, 42171.94*m,
        39239.39*m, 36462.50*m, 33835.68*m,
        31353.41*m, 29010.30*m, 26801.03*m,
        24720.42*m, 22763.36*m, 20924.88*m,
        19200.07*m, 17584.16*m, 16072.45*m,
        14660.38*m, 13343.46*m, 12117.33*m,
        10977.70*m, 9920.416*m, 8941.407*m,
        8036.711*m, 7202.470*m, 6434.927*m,
        5730.429*m, 5085.425*m, 4496.467*m,
        3960.210*m, 3473.413*m, 3032.937*m,
        2635.746*m, 2278.907*m, 1959.588*m,
        1675.064*m, 1422.710*m, 1200.004*m,
        1004.528*m, 833.9666*m, 686.1063*m
    };

    assert(sizeof(mie_water) == sizeof(energy_water));

    // gforward, gbackward, forward backward ratio
    G4double mie_water_const[3]={0.99,0.99,0.8};

    myMPT1->AddProperty("MIEHG",energy_water,mie_water,numentries_water)
            ->SetSpline(true);
    myMPT1->AddConstProperty("MIEHG_FORWARD",mie_water_const[0]);
    myMPT1->AddConstProperty("MIEHG_BACKWARD",mie_water_const[1]);
    myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO",mie_water_const[2]);

    G4cout << "Water G4MaterialPropertiesTable" << G4endl;
    myMPT1->DumpTable();

    water->SetMaterialPropertiesTable(myMPT1);

    // Set the Birks Constant for the Water scintillator

    water->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    //
    // Air
    //
    G4double refractiveIndex2[] =
    { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
      1.00, 1.00, 1.00, 1.00 };

    G4MaterialPropertiesTable* myMPT2 = new G4MaterialPropertiesTable();
    myMPT2->AddProperty("RINDEX", photonEnergy, refractiveIndex2, nEntries);

    G4cout << "Air G4MaterialPropertiesTable" << G4endl;
    myMPT2->DumpTable();

    air->SetMaterialPropertiesTable(myMPT2);

    //
    // ------------- Volumes --------------

    // The experimental Hall
    //
    G4Box* expHall_box = new G4Box("World",fExpHall_x,fExpHall_y,fExpHall_z);

    G4LogicalVolume* expHall_log
            = new G4LogicalVolume(expHall_box,water,"World",0,0,0);

    G4VPhysicalVolume* expHall_phys
            = new G4PVPlacement(0,G4ThreeVector(),expHall_log,"World",0,false,0);

   // The Water Tank
    //
    G4Box* waterTank_box = new G4Box("Tank",fTank_x,fTank_y,fTank_z);

    G4LogicalVolume* waterTank_log
            = new G4LogicalVolume(waterTank_box,water,"Tank",0,0,0);

    G4VPhysicalVolume* waterTank_phys
            = new G4PVPlacement(0,G4ThreeVector(),waterTank_log,"Tank",
                                expHall_log,false,0);

    // The Air Bubble
    //
    G4Box* bubbleAir_box = new G4Box("Bubble",fBubble_x,fBubble_y,fBubble_z);

    G4LogicalVolume* bubbleAir_log
            = new G4LogicalVolume(bubbleAir_box,air,"Bubble",0,0,0);

    //G4VPhysicalVolume* bubbleAir_phys =
    new G4PVPlacement(0,G4ThreeVector(0,2.5*m,0),bubbleAir_log,"Bubble",
                      waterTank_log,false,0);

    // ------------- Surfaces --------------
    //
    // Water Tank
    //
    G4OpticalSurface* opWaterSurface = new G4OpticalSurface("WaterSurface");
    //  opWaterSurface->SetType(dielectric_dielectric);
    //  opWaterSurface->SetFinish(ground);
    //  opWaterSurface->SetModel(unified);
    opWaterSurface->SetType(dielectric_LUTDAVIS);
    opWaterSurface->SetFinish(Rough_LUT);
    opWaterSurface->SetModel(DAVIS);

    G4LogicalBorderSurface* waterSurface =
            new G4LogicalBorderSurface("WaterSurface",
                                       waterTank_phys,expHall_phys,opWaterSurface);

    G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*>
            (waterSurface->GetSurface(waterTank_phys,expHall_phys)->
             GetSurfaceProperty());
    if (opticalSurface) opticalSurface->DumpInfo();

    // Air Bubble
    //
    G4OpticalSurface* opAirSurface = new G4OpticalSurface("AirSurface");
    opAirSurface->SetType(dielectric_dielectric);
    opAirSurface->SetFinish(polished);
    opAirSurface->SetModel(glisur);

    G4LogicalSkinSurface* airSurface =
            new G4LogicalSkinSurface("AirSurface", bubbleAir_log, opAirSurface);

    opticalSurface = dynamic_cast <G4OpticalSurface*>
            (airSurface->GetSurface(bubbleAir_log)->GetSurfaceProperty());
    if (opticalSurface) opticalSurface->DumpInfo();

    //
    // Generate & Add Material Properties Table attached to the optical surfaces
    //
    const G4int num = 2;
    G4double ephoton[num] = {2.034*eV, 4.136*eV};

    //OpticalWaterSurface
    G4double refractiveIndex[num] = {1.35, 1.40};
    G4double specularLobe[num]    = {0.3, 0.3};
    G4double specularSpike[num]   = {0.2, 0.2};
    G4double backScatter[num]     = {0.2, 0.2};

    G4MaterialPropertiesTable* myST1 = new G4MaterialPropertiesTable();

    myST1->AddProperty("RINDEX",                ephoton, refractiveIndex, num);
    myST1->AddProperty("SPECULARLOBECONSTANT",  ephoton, specularLobe,    num);
    myST1->AddProperty("SPECULARSPIKECONSTANT", ephoton, specularSpike,   num);
    myST1->AddProperty("BACKSCATTERCONSTANT",   ephoton, backScatter,     num);

    G4cout << "Water Surface G4MaterialPropertiesTable" << G4endl;
    myST1->DumpTable();

    //  opWaterSurface->SetMaterialPropertiesTable(myST1);

    //OpticalAirSurface
    G4double reflectivity[num] = {0.3, 0.5};
    G4double efficiency[num]   = {0.8, 1.0};

    G4MaterialPropertiesTable *myST2 = new G4MaterialPropertiesTable();

    myST2->AddProperty("REFLECTIVITY", ephoton, reflectivity, num);
    myST2->AddProperty("EFFICIENCY",   ephoton, efficiency,   num);

    G4cout << "Air Surface G4MaterialPropertiesTable" << G4endl;
    myST2->DumpTable();

    opAirSurface->SetMaterialPropertiesTable(myST2);

    //always return the physical World
    return expHall_phys;*/







    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();


    if(objects.count()==0){ //create an empty box if nothing is present
        // G4cout<<"No world volume found!";
        CreateEmptyWorld();
    }


    else{
        //qDebug()<<"Number of objects"<<objects.count();
        bool magFieldChanged=false;
        for(int ii=0; ii<objects.count(); ii++){
            SimpleObject* obj = objects.at(ii);
            magFieldChanged=obj->magFieldChanged();
            Draw(obj);
            if(ii<100)
                print(obj);
        }
        if(magFieldChanged) // do this only if magnetic field changed
            setMagneticFields();
    }
    return physWorld;


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
