#include "SimpleObjectTableModel.h"
#include <QDebug>
SimpleObjectTableModel::SimpleObjectTableModel()
{
    obj = nullptr;
}

int SimpleObjectTableModel::rowCount(const QModelIndex &parent) const
{
    if(obj!=nullptr){
        return obj->GetParameterCount();
    }
    else
        return 0;
}
int SimpleObjectTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}
QVariant SimpleObjectTableModel::data(const QModelIndex &index, int role) const
{
    if(obj==nullptr) return QVariant();
    if (role == Qt::DisplayRole)
    {
        if(index.row()==0)
        {
            if(index.column()==0)
                return "Name";
            if(index.column()==1)
                return obj->GetObjectName();
        }
        if(index.row()==1)
        {
            if(index.column()==0)
                return "Material";
            if(index.column()==1)
                return obj->GetMaterial();
        }
        if(index.row()==2)
        {
            if(index.column()==0)
                return "Position (mm)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(obj->GetPosition().x()).arg(obj->GetPosition().y()).arg(obj->GetPosition().z());
        }
        if(index.row()==3)
        {
            if(index.column()==0)
                return "Rotation  (deg)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(obj->GetRotation().x()).arg(obj->GetRotation().y()).arg(obj->GetRotation().z());
        }
        if(index.row()==4)
        {
            if(index.column()==0)
                return "Mag. Field  (G)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(obj->GetMagField().x()).arg(obj->GetMagField().y()).arg(obj->GetMagField().z());

        }
        if(index.row()==5)
        {
            if(index.column()==0)
                return "Store data";
            if(index.column()==1){
                if(obj->storeData()==true){
                    return QString("%1").arg("Yes");
                }
                else {
                    return QString("%1").arg("No");
                }
            }

        }

        int ind =6;
        int ii=0;
        for (ii=0;ii<obj->GetDimensionNames().count();ii++) {

            if(index.row()==ind+ii)
            {
                if(index.column()==0)
                    return obj->GetDimensionNames().at(ii);
                if(index.column()==1)
                    return obj->GetDimension(ii);
            }
        }
//        if(obj->isMesh()){
//            if(index.row()==ind+ii)
//            {
//                if(index.column()==0)
//                    return "Bins";
//                if(index.column()==1)
//                    return QString("%1,%2,%3").arg(obj->GetBins().x()).arg(obj->GetBins().y()).arg(obj->GetBins().z());

//            }
//            if(index.row()==ind+ii+1)
//            {
//                if(index.column()==0)
//                    return "Meas. Qty";
//                if(index.column()==1)
//                    return obj->MeasurementQuantities().join(",");
//            }
//            if(index.row()==ind+ii+2)
//            {
//                if(index.column()==0)
//                    return "Filter";
//                if(index.column()==1)
//                    return obj->MeasurementFilters().join(",");
//            }
//        }

        return QVariant();

    }

    return QVariant();
}

QVariant SimpleObjectTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Property");
            case 1:
                return QString("value");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags SimpleObjectTableModel::flags(const QModelIndex &index) const
{
    // if(obj==nullptr) return QVariant();

    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(index.column()==1)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else {
        return QAbstractItemModel::flags(index);
    }
}

// converts a string of a format x,y,z to QVector3D
// if only one value is input like x then it sets x=y=z
G4ThreeVector SimpleObjectTableModel::GetVector(QString string)
{
    G4ThreeVector v;
    QStringList s  = string.split(",");
    if(s.count()==3)
        v = G4ThreeVector(s.at(0).toDouble(),s.at(1).toDouble(),s.at(2).toDouble());
    else if(s.count()==1)
        v = G4ThreeVector(s.at(0).toDouble(),s.at(0).toDouble(),s.at(0).toDouble());

    return v;

}

bool SimpleObjectTableModel::setData(const QModelIndex &index,const QVariant &value, int role)
{

    if(value.toString()=="")
        return false;
    if (index.isValid() && role == Qt::EditRole) {

        if(index.row()==1)
        {
            if(materials.contains(value.toString()))
                obj->SetMaterial(value.toString());
        }
        else  if(index.row()==2)
        {
            obj->SetPosition(GetVector(value.toString()));
        }
        else   if(index.row()==3)
        {
            obj->SetRotation(GetVector(value.toString()));
        }
        else  if(index.row()==4)
        {
            obj->SetMagField(GetVector(value.toString()));
        }
        else  if(index.row()==5)
        {

            obj->SetDataStorage(value.toBool());
        }
        int ind =6;
        int ii=0;
        for (ii=0;ii<obj->GetDimensionNames().count();ii++) {

            if(index.row()==ind+ii)
            {
                obj->SetDimension(ii,value.toDouble());
            }
        }

//        if(obj->isMesh()){
//            if(index.row()==ind+ii)
//            {
//                obj->SetBins(GetVector(value.toString()));

//            }
//            if(index.row()==ind+ii+1)
//            {
//                obj->AppendMeasurementQuantity(GetStringList(value.toString()));
//            }
//            if(index.row()==ind+ii+2)
//            {
//                obj->AppendMeasurementQuantityFilter(GetStringList(value.toString()));
//            }
//        }

        emit dataChanged(index, index);
        emit objectUpdated(obj,obj_index);
        return true;
    }
    return false;
}
