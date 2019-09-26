// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleMaterialPropertyBuilder.h"
#include "ui_SimpleMaterialPropertyBuilder.h"
#include <QMessageBox>
#include <G4SystemOfUnits.hh>
SimpleMaterialPropertyBuilder *SimpleMaterialPropertyBuilder::instance = nullptr;

SimpleMaterialPropertyBuilder::SimpleMaterialPropertyBuilder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleMaterialPropertyBuilder)
{
    ui->setupUi(this);
    setWindowTitle("Add material property");
}


SimpleMaterialPropertyBuilder::~SimpleMaterialPropertyBuilder()
{
    delete ui;
}

void SimpleMaterialPropertyBuilder::SetDataBaseManager(SimpleDatabaseManager *dbM)
{
    dbManager = dbM;
    ui->g4_materials->insertItems(0,dbManager->GetListOfMaterials());
    ui->g4_property->insertItems(0,dbManager->GetMaterialPropertyList());
    ui->g4_const_property->insertItems(0,dbManager->GetMaterialConstPropertyList());
}

void SimpleMaterialPropertyBuilder::ShowUI()
{
    ui->g4_materials->insertItems(0,dbManager->GetListOfMaterials());
    ui->g4_property->insertItems(0,dbManager->GetMaterialPropertyList());
    ui->g4_const_property->insertItems(0,dbManager->GetMaterialConstPropertyList());
    ui->commands->clear();
    show();
}

void SimpleMaterialPropertyBuilder::on_addProperty_clicked()
{

    if(ui->g4_prop_value->text().split(",").count() != ui->g4_prop_energies->text().split(",").count()){
        QMessageBox::warning(this,"Error in parameters" , "Please enter equal number of energies and corresponding values");
        return;
    }

    if(ui->g4_prop_value->text()=="" || ui->g4_prop_energies->text()=="" ){
        QMessageBox::warning(this,"Error in parameters" , "Please enter equal number of energies and corresponding values");
        return;
    }

    commands.append(QString("(property,%3,(%1),(%2))").arg(ui->g4_prop_energies->text()).arg(ui->g4_prop_value->text()).arg(ui->g4_property->currentText()));
    ui->commands->setPlainText(commands.join("\n"));

}

void SimpleMaterialPropertyBuilder::on_add_const_property_clicked()
{
    commands.append(QString("(const_property,%2,%1)").arg(ui->g4_const_prop_value->value()).arg(ui->g4_const_property->currentText()));
    ui->commands->setPlainText(commands.join("\n"));
}

void SimpleMaterialPropertyBuilder::on_done_clicked()
{
    commands.clear();

    if(ui->mat_name->text().contains("G4_")){
        QMessageBox::warning(this,"Invalid material name" , "New material names should not contain G4_");
        return;
    }

    QString commands = ui->commands->toPlainText().split("\n").join(";");
    if(ui->g4_materials->currentText().contains("G4_")){ // if it is a G4 material, do not disturb it. Create a material with a new name.
        if(ui->mat_name->text()==""){
            QMessageBox::warning(this,"Empty material name" , "Please enter a new material name");
            return;
        }
        dbManager->InsertMaterialProperty(ui->mat_name->text().trimmed().remove(" "),ui->g4_materials->currentText(),ui->g4_materials->currentText()+";"+commands);
    }
    else{ // if it is a user created material, then update its property
        dbManager->UpdateMaterialProperty(ui->g4_materials->currentText(),ui->g4_materials->currentText()+";"+commands);
    }

    hide();

}

void SimpleMaterialPropertyBuilder::on_g4_materials_currentIndexChanged(const QString &arg1)
{
    if(arg1.contains("G4_")){
        ui->mat_name->show();
        ui->new_material_label->show();
    }
    else {
        ui->mat_name->hide();
        ui->new_material_label->hide();
    }
}

