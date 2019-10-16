// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleObject.h"
#include "ui_simpleobject.h"
#include <QStringList>
#include "G4SystemOfUnits.hh"
#include "G4VProcess.hh"
#include <QDebug>
#include <SimpleMaterialBuilder.h>

int SimpleObject::objId=0;

SimpleObject::SimpleObject(QString type, G4ThreeVector pos, G4ThreeVector rot, G4ThreeVector mag, QString material, QString material_formula, QString color, bool rec_data)
{
    // is_mesh=false;
    if(!types.contains(type))
        return;
    if(type=="Box"){
        dim_names = box_params;
        dim_val = default_dims_box;

    }
    else if (type=="Cylinder"){
        dim_names = cyl_params;
        dim_val = default_dims_cyl;
    }
    else if(type=="Sphere"){
        dim_names = sph_params;
        dim_val = default_dims_sph;
    }
    else if (type=="Wedge"){
        dim_names = wedge_params;
        dim_val = default_dims_wedge;
    }



    if(objId==0){
        objName = "World";
        isWorld =true;
    }
    else{
        objName = QString("%1-%2").arg(type).arg(objId,3,10, QChar('0'));
        isWorld = false;
    }

    if(objId==0)
        isWorld=true;
    else {
        isWorld=false;
    }

    id = objId;
    objType =type;
    objColor=color;
    position = pos;
    rotation = rot;
    magField = mag;
    objMaterial =material;
    objMaterialFormula =material_formula;
    objMaterialProps ="";
    record_data = rec_data;
    isSolid=true;

    if(magField.x()==0.00000 & magField.y()==0.0000 && magField.z()==0.0000)
        mag_field_changed=false;
    else {
        mag_field_changed=true;
    }
    magfield_off = false;

    jsonObject["id"] = id;
    jsonObject["objName"] = objName;
    jsonObject["objType"] = type;
    jsonObject["objColor"] = color;
    jsonObject["posx"] = pos.x();
    jsonObject["posy"] = pos.y();
    jsonObject["posz"] = pos.z();
    jsonObject["rotx"] = rot.x();
    jsonObject["roty"] = rot.y();
    jsonObject["rotz"] = rot.z();
    jsonObject["magx"] = mag.x();
    jsonObject["magy"] = mag.y();
    jsonObject["magz"] = mag.z();
    jsonObject["objMaterial"] = material;
    jsonObject["objMaterialFormula"] = material_formula;
    jsonObject["objMaterialProps"] = objMaterialProps;
    jsonObject["record_data"] = rec_data;
    int ii=0;
    foreach(double dim, dim_val){
        jsonObject[QString("dim-%1").arg(ii++)] = dim;
    }

    G4NistManager* nist = G4NistManager::Instance();

    if(objMaterial.contains("G4_"))
        g4material = nist->FindOrBuildMaterial(objMaterial.toLatin1().data());
    else {
        // custom material, created
        g4material= SimpleMaterialBuilder::getInstance()->GetMaterial(objMaterial, objMaterialFormula);
        if(g4material->GetName()=="G4_AIR"){
            objMaterial="G4_AIR"; // for the initial world volume
            objMaterialFormula="";
        }
    }
    objId++;
}

