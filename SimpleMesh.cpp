// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleMesh.h"
#include <QDebug>
#include <G4UImanager.hh>
#include <QFile>
#include <TTree.h>
#include <TFile.h>

int SimpleMesh::mesh_id=0;
SimpleMesh::SimpleMesh(QString type, QString qty, bool chargeParticle, bool neutralParticle, bool pid, QString p_id, bool ke, double min_ke, double max_ke)
{
    mesh_type = type;
    mesh_name = QString("%1-Mesh-%2").arg(type).arg(mesh_id,3,10, QChar('0'));
    scoring_qty = get_scoring_string(qty);
    scoring_filter  = GetScoringFilterCommands(chargeParticle, neutralParticle,pid, p_id, ke,  min_ke,  max_ke);
    // init all values to 10
    dims.append(10);
    dims.append(10);
    dims.append(10);

    bins.set(1,1,1);

    jsonObject["mesh_id"] = mesh_id;
    jsonObject["mesh_name"] = mesh_name;
    jsonObject["mesh_type"] = mesh_type;

    jsonObject["scoring_qty"] = scoring_qty;
    jsonObject["scoring_filter"] = scoring_filter.join(",");
    int ii=0;
    foreach(double dim, dims){
        jsonObject[QString("dim-%1").arg(ii++)] = dim;
    }
    jsonObject["binsx"] = bins.x();
    jsonObject["binsy"] = bins.y();
    jsonObject["binsz"] = bins.z();

    jsonObject["posx"] = position.x();
    jsonObject["posy"] = position.y();
    jsonObject["posz"] = position.z();

    jsonObject["rotx"] = rotation.x();
    jsonObject["roty"] = rotation.y();
    jsonObject["rotz"] = rotation.z();
    mesh_id++;


}

SimpleMesh::SimpleMesh(QJsonObject jo)
{
    mesh_id=jo["mesh_id"].toInt();
    mesh_name=jo["mesh_name"].toString();
    mesh_type=jo["mesh_type"].toString();

    scoring_qty=jo["scoring_qty"].toString();
    scoring_filter=jo["scoring_filter"].toString().split(",");

    bins.set(jo["binsx"].toInt(),jo["binsy"].toInt(),jo["binsz"].toInt());
    position.set(jo["posx"].toDouble(),jo["posy"].toDouble(),jo["posz"].toDouble());
    rotation.set(jo["rotx"].toDouble(),jo["roty"].toDouble(),jo["rotz"].toDouble());


   for(int ii=0;ii<3;ii++)
        dims.append(jo[QString("dim-%1").arg(ii)].toDouble());

   jsonObject =jo;
   mesh_id++;

}
SimpleMesh::SimpleMesh(QString type, QString qty, QString filter)
{
    //    mesh_type = type;
    //    mesh_name = QString("%1-%2").arg(type).arg(mesh_id++,3,10, QChar('0'));
    //    scoring_qty =qty;
    //    if(filter!="")
    //    scoring_filter.append(filter);
}

void SimpleMesh::writeToFile(QString file_prefix)
{

    //if(fileName==""){
    mesh_file_name = file_prefix+scoring_qty+"-"+mesh_name;
    //}
    QString command = QString("/score/dumpAllQuantitiesToFile %1 %2").arg(mesh_name).arg(mesh_file_name); // this writes to csv format
    execute(command);


    // write to root format

    //# mesh name: Box-Mesh-000
    //# primitive scorer name: energyDepositBox-Mesh-000
    //# iX, iY, iZ, total(value) [MeV], total(val^2), entry
    //0,0,0,0.09558370555881757,0.0001435852588928847,100

    QFile file(mesh_file_name);
    QStringList lineData;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Error reading mesh file "<<mesh_file_name;
    }

    TFile *rootfile = new TFile(mesh_file_name.append(".root").toLatin1().data(),"RECREATE");
    TTree *tree = new TTree("mesh_data", "Converted using Simple");
    int binx, biny, binz, entries;
    double value, variance;
    tree->Branch("binx", &binx);
    tree->Branch("biny", &biny);
    tree->Branch("binz", &binz);
    tree->Branch("entries", &entries);
    tree->Branch("var", &variance); // not sure what this column contains
    int readLines=0;
    while(!file.atEnd()){
        //return QString(file.readLine()).split(QRegExp(delimiter));
        lineData = QString(file.readLine()).split(QRegExp(","));
        if(readLines==2){
            if(lineData.count()==6){
                QString s = lineData.at(3);
                QString s1 = s.replace("(","-").replace("[","-").remove(QRegExp("[()].")).trimmed();
                tree->Branch(s1.remove("]").remove("-").toLatin1().data(),&value);
                //tree->Branch("val",&value);
            }
            else {
                qDebug()<<"Error: Mesh file "<<mesh_file_name<<" does not contain 6 headers...";
                break;
            }
        }
        if(readLines>=3){
            if(lineData.count()!=6){
                qDebug()<<"Error: Mesh file "<<mesh_file_name<<" does not contain 6 columns...";
                break;
            }

            binx = lineData.at(0).toInt();
            biny = lineData.at(1).toInt();
            binz = lineData.at(2).toInt();
            value =lineData.at(3).toDouble();
            variance =lineData.at(4).toDouble(); // dont know what this column contains
            entries =lineData.at(5).toInt();
            tree->Fill();


        }
        readLines++;

    }
    rootfile->Write();
    rootfile->Close();
}

void SimpleMesh::execute(QString command)
{
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand(command.toLatin1().data());
    qDebug()<<command;
}

