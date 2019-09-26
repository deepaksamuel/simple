// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleMeshTableModel.h"

SimpleMeshTableModel::SimpleMeshTableModel()
{
    mesh = nullptr;
}

int SimpleMeshTableModel::rowCount(const QModelIndex &parent) const
{
    if(mesh!=nullptr){
        return mesh->get_parameter_count();
    }
    else {
        return 0;
    }
}

int SimpleMeshTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SimpleMeshTableModel::data(const QModelIndex &index, int role) const
{
    if(mesh==nullptr) return QVariant();
    if (role == Qt::DisplayRole)
    {
        if(index.row()==0)
        {
            if(index.column()==0)
                return "Name";
            if(index.column()==1)
                return mesh->name();
        }
        if(index.row()==1)
        {
            if(index.column()==0)
                return "Position (mm)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(mesh->GetPosition().x()).arg(mesh->GetPosition().y()).arg(mesh->GetPosition().z());

        }
        if(index.row()==2)
        {
            if(index.column()==0)
                return "Rotation  (deg)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(mesh->GetRotation().x()).arg(mesh->GetRotation().y()).arg(mesh->GetRotation().z());
        }
        if(index.row()==3)
        {
            if(index.column()==0)
                return "Bins (x,y,z)";
            if(index.column()==1)
                return QString("%1,%2,%3").arg(mesh->GetBins().x()).arg(mesh->GetBins().y()).arg(mesh->GetBins().z());

        }
        if(index.row()==4)
        {
            if(index.column()==0)
                return "Measurement";
            if(index.column()==1){
                return mesh->measurement();
            }

        }
        if(index.row()==5)
        {
            if(index.column()==0)
                return "Filter";
            if(index.column()==1){
                return mesh->filters();
            }

        }

        int ind =6;
        int ii=0;
        for (ii=0;ii<mesh->dim_names().count();ii++) {

            if(index.row()==ind+ii)
            {
                if(index.column()==0)
                    return mesh->dim_names().at(ii);
                if(index.column()==1)
                    return mesh->get_dims().at(ii);
            }
        }

        return QVariant();

    }

    return QVariant();
}

QVariant SimpleMeshTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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


Qt::ItemFlags SimpleMeshTableModel::flags(const QModelIndex &index) const
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
G4ThreeVector SimpleMeshTableModel::GetVector(QString string)
{
    G4ThreeVector v;
    QStringList s  = string.split(",");
    if(s.count()==3)
        v = G4ThreeVector(s.at(0).toDouble(),s.at(1).toDouble(),s.at(2).toDouble());
    else if(s.count()==1)
        v = G4ThreeVector(s.at(0).toDouble(),s.at(0).toDouble(),s.at(0).toDouble());

    return v;

}

bool SimpleMeshTableModel::setData(const QModelIndex &index,const QVariant &value, int role)
{
    if(mesh==nullptr) return true;
    if(value.toString()=="")
        return false;
    if (index.isValid() && role == Qt::EditRole) {

        if(index.row()==1)
        {
            mesh->SetPosition(GetVector(value.toString()));
        }
        else   if(index.row()==2)
        {
            mesh->SetRotation(GetVector(value.toString()));
        }
        else  if(index.row()==3)
        {
            mesh->setBins(GetVector(value.toString()));
        }
        else  if(index.row()==4)
        {
            mesh->set_scoring_qty(value.toString());
        }
        else  if(index.row()==5)
        {
            if(value!=""){
                if(value.toString().contains(","))
                {
                    QStringList s= value.toString().split(",");
                    mesh->set_scoring_filter(s);
                }
                else {
                    mesh->append_scoring_filter(value.toString());
                }
            }
        }

        int ind =6;
        int ii=0;
        for (ii=0;ii<mesh->get_dims().count();ii++) {

            if(index.row()==ind+ii)
            {
                mesh->setDims(ii,value.toDouble());
            }
        }



        emit dataChanged(index, index);
        return true;
    }
    return false;
}