SimpleObject::SimpleObject(QJsonObject jo)
{

    id = jo["id"].toInt();
    objName= jo["objName"].toString();
    if(objName=="World") isWorld=true;
    objType= jo["objType"].toString();
    objColor=jo["objColor"].toString();
    position.set(jo["posx"].toDouble(),jo["posy"].toDouble(),jo["posz"].toDouble());
    rotation.set(jo["rotx"].toDouble(),jo["roty"].toDouble(),jo["rotz"].toDouble());
    magField.set(jo["magx"].toDouble(),jo["magy"].toDouble(),jo["magz"].toDouble());
    objMaterial= jo["objMaterial"].toString();
    objMaterialFormula = jo["objMaterialFormula"].toString();
    objMaterialProps=  jo["objMaterialProps"].toString();
    record_data =  jo["record_data"].toBool();

    if(magField.x()==0.00000 & magField.y()==0.0000 && magField.z()==0.0000)
        mag_field_changed=false;
    else {
        mag_field_changed=true;
    }
    magfield_off = false;

    int par_count=0;

    if(objType=="Box"){ dim_names = box_params; par_count=box_params.count();}
    if(objType=="Cylinder"){ dim_names = cyl_params; par_count=cyl_params.count();}
    if(objType=="Sphere"){ dim_names = sph_params;par_count = sph_params.count();}
    if(objType=="Wedge"){ dim_names = wedge_params;par_count = wedge_params.count();}

    for(int ii=0; ii<par_count;ii++){
        dim_val.append(jo[QString("dim-%1").arg(ii)].toDouble());
    }

    G4NistManager* nist = G4NistManager::Instance();
    if(objMaterial.contains("G4"))
        g4material = nist->FindOrBuildMaterial(objMaterial.toLatin1().data());
    else {
        // custom material, created
        g4material= SimpleMaterialBuilder::getInstance()->GetMaterial(objMaterial, objMaterialFormula);
        if(g4material->GetName()=="G4_AIR"){ // for the initial world volume
            objMaterial="G4_AIR";
            objMaterialFormula="";
        }
    }
    jsonObject = jo;
    objId++;
}

//SimpleObject::SimpleObject(QString type,G4ThreeVector pos, G4ThreeVector rot, G4ThreeVector nbins)
//{
//    is_mesh =true;
//    if(type=="Box Mesh")
//    {
//        dim_names = box_mesh_params;
//        dim_val = default_dims_box_mesh;
//    }
//    else if(type=="Cylinder Mesh")
//    {
//        dim_names = cyl_mesh_params;
//        dim_val = default_dims_cyl_mesh;
//    }

//    objName = QString("%1-%2").arg(type).arg(objId,3,10, QChar('0'));
//    isWorld = false;


//    bins = nbins;
//    id = objId;
//    objType =type;
//    position = pos;
//    rotation = rot;
//    objMaterial ="NA";
//    record_data = false;
//    objId++;
//}

SimpleObject::~SimpleObject()
{
    objId--;
}

G4Material* SimpleObject::CreateMaterial(QString name,QString formula)
{
    // the format is a*mat1+b*mat2, density=p
    // where mat1 and mat2 are G4 materials and a and b are fractions such that a+b = 1 and p is the density in g/cm3
    // if the formula has errors, the material is set to G4_AIR
    G4NistManager* nist = G4NistManager::Instance();

    QStringList s = formula.split(",");
    double density =0; // in g/cm3
    int nelements=0;
    if(s.count()==2){
        QString d1 = s.at(1);
        QStringList d2 = d1.split("=");
        if(d2.count()==2){
            density = d2.at(1).toDouble();
        }
        else {
            output(QString("Error in creating material %1 with formula. Incorrect density format").arg(name).arg(formula));
            output("The correct format is a*mat1+b*mat2, density=p, where mat1 and mat2 are G4 elements and a and b are fractions such that a+b = 1 and p is the density in g/cm3");
            output("The material is set to G4_AIR");
            objMaterial="G4_AIR";
            objMaterialFormula="";
            return nist->FindOrBuildMaterial("G4_AIR");
        }
        QStringList d3 = s.at(0).split("+");
        nelements = d3.count();

        if(nelements<=0){
            output(QString("Error in creating material %1 with formula. Incorrect of number of elements: %3").arg(name).arg(formula).arg(nelements));
            output("The correct format is a*mat1+b*mat2, density=p, where mat1 and mat2 are G4 elements and a and b are fractions such that a+b = 1 and p is the density in g/cm3");
            output("The material is set to G4_AIR");
            objMaterial="G4_AIR";
            objMaterialFormula="";
            return nist->FindOrBuildMaterial("G4_AIR");
        }

        G4Material* new_material = new G4Material(name.toLatin1().data(), density*g/cm3, nelements);
        for(int ii=0; ii<nelements;ii++){
            QStringList d4 = d3.at(ii).split("*");
            if(d4.count()!=2){
                output(QString("Error in creating material %1 with formula. Incorrect of number of elements: %3").arg(name).arg(formula).arg(nelements));
                output("The correct format is a*mat1+b*mat2, density=p, where mat1 and mat2 are G4 elements and a and b are fractions such that a+b = 1 and p is the density in g/cm3");
                output("The material is set to G4_AIR");
                objMaterial="G4_AIR";
                objMaterialFormula="";
                return nist->FindOrBuildMaterial("G4_AIR");
            }

            G4Element* e=nist->FindOrBuildElement(d4.at(1).toStdString());
            if(e!=nullptr)
                new_material->AddElement(e,d4.at(0).toDouble());
            else {
                output(QString("Error in creating material %1 with formula. Could not find or build element: %3").arg(name).arg(formula).arg(d4.at(1)));
                output("The correct format is a*mat1+b*mat2, density=p, where mat1 and mat2 are G4 elements and a and b are fractions such that a+b = 1 and p is the density in g/cm3");
                output("The material is set to G4_AIR");
                objMaterial="G4_AIR";
                objMaterialFormula="";
                return nist->FindOrBuildMaterial("G4_AIR");
            }
        }
        return new_material;

    }
    else {
        output(QString("Error in creating material %1 with formula. Incorrect format.").arg(name).arg(formula));
        output("The correct format is a*mat1+b*mat2, density=p, where mat1 and mat2 are G4 elements and a and b are fractions such that a+b = 1 and p is the density in g/cm3");
        output("The material is set to G4_AIR");
        objMaterial="G4_AIR";
        objMaterialFormula="";
        return nist->FindOrBuildMaterial("G4_AIR");
    }


}