void SimpleMesh::execute(QStringList commands)
{
    foreach(QString command, commands){
        execute(command);
    }
}

void SimpleMesh::createMesh()
{
    execute(scoringCommandList());
}

QStringList SimpleMesh::scoringCommandList()
{
    QStringList scoringCommands;
    QString createMesh_command;
    QString meshSize_command;
    if(mesh_type=="Box"){
        createMesh_command = QString("/score/create/boxMesh %1").arg(mesh_name); // mesh name
        meshSize_command = QString("/score/mesh/boxSize %1 %2 %3 mm").arg(dims.at(0)/2).arg(dims.at(1)/2).arg(dims.at(2)/2); // size
    }
    else{
        createMesh_command = QString("/score/create/cylinderMesh %1").arg(mesh_name);
        meshSize_command = QString("/score/mesh/cylinderSize %1 %2 mm").arg(dims.at(1)/2).arg(dims.at(0)/2); //rad, len
    }

    QString meshPos_command = QString("/score/mesh/translate/xyz %1 %2 %3 mm").arg(position.x()).arg(position.y()).arg(position.z()); // location

    QString meshrotx_command = QString("/score/mesh/rotate/rotateX %1 deg").arg(rotation.x()); // rotation
    QString meshroty_command = QString("/score/mesh/rotate/rotateY %1 deg").arg(rotation.y());
    QString meshrotz_command = QString("/score/mesh/rotate/rotateZ %1 deg").arg(rotation.z());


    QString meshBin_command = QString("/score/mesh/nBin %1 %2 %3").arg(bins.x()).arg(bins.y()).arg(bins.z()); //bins

    QString qty_command = QString("/score/quantity/%1 %1%2").arg(scoring_qty).arg(mesh_name); // quantity

    scoringCommands.append(createMesh_command);
    scoringCommands.append(meshSize_command);
    scoringCommands.append(meshPos_command);
    scoringCommands.append(meshrotx_command);
    scoringCommands.append(meshroty_command);
    scoringCommands.append(meshrotz_command);
    scoringCommands.append(meshBin_command);
    scoringCommands.append(qty_command);

    if(scoring_filter.count()>0)
        scoringCommands.append(scoring_filter); //filters

    scoringCommands.append("/score/close"); // close
    qDebug()<<scoringCommands;
    return scoringCommands;

}
void SimpleMesh::plotProjection()
{
    execute(plotProjectionCommands());
}

void SimpleMesh::plotColumn(QString plane, int col)
{
    execute(plotColumnCommands(plane,col));
}
QStringList SimpleMesh::plotProjectionCommands()
{
    //    /score/colorMap/floatMinMax
    //    /score/dumpAllQuantitiesToFile Box-Mesh-000 000.txt
    //    /score/drawProjection Box-Mesh-000 energyDepositBox-Mesh-000

    QStringList commands;
    commands.append("/score/colorMap/floatMinMax");
    QString projectionCommand=  QString("/score/drawProjection %1 %2%1").arg(mesh_name).arg(scoring_qty);
    commands.append(projectionCommand);

    return  commands;
}

QStringList SimpleMesh::plotColumnCommands(QString plane, int column)
{
    //    /score/colorMap/floatMinMax
    //    /score/dumpAllQuantitiesToFile Box-Mesh-000 000.txt
    //    /score/drawProjection Box-Mesh-000 energyDepositBox-Mesh-000

    QStringList commands;
    int p =0;
    if(plane=="Y") p=1;
    if(plane=="Z") p=2;
    commands.append("/score/colorMap/floatMinMax");
    QString projectionCommand=  QString("/score/drawColumn %1 %2%1 %3 %4").arg(mesh_name).arg(scoring_qty).arg(p).arg(column);
    commands.append(projectionCommand);

    return  commands;
}




QStringList SimpleMesh::GetScoringFilterCommands(bool chargedParticle, bool neutralParticle, bool pid, QString p_id, bool ke, double min_ke, double max_ke)
{
    QStringList commands;
    QString filterCommand ;
    if(!chargedParticle && !neutralParticle && !pid && !ke)
    {
        return commands;
    }
    if(chargedParticle){
        filterCommand = QString("/score/filter/charged chargedFilter");
        commands.append(filterCommand);
    }
    if(neutralParticle){
        filterCommand = QString("/score/filter/neutral neutralFilter");
        commands.append(filterCommand);
    }
    if(pid && ke)
    {
        filterCommand = QString("/score/filter/particleWithKineticEnergy pkeFilter %1 %2 MeV %3").arg(min_ke).arg(min_ke).arg(p_id);
        commands.append(filterCommand);
        return commands;
    }
    if(pid && !ke){
        filterCommand = QString("/score/filter/particle pidFilter %1").arg(pid);
        commands.append(filterCommand);
        return commands;
    }
    if(!pid && ke){
        filterCommand = QString("/score/filter/kineticEnergy keFilter %2 %3 MeV").arg(min_ke).arg(max_ke);
        commands.append(filterCommand);
        return commands;
    }
    return commands;
}

// when a beam on is made when an existing mesh is present, it complains that a mesh is already existing.
// This is a workaround by renaming the mesh with another unique name.
void SimpleMesh::makeUnique()
{
    mesh_name = QString("%1-Mesh-%2").arg(mesh_type).arg(mesh_id,3,10, QChar('0'));
    jsonObject["mesh_id"] = mesh_id;
    jsonObject["mesh_name"] = mesh_name;
    execute(scoringCommandList());
    mesh_id++;
}