void SimpleMaterialPropertyBuilder::SetMaterialProperties(G4Material *mat, QString props)
{
    if(mat==nullptr) return;

    G4MaterialPropertiesTable* matTable = new G4MaterialPropertiesTable();

    // string is of the form:
    //G4_WATER;(property,FASTCOMPONENT,(1,2),(2,3));(const_property,SCINTILLATIONYIELD,0);(property,FASTCOMPONENT,(1,2),(2,3))
    //split first with ;
    //first string is the material
    // the rest are all properties
    QStringList s = props.split(";");
    QString name = s.at(0);

    qDebug()<<"Setting material property for "<<s.at(0)<<" using user string"<<props;

    for(int ii=0; ii<s.count();ii++){
        if(ii>0){
            //strings are now in one of the following formats
            //format 1: (property,FASTCOMPONENT,(1,2),(2,3))
            //format 2: (const_property,SCINTILLATIONYIELD,0)

            // remove brackets
            QString s1 = s.at(ii);
            s1 = s1.remove("(").remove(")");
            //strings are now in one of the following formats
            //format 1: property,FASTCOMPONENT,1,2,2,3
            //format 2: const_property,SCINTILLATIONYIELD,0

            // split using ,

            QStringList sl = s1.split(",");

            if(sl.at(0)=="property"){
                //property,FASTCOMPONENT,1,2,2,3
                QString propType = sl.at(1);
                int npars = (sl.count()-2)/2;
                double *e = new  double [npars];
                double *val= new double [npars];
                for(int ii=0;ii<npars;ii++){
                    e[ii] = sl.at(2+ii).toDouble()*eV;
                    if(propType=="ABSLENGTH"){
                        val[ii] = sl.at(2+ii+npars).toDouble()*cm;
                        qDebug()<<"Adding "<<propType<<": "<<sl.at(2+ii+npars).toDouble()<<" (cm)"<< " at energy "<<sl.at(2+ii).toDouble()<<" (eV)";
                    }
                    else {

                        val[ii] = sl.at(2+ii+npars).toDouble();
                        qDebug()<<"Adding "<<propType<<": "<<val[ii]<<" (--)"<< " at energy "<<sl.at(2+ii).toDouble()<<" (eV)";
                    }
                }
                matTable->AddProperty(propType.toLatin1().data(),e,val,npars);
            }

            if(sl.at(0)=="const_property"){
                //const_property,SCINTILLATIONYIELD,0
                QString propType = sl.at(1);
                if(propType=="BIRKSCONSTANT"){
                    mat->GetIonisation()->SetBirksConstant(sl.at(2).toDouble()*mm/MeV);
                    qDebug()<<"Adding "<<propType<<": "<<sl.at(2)<<" (mm/MeV)";
                }
                else{
                    if(propType=="SCINTILLATIONYIELD"){
                        matTable->AddConstProperty(propType.toLatin1().data(),sl.at(2).toDouble()*MeV);
                        qDebug()<<"Adding "<<propType<<": "<<sl.at(2)<<" (MeV)";
                    }
                    if(propType=="FASTIMECONSTANT"){
                        matTable->AddConstProperty(propType.toLatin1().data(),sl.at(2).toDouble()*ns);
                        qDebug()<<"Adding "<<propType<<": "<<sl.at(2)<<" (ns)";
                    }
                    if(propType=="SLOWIMECONSTANT"){
                        matTable->AddConstProperty(propType.toLatin1().data(),sl.at(2).toDouble()*ns);
                        qDebug()<<"Adding "<<propType<<": "<<sl.at(2)<<" (ns)";
                    }
                    else {
                        matTable->AddConstProperty(propType.toLatin1().data(),sl.at(2).toDouble());
                        qDebug()<<"Adding "<<propType<<": "<<sl.at(2)<<" (--)";
                    }


                }

            }

        }

    }
    qDebug()<<"Setting material table...";
    mat->SetMaterialPropertiesTable(matTable);
    qDebug()<<"Dumping table properties...";
    qDebug()<<matTable->GetPropertyIndex("SPECULARSPIKECONSTANT")<<mat->GetMaterialPropertiesTable()->GetPropertyIndex("SPECULARSPIKECONSTANT");
    matTable->DumpTable();
}

void SimpleMaterialPropertyBuilder::on_g4_const_property_currentTextChanged(const QString &arg1)
{

    ui->const_prop_unit->setText("--");
    if(arg1=="SCINTILLATIONYIELD")
        ui->const_prop_unit->setText("MeV");
    if(arg1=="FASTTIMECONSTANT")
        ui->const_prop_unit->setText("ns");
    if(arg1=="SLOWTIMECONSTANT")
        ui->const_prop_unit->setText("ns");
    if(arg1=="BIRKSCONSTANT")
        ui->const_prop_unit->setText("mm/MeV");
    ui->prop_unit->setText("--");


}

void SimpleMaterialPropertyBuilder::on_g4_const_prop_value_valueChanged(const QString &arg1)
{

}

void SimpleMaterialPropertyBuilder::on_g4_property_currentTextChanged(const QString &arg1)
{
    if(arg1=="ABSLENGTH")
        ui->prop_unit->setText("cm");
}
