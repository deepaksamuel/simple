#ifndef SIMPLEOBJECTTABLEMODEL_H
#define SIMPLEOBJECTTABLEMODEL_H

#include <QAbstractTableModel>
#include <SimpleObject.h>

class SimpleObjectTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SimpleObjectTableModel();
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index,const QVariant &value, int role);
    G4ThreeVector GetVector(QString string);
    QStringList GetStringList(QString string, QString sep="'"){return string.split(sep);}
    void setObject(SimpleObject* o, int index){obj=o; obj_index=index;emit layoutChanged();}
    void setMaterialList(QStringList l){materials=l;}
    SimpleObject *obj;
    int obj_index; // this contains the original index of the object in the main QList
    QStringList materials;

signals:
    void objectUpdated(SimpleObject *obj, int newValue);
};

#endif // SIMPLEOBJECTTABLEMODEL_H
