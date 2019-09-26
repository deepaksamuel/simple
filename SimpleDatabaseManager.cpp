// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleDatabaseManager.h"

SimpleDatabaseManager::SimpleDatabaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    activeDb = "db.sqlite";
    db.setDatabaseName(activeDb);
    if (!db.open())
    {
        qDebug() << "Could not open database file:";
        qDebug() << db.lastError();
        return;
    }

}


QSqlDatabase SimpleDatabaseManager::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}


QStringList SimpleDatabaseManager::GetListOfShapes()
{
    QStringList shapes;
    QSqlQuery query;
    query.exec("SELECT * FROM shapes");

    while (query.next()) {
        QString shape = query.value(0).toString().trimmed();
        shapes.append(shape);

        //int salary = query.value(1).toInt();
        //qDebug() << name << salary;
    }
    return shapes;
}



QStringList SimpleDatabaseManager::GetListOfEnvVars()
{
    QStringList dsnames;
    QSqlQuery query;
    query.exec("SELECT * FROM env_vars");

    while (query.next()) {
        QString ds = query.value(0).toString().trimmed();
        dsnames.append(ds);

        //int salary = query.value(1).toInt();
        //qDebug() << name << salary;
    }
    return dsnames;
}

QStringList SimpleDatabaseManager::GetListOfEnvVarValues()
{
    QStringList dspaths;
    QSqlQuery query;
    query.exec("SELECT * FROM env_vars");

    while (query.next()) {
        QString dspath = query.value(1).toString().trimmed();
        dspaths.append(dspath);

        //int salary = query.value(1).toInt();
        //qDebug() << name << salary;
    }
    return dspaths;
}

void SimpleDatabaseManager::SetEnvVar(QString name, QString path)
{
    QSqlQuery query;
    QString sqlcommand = QString("UPDATE env_vars SET env_var='%1' WHERE value='%2'").arg(path).arg(name);
    qDebug()<<sqlcommand;
    query.exec(sqlcommand);

}

void SimpleDatabaseManager::InsertNewMaterial(QString name, QString formula)
{
    QSqlQuery query;
    QString sqlcommand = QString("INSERT INTO materials VALUES('%1','%2','')").arg(name).arg(formula);
    qDebug()<<sqlcommand;
    query.exec(sqlcommand);
}

void SimpleDatabaseManager::InsertMaterialProperty(QString name, QString formula, QString property)
{
    QSqlQuery query;
    QString sqlcommand = QString("INSERT INTO materials VALUES('%1','%2','%3')").arg(name).arg(formula).arg(property);
    qDebug()<<sqlcommand;
    query.exec(sqlcommand);
}

void SimpleDatabaseManager::UpdateMaterialProperty(QString name, QString property)
{
    QSqlQuery query;
    QString sqlcommand = QString("UPDATE materials SET property='%2' WHERE name='%1'").arg(name).arg(property);
    qDebug()<<sqlcommand;
    query.exec(sqlcommand);
}

QStringList SimpleDatabaseManager::GetListOfRefPhysics()
{
    QStringList refphys;
    QSqlQuery query;
    query.exec("SELECT * FROM physics_lists");

    while (query.next()) {
        QString phys = query.value(0).toString().trimmed();
        refphys.append(phys);

        //int salary = query.value(1).toInt();
        //qDebug() << name << salary;
    }
    return refphys;
}

QStringList SimpleDatabaseManager::GetListOfRefPhysicsHints()
{
    QStringList refphyshints;
    QSqlQuery query;
    query.exec("SELECT * FROM physics_lists");

    while (query.next()) {
        QString physhint = query.value(1).toString().trimmed();
        refphyshints.append(physhint);

        //int salary = query.value(1).toInt();
        //qDebug() << name << salary;
    }
    return refphyshints;
}

QStringList SimpleDatabaseManager::GetListOfMaterials()
{
    QStringList materials;
    QSqlQuery query;
    query.exec("SELECT * FROM materials");

    while (query.next()) {
        QString material = query.value(0).toString().trimmed();
        materials.append(material);
    }
    return materials;
}

QStringList SimpleDatabaseManager::GetListOfElements()
{
    QStringList elements;
    QSqlQuery query;
    query.exec("SELECT Element FROM elements");

    while (query.next()) {
        QString element = query.value(0).toString().trimmed();
        elements.append(element);
    }
    return elements;
}

QStringList SimpleDatabaseManager::GetListOfElementSymbols()
{
    QStringList symbols;
    QSqlQuery query;
    query.exec("SELECT Symbol FROM elements");

    while (query.next()) {
        QString symbol = query.value(0).toString().trimmed();
        symbols.append(symbol);
    }
    return symbols;
}


QStringList SimpleDatabaseManager::GetListOfAtomicMass()
{
    QStringList masses;
    QSqlQuery query;
    query.exec("SELECT AtomicMass FROM elements");

    while (query.next()) {
        QString mass = query.value(0).toString().trimmed();
        masses.append(mass);
    }
    return masses;
}

