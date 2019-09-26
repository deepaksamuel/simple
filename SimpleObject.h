#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H
// File modified by Deepak Samuel on 25 Sep 2019
#include <QVector3D>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <QStringList>
#include <G4NistManager.hh>
#include <G4CSGSolid.hh>
#include <G4VisAttributes.hh>
#include <G4Step.hh>
#include <g4root.hh>
#include <QJsonObject>
#include "globals.hh"

class SimpleObject
{
public:
    SimpleObject(QString type,G4ThreeVector pos, G4ThreeVector rot, G4ThreeVector mag, QString material, QString material_formula="", QString color="black", bool rec_data=false);
    SimpleObject(QJsonObject jsonObject);
    //    SimpleObject(QString type,G4ThreeVector pos, G4ThreeVector rot, G4ThreeVector nbins);
    ~SimpleObject();
    QStringList GetDimensionNames(){return dim_names;}
    QList <double> GetDimensionDefaults();
    void SetDimensions(QList <double> val);
    void SetDimension( int ii, double val);
    void SetRecordParameters(bool recordPos, bool recordTime, bool recordEnergy, bool recordMom, bool recordProcess, QString filterPIDs);
    //    void AppendMeasurementQuantity(QString qty);
    //    void AppendMeasurementQuantityFilter(QString filter);
    //    void AppendMeasurementQuantity(QStringList qtys);
    //    void AppendMeasurementQuantityFilter(QStringList filters);
    //    QStringList MeasurementQuantities(){return measurement_qty;}
    //    QStringList MeasurementFilters(){return  measurement_qty_filter;}
    QList <double> GetDimensions();
    double GetDimension(int p);
    int GetObjId();
    //    bool isMesh(){return is_mesh;}
    QString GetObjectType(){return objType;}
    QString GetObjectName(){return objName;}
    QString GetMaterial(){return objMaterial;}
    QString GetColor(){return objColor;}
    int GetParameterCount(){/*if(isMesh()) return(6+dim_names.count()+3);*/ return (6+dim_names.count());}
    bool GetVisibility(){return !isHidden;}
    G4ThreeVector GetPosition(){return position;}
    G4ThreeVector GetRotation(){return rotation;}
    G4ThreeVector GetMagField(){return magField;}
    //    G4ThreeVector GetBins(){return bins;}
    //    void SetBins(G4ThreeVector bin){bins=bin;}
    G4Material* GetG4Material(){return g4material;}
    void SetMaterial(QString material);
    void SetPosition(G4ThreeVector pos){
        position =pos;
        jsonObject["posx"] = pos.x();
        jsonObject["posy"] = pos.y();
        jsonObject["posz"] = pos.z();
    }
    void SetRotation(G4ThreeVector rot){
        rotation =rot;
        jsonObject["rotx"] = rot.x();
        jsonObject["roty"] = rot.y();
        jsonObject["rotz"] = rot.z();
    }
    void SetMagField(G4ThreeVector mag){
        magField =mag; mag_field_changed=true;
        jsonObject["magx"] = mag.x();
        jsonObject["magy"] = mag.y();
        jsonObject["magz"] = mag.z();
    }
    bool magFieldChanged(){return mag_field_changed;}
    void SetVisiblity(bool hidden);
    void SetMaterialFormula(QString formula){
        objMaterialFormula =formula;
        jsonObject["objMaterialFormula"] = formula;
    }
    QString GetMaterialFormula(){return objMaterialFormula;}
    G4Material* CreateMaterial(QString name, QString formula);
    void output(QString out){G4cout<<out.toLatin1().data();}
    void SetG4Volumes(G4CSGSolid* sV, G4LogicalVolume* lV, G4VPhysicalVolume* pV){solidvol=sV; physVol=pV; logicVol=lV;}
    G4LogicalVolume* GetLogicalVolume(){return logicVol;}
    void SetVisProperties(QString color, bool isVisible, bool isSolid);
    void SetVisSolid(bool isSolid);
    bool storeData(){return record_data;}
    void SetDataStorage(bool store){record_data=store;  jsonObject["record_data"] = store;}
    void ClearData();
    void FillData(const G4Step* step, ulong event_id);
    int appendPositionParameters(int column_id, const G4Step* step);
    int appendTimeParameters(int column_id,const G4Step* step);
    int appendMomentumParamters(int column_id,const G4Step* step);
    int appendEnergyParameters(int column_id,const G4Step* step);
    int appendProcessName(int column_id,const G4Step* step);
    QJsonObject jsonobject(){return jsonObject;}
    //    QStringList GetScoringCommands();
    //  void appendParticleId(int nn, int p_id, int parentId){pid.append(p_id); parent_id.append(parentId);}
private:
    static int objId; // a running id auto incremented

    int id; // id of the local object
    QString objName;
    QString objType;
    QString objColor;
    QString objMaterial;
    QString objMaterialFormula; // in case a custom material is used;
    QString objMaterialProps;


    bool isHidden;
    bool record_data, rec_pos, rec_energy, rec_mom, rec_time, rec_process;
    QList <int> filterIDs;
    bool isWorld;
    bool isSolid;
    //bool is_mesh;
    bool mag_field_changed; // this flag is required to minimize unneccessary initialization of magnetic field in detector construction

    G4ThreeVector position, rotation, magField, bins; // mm, deg, G
    QStringList dim_names;
    QList <double> dim_val;
    //    QStringList measurement_qty ;
    //    QStringList measurement_qty_filter;



    G4LogicalVolume* logicVol;
    G4VPhysicalVolume* physVol;
    G4CSGSolid* solidvol; // all shapes inherit from this
    G4Material* g4material;
    QJsonObject jsonObject;

    // data holders





    // When a new shape is added, the following parameters should also be added accordingly
    QStringList types ={"Box", "Cylinder", "Sphere", "Wedge"};

    QStringList box_params = {"Length (mm)", "Breadth (mm)", "Width (mm)"};
    QStringList cyl_params = {"Inner radius (mm)", "Outer radius (mm)", "Length (mm)","Start angle (deg)" , "Span angle (deg)"};
    QStringList sph_params = {"Inner radius (mm)", "Outer radius (mm)", "Start phi (deg)","Span phi (deg)" , "Start theta (deg)", "Span theta (deg)"};
    QStringList wedge_params= {"Length along y (mm)", "Length along z (mm)", "Wide length along x (mm)","Narrow length along x (mm)"};
    //    QStringList box_mesh_params ={"Length (mm)", "Breadth (mm)", "Width (mm)"};
    //    QStringList cyl_mesh_params ={"Length (mm)", "Radius (mm)"};
    // "Bins", "Quantity to measure","Filters"


    QList <double> default_dims_box={100,100,100};
    QList <double> default_dims_cyl={0,100,100,0,360};
    QList <double> default_dims_sph={0,100,0,360,0,180};
    QList <double> default_dims_wedge={50,50,50,0.1};
    //    QList <double> default_dims_box_mesh={100,100,100};
    //    QList <double> default_dims_cyl_mesh={100,100};

};

#endif // SIMPLEOBJECT_H