void SimpleObject::SetMaterial(QString material)
{
    G4NistManager* nist = G4NistManager::Instance();
    objMaterial=material;
    jsonObject["objMaterial"] = material;

    if(!material.contains("G4_")){ // user created material
        objMaterialFormula=SimpleMaterialBuilder::getInstance()->GetMaterialFormula(material);
        jsonObject["objMaterialFormula"] = objMaterialFormula;
    }

    if(objMaterial.contains("G4"))
        g4material = nist->FindOrBuildMaterial(objMaterial.toLatin1().data());
    else {
        g4material= SimpleMaterialBuilder::getInstance()->GetMaterial(objMaterial, objMaterialFormula);
    }
}

QList <double> SimpleObject::GetDimensionDefaults()
{
    if(objType=="Box"){
        return  default_dims_box;
    }
    else if (objType=="Cylinder"){
        return  default_dims_cyl;
    }
    else if(objType=="Sphere"){
        return  default_dims_sph;
    }
    else if (objType=="Wedge"){
        return  default_dims_wedge;
    }

}

void SimpleObject::SetDimensions(QList <double> val)
{
    dim_val = val;
    int ii=0;
    foreach(double dim, dim_val){
        jsonObject[QString("dim-%1").arg(ii++)] = dim;
    }
}

void SimpleObject::SetDimension(int ii, double val)
{
    dim_val.replace(ii,val);
    jsonObject[QString("dim-%1").arg(ii)] = val;

}
void SimpleObject::SetRecordParameters(bool recordPos, bool recordTime, bool recordEnergy, bool recordMom, bool recordProcess, QString filterPIDs)
{
    filterIDs.clear();
    rec_pos = recordPos;
    rec_time = recordTime;
    rec_energy = recordEnergy;
    rec_mom = recordMom;
    rec_process = recordProcess;
    if(filterPIDs!=""){
        QStringList sl = filterPIDs.split(",");
        if(sl.count()>0){
            foreach(QString s, sl)
                filterIDs.append(s.toInt());
        }
    }

}
QList <double> SimpleObject::GetDimensions()
{
    return dim_val;
}

double SimpleObject::GetDimension(int p)
{
    return dim_val.at(p);
}

int SimpleObject::GetObjId()
{
    return id;
}

void SimpleObject::SetVisProperties(QString color, bool isVisible, bool isSolid)
{
    G4VisAttributes att = logicVol->GetVisAttributes();
    QColor col(color);
    att.SetColor(col.red(),col.green(),col.blue(),0.2);
    att.SetVisibility(isVisible);
    //    if(isSolid)
    //        att.SetForceSolid();
    //    else {
    //        att.SetForceWireframe();
    //    }
    logicVol->SetVisAttributes(att);
}

