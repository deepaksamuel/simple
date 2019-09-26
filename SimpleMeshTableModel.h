#ifndef SIMPLEMESHTABLEMODEL_H
#define SIMPLEMESHTABLEMODEL_H
// File modified by Deepak Samuel on 25 Sep 2019

#include <QAbstractTableModel>
#include <SimpleMesh.h>

class SimpleMeshTableModel : public QAbstractTableModel
{
     Q_OBJECT
public:
    SimpleMeshTableModel();
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index,const QVariant &value, int role);
    void setMesh(SimpleMesh* m){mesh=m; emit layoutChanged();}
    G4ThreeVector GetVector(QString s);
private:
    SimpleMesh* mesh;

};

#endif // SIMPLEMESHTABLEMODEL_H
