#ifndef SIMPLEDATABASEMANAGER_H
#define SIMPLEDATABASEMANAGER_H
// File modified by Deepak Samuel on 25 Sep 2019
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QFormLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSqlDatabase>
#include <QSqlTableModel>

class SimpleDatabaseManager
{
public:
    SimpleDatabaseManager();

    QSqlDatabase db;
    QStringList Query(QString qry);
    QStringList GetListOfShapes();
    QStringList GetListOfMaterials();
    QStringList GetListOfElements();
    QStringList GetListOfElementSymbols();
    QStringList GetListOfAtomicMass();
    QStringList GetListOfZ();
    QStringList GetListOfRefPhysics();
    QStringList GetListOfRefPhysicsHints();
    QStringList GetListOfSizeParameters(QString shapeName);
    QList<float> GetListOfDefaultSizeParameters(QString shapeName);
    QStringList GetListOfEnvVars();
    QStringList GetListOfEnvVarValues();
    void SetEnvVar(QString name, QString path);
    QSqlDatabase currentDatabase() const;
    QStringList GetListOfPositionParameters();
    QList<float> GetListOfDefaultPositionParameters();
    QString activeDb;
    QFormLayout* GetObjectSizeParametersForm(QString shapeName);
    QFormLayout* GetObjectPositionParametersForm();
    void InsertNewMaterial(QString name, QString formula);
    QString GetMaterialFormula(QString custom_material); // used for custom materials
    QStringList GetMaterialPropertyList();
    QStringList GetMaterialConstPropertyList();
    void InsertMaterialProperty(QString name, QString formula, QString property);
    void UpdateMaterialProperty(QString name, QString property);
    QString GetMaterialProperty(QString name);
};


// the following is required for database table editor
class DBTableModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit DBTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase())
        : QSqlTableModel(parent, db) {}

    QVariant data(const QModelIndex &idx, int role) const override
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::gray));
        return QSqlTableModel::data(idx, role);
    }
};

#endif // SIMPLEDATABASEMANAGER_H