void SimpleObject::SetVisSolid(bool isSolid)
{
    G4VisAttributes att= logicVol->GetVisAttributes();
    QColor col(objColor);
    att.SetColor(col.red(),col.green(),col.blue(),0.2);
    att.SetVisibility(!isHidden);
    if(isSolid)
        att.SetForceSolid();
    else {
        att.SetForceWireframe();
    }
    logicVol->SetVisAttributes(att);
}
void SimpleObject::SetVisiblity(bool isVisible)
{
    isHidden=!isVisible;
    //SetVisProperties(objColor,!isHidden,isSolid);
}

void SimpleObject::ClearData()
{

}

void SimpleObject::FillData(const G4Step *step, ulong event_id)
{
    if(record_data && step->GetPreStepPoint()->GetTouchableHandle() ->GetVolume()->GetLogicalVolume()->GetName()==objName.toLatin1().data()){
        int columnId =0;
        if((filterIDs.contains(step->GetTrack()->GetDefinition()->GetPDGEncoding())) || (filterIDs.count()==0)){
            G4AnalysisManager* man = G4AnalysisManager::Instance();
            {
                man->FillNtupleDColumn(columnId++, event_id);
                man->FillNtupleDColumn(columnId++, step->GetTrack()->GetDefinition()->GetPDGEncoding());
                man->FillNtupleSColumn(columnId++, objName.toLatin1().data());
                man->FillNtupleDColumn(columnId++,step->GetTrack()->GetParentID());


                columnId =appendPositionParameters(columnId, step);
                columnId =appendTimeParameters(columnId,step);
                columnId =appendEnergyParameters(columnId,step);
                columnId =appendMomentumParamters(columnId,step);
                columnId =appendProcessName(columnId,step);
                man->AddNtupleRow(0);

            }
        }
    }
}


int SimpleObject::appendPositionParameters(int column_id, const G4Step *step)
{
    if(rec_pos){
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(column_id++,step->GetPreStepPoint()->GetPosition().x()*mm);
        man->FillNtupleDColumn(column_id++,step->GetPreStepPoint()->GetPosition().y()*mm);
        man->FillNtupleDColumn(column_id++,step->GetPreStepPoint()->GetPosition().z()*mm);
        man->FillNtupleDColumn(column_id++,step->GetDeltaPosition().x()*mm);
        man->FillNtupleDColumn(column_id++,step->GetDeltaPosition().y()*mm);
        man->FillNtupleDColumn(column_id++,step->GetDeltaPosition().z()*mm);
    }
    return column_id;

}

int SimpleObject::appendTimeParameters(int column_id,const G4Step* step)
{
    if(rec_time){
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(column_id++,step->GetDeltaTime()*ns);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetGlobalTime()*ns);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetProperTime()*ns);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetLocalTime()*ns);
    }
    return column_id;
}

int SimpleObject::appendEnergyParameters(int column_id,const G4Step* step)
{
    if(rec_energy){
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(column_id++,step->GetNonIonizingEnergyDeposit()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTotalEnergyDeposit()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetTotalEnergy()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetKineticEnergy()*keV);
    }
    return column_id;

}

int SimpleObject::appendMomentumParamters(int column_id,const G4Step* step)
{
    if(rec_mom){
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentum().x()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentum().y()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentum().z()*keV);
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentumDirection().x());
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentumDirection().y());
        man->FillNtupleDColumn(column_id++,step->GetTrack()->GetMomentumDirection().z());
    }
    return column_id;
}
int SimpleObject::appendProcessName(int column_id, const G4Step *step)
{
    if(rec_process){
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        const G4VProcess* proc = step->GetPreStepPoint()->GetProcessDefinedStep();

        if(proc!=NULL)
            man->FillNtupleSColumn(column_id++,proc->GetProcessName());
        else
            man->FillNtupleSColumn(column_id++,"Unknown");
    }
    return column_id;
}