QStringList SimpleDatabaseManager::GetListOfZ()
{
    QStringList zs;
    QSqlQuery query;
    query.exec("SELECT NumberofProtons FROM elements");

    while (query.next()) {
        QString z = query.value(0).toString().trimmed();
        zs.append(z);
    }
    return zs;
}




QString SimpleDatabaseManager::GetMaterialFormula(QString custom_material)
{
    QString formula;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM materials WHERE name='%1'").arg(custom_material);
    query.exec(qry);
    query.first();
    return query.value("formula").toString().trimmed();
}

QString SimpleDatabaseManager::GetMaterialProperty(QString name)
{
    QString formula;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM materials WHERE name='%1'").arg(name);
    query.exec(qry);
    query.first();
    return query.value("property").toString().trimmed();
}

QStringList SimpleDatabaseManager::GetMaterialPropertyList()
{
    QString formula;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM material_prop_names WHERE property_name='properties'");
    query.exec(qry);
    query.first();
    return query.value("list").toString().trimmed().split(",");
}

QStringList SimpleDatabaseManager::GetMaterialConstPropertyList()
{
    QString formula;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM material_prop_names WHERE property_name='constant_properties'");
    query.exec(qry);
    query.first();
    return query.value("list").toString().trimmed().split(",");
}


QStringList SimpleDatabaseManager::GetListOfSizeParameters(QString shapeName)
{
    QStringList params;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM shapes WHERE name='%1'").arg(shapeName);
    query.exec(qry);

    int jj=0;
    while (query.next()) {
        int  nParams= query.value(1).toInt();
        for(int ii=0;ii<nParams;ii++){
            params.append(query.value(2+jj).toString().trimmed());
            jj=jj+2;
        }
    }
    return params;
}

QList <float> SimpleDatabaseManager::GetListOfDefaultSizeParameters(QString shapeName)
{
    QList <float> params;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM shapes WHERE name='%1'").arg(shapeName);
    query.exec(qry);
    int jj=0;
    while (query.next()) {
        int  nParams= query.value(1).toInt();
        for(int ii=0;ii<nParams;ii++){
            params.append(query.value(3+jj).toFloat());
            jj=jj+2;
        }
    }
    return params;
}

QFormLayout* SimpleDatabaseManager::GetObjectSizeParametersForm(QString shapeName)
{

    QStringList parNames = GetListOfSizeParameters(shapeName);
    QList <float> defParValues = GetListOfDefaultSizeParameters(shapeName);

    QFormLayout *layout = new QFormLayout;
    for(int i=0;i<parNames.count();i++){
    QDoubleSpinBox *val = new QDoubleSpinBox();
    val->setMinimum(-99999999);
    val->setMaximum(99999999);
    val->setValue(defParValues.at(i));
    //val->setObjectName(parNames.at(i));
   // qDebug()<<defParValues.at(i);
    layout->addRow(new QLabel(parNames.at(i)), val);

    }//layout->addRow(button2, lineEdit2);
    //layout->addRow(button3, lineEdit3);
   return layout;
}

QStringList SimpleDatabaseManager::GetListOfPositionParameters()
{
    QStringList params;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM shape_defaults ");
    query.exec(qry);

    int jj=0;
    while (query.next()) {
        int  nParams= query.value(1).toInt();
        for(int ii=0;ii<nParams;ii++){
            params.append(query.value(2+jj).toString().trimmed());
            jj=jj+2;
        }
    }
    return params;
}


QList <float> SimpleDatabaseManager::GetListOfDefaultPositionParameters()
{
    QList <float> params;
    QSqlQuery query;
    QString qry = QString("SELECT * FROM shape_defaults ");
    query.exec(qry);
    int jj=0;
    while (query.next()) {
        int  nParams= query.value(1).toInt();
        for(int ii=0;ii<nParams;ii++){
            params.append(query.value(3+jj).toFloat());
            jj=jj+2;

        }
    }
    return params;
}

QFormLayout* SimpleDatabaseManager::GetObjectPositionParametersForm()
{

    QStringList parNames = GetListOfPositionParameters();
    QList <float> defParValues = GetListOfDefaultPositionParameters();

    QFormLayout *layout = new QFormLayout;
    for(int i=0;i<parNames.count();i++){
    QDoubleSpinBox *val = new QDoubleSpinBox();
    val->setMinimum(-99999999);
    val->setMaximum(99999999);
    val->setValue(defParValues.at(i));
    //qDebug()<<defParValues.at(i);
    layout->addRow(new QLabel(parNames.at(i)), val);
    }

return layout;
}


QStringList SimpleDatabaseManager::Query(QString qry)
{

    QSqlQuery query;
    query.exec("SELECT * FROM shapes");

    while (query.next()) {
        QString name = query.value(0).toString();
        int salary = query.value(1).toInt();
        qDebug() << name << salary;
    }
}
