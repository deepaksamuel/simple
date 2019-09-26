// File modified by Deepak Samuel on 25 Sep 2019
#include "SimpleMaterialBuilder.h"
#include "ui_SimpleMaterialBuilder.h"
#include <QMessageBox>
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
SimpleMaterialBuilder *SimpleMaterialBuilder::instance = nullptr;

SimpleMaterialBuilder::SimpleMaterialBuilder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleMaterialBuilder)
{
    ui->setupUi(this);
    //    dbManager = dbM;
    
    //    ui->g4materiallist->insertItems(0,dbManager->GetListOfMaterials());
    //    ui->g4elementlist->insertItems(0,dbManager->GetListOfElementSymbols());
    setWindowTitle("New material");
    ui->material_temp->setVisible(false);
    ui->material_pressure->setVisible(false);
    ui->temp_label->setVisible(false);
    ui->press_label->setVisible(false);
    ui->g4elementlist->hide();
    ui->number_of_atoms->hide();
    ui->element_label->hide();
    ui->frac_mass->hide();
    ui->frac_mass->setChecked(true);
    
}



SimpleMaterialBuilder::~SimpleMaterialBuilder()
{
    delete ui;
}

void SimpleMaterialBuilder::SetDataBaseManager(SimpleDatabaseManager *dbM)
{
    dbManager = dbM;
    
    ui->g4materiallist->insertItems(0,dbManager->GetListOfMaterials());
    ui->g4elementlist->insertItems(0,dbManager->GetListOfElementSymbols());
}
void SimpleMaterialBuilder::ShowUI()
{
    
    ui->g4materiallist->clear();
    ui->g4elementlist->clear();
    
    ui->g4materiallist->insertItems(0,dbManager->GetListOfMaterials());
    ui->g4elementlist->insertItems(0,dbManager->GetListOfElementSymbols());
    
    ui->materialList->clear();
    
    mat_names.clear();
    mat_fractions.clear();
    totalFrac=0.000000;
    show();
}

void SimpleMaterialBuilder::SetBuildForElements(bool state)
{
    // ui->build_from_elements->setChecked(state);
    
    ui->g4materiallist->setVisible(!state);
    ui->add_material->setVisible(!state);
    
}



void SimpleMaterialBuilder::on_build_from_elements_toggled(bool checked)
{
    SetBuildForElements(checked);
}

void SimpleMaterialBuilder::on_material_stp_toggled(bool checked)
{
    ui->material_temp->setVisible(!checked);
    ui->material_pressure->setVisible(!checked);
    ui->temp_label->setVisible(!checked);
    ui->press_label->setVisible(!checked);
}

void SimpleMaterialBuilder::on_add_material_clicked()
{
    //create a string of the form
    if(ui->mat_composition->isChecked())
        mat_names.append(ui->g4materiallist->currentText());
    else {
        mat_names.append(ui->g4elementlist->currentText());
    }
    mat_fractions.append(ui->material_fraction->value());
    totalFrac=0.00000;
    
    foreach(double d, mat_fractions)
        totalFrac+=d;
    if(ui->mat_composition->isChecked() | ui->frac_mass->isChecked())
        ui->totfrac_label->setText(QString("Total fraction: %1").arg(totalFrac));
    else {
        ui->totfrac_label->setText(QString("Total atoms: %1").arg(totalFrac));
    }
    if(ui->elementalcomposition->isChecked() && ui->number_of_atoms->isChecked())
        ui->materialList->append(QString("%2*%1").arg(mat_names.last()).arg(int(mat_fractions.last())));
    else {
        ui->materialList->append(QString("%2*%1").arg(mat_names.last()).arg((mat_fractions.last())));
    }
    
}

void SimpleMaterialBuilder::on_add_element_clicked()
{
    //create a string of the form
    //  "(name:n;density:d;temp:t;press:p; mat1*frac1 + mat2*frac + ....)"
}

void SimpleMaterialBuilder::on_clear_clicked()
{
    mat_names.clear();
    mat_fractions.clear();
    totalFrac=0.000000;
    ui->totfrac_label->setText(QString("Total: %1").arg(totalFrac));
    ui->materialList->clear();
    
}

void SimpleMaterialBuilder::on_create_material_clicked()
{
    mat_names.clear();
    mat_fractions.clear();
    
    
    QStringList sl = ui->materialList->toPlainText().split("\n");
    
    foreach(QString str,sl)
    {
        
        mat_fractions.append(str.split("*").at(0).toDouble());
        mat_names.append(str.split("*").at(1));
        
    }
    
    
    
    if(mat_names.count()<2){
        QMessageBox::warning(this,"Insufficient materials to build" , "Please enter atleast 2 materials to build a composite");
        return;
    }
    
    QString name = ui->material_name->text();
    int state = ui->material_state->currentIndex(); //kStateUndefined    kStateSolid 	  kStateLiquid 	 kStateGas
    
    if(name==""){
        QMessageBox::warning(this,"Empty name field" , "Please enter a name for the material");
        return;
    }
    if(name.contains("G4_")){
        QMessageBox::warning(this,"Invalid name" , "Custom material names should not contain G4_");
        return;
    }
    
    double density = ui->material_density->value(); //g/cm3
    
    double temp = -999999; // dummy temp for STP
    double press = -999999; // dummy press for STP
    if(!ui->material_stp->isChecked()){
        temp = ui->material_temp->value(); // c
        press = ui->material_pressure->value(); // atm
    }
    
    int ii=0;
    QStringList mats;
    foreach(QString material, mat_names)
    {
        mats.append(QString("%1*%2").arg(mat_fractions.at(ii++)).arg(material));
    }
    QString type;
    if(ui->mat_composition->isChecked())
    {
        type="material";
    }
    else {
        type = "element-natoms";
        if(ui->frac_mass->isChecked()){
            type="element-fracmass";
        }
        
    }
    
    //"(type: t;state: s; density:d;temp:t;press:p; frac1*mat1 + frac2*mat + ....)"
    QString final_command = QString("(type:%6;state:%1;density:%2;temp:%3;press:%4;%5)").arg(state).arg(density).arg(temp).arg(press).arg(mats.join("+")).arg(type);
    qDebug()<<"Inserting into material database:"<<final_command;
    dbManager->InsertNewMaterial(name,final_command);
    close();
}

QString SimpleMaterialBuilder::GetMaterialFormula(QString name)
{
    return dbManager->GetMaterialFormula(name);
}
G4Material* SimpleMaterialBuilder::GetMaterial(QString name, QString formula)
{
    //format: "(type:mat;state:s; density:d;temp:t;press:p; frac1*mat1 + frac2*mat + ....)"
    G4NistManager* nist = G4NistManager::Instance();
    
    formula = formula.remove("(").remove(")");
    QStringList s = formula.split(";");
    qDebug()<<"New material creation from user string"<<s;
    int state; //kStateUndefined    kStateSolid 	  kStateLiquid 	 kStateGas
    double density, temp, press;
    int nComponents =0;
    QString type;
    G4Material* new_material = nullptr;

    bool modify_prop_only = false;
    if(s.count()!=6){
        //if the material has modified property, it will have its formula as a G4Material. Check that
        if(s.count()==1 && s.at(0).contains("G4_")){
            modify_prop_only =true;
            new_material = nist->FindOrBuildMaterial(s.at(0).toLatin1().data());
            qDebug()<<"Found G4 material with property formula "<<s.at(0);
        }
        else{
            qDebug()<<"[1] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR";
            return nist->FindOrBuildMaterial("G4_AIR");
        }
    }
    if(!modify_prop_only){
        if(s.at(0).split(":").count()==2){
            type =s.at(0).split(":").at(1);
        }
        else{qDebug()<<"[2] Error creating material from formula (Unknown type). Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");}
        if(s.at(1).split(":").count()==2){
            state =s.at(1).split(":").at(1).toInt();
        }
        else {qDebug()<<"[3] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");}
        if(s.at(2).split(":").count()==2){
            density =s.at(2).split(":").at(1).toDouble();
        }
        else{qDebug()<<"[4] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");}
        if(s.at(3).split(":").count()==2){
            temp =s.at(3).split(":").at(1).toDouble();
        }
        else{qDebug()<<"[5] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");}
        if(s.at(4).split(":").count()==2){
            press =s.at(4).split(":").at(1).toDouble();
        }
        else{qDebug()<<"[6] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");}

        G4State g4_state= G4State::kStateUndefined;
        if(state==1) g4_state = G4State::kStateSolid;
        if(state==2) g4_state = G4State::kStateLiquid;
        if(state==3) g4_state = G4State::kStateGas;

        QStringList comps = s.at(5).split("+");
        nComponents =comps.count();
        QStringList mats;
        QList <double> fracs;


        //format: "(type:mat;state:s; density:d;temp:t;press:p; frac1*mat1 + frac2*mat + ....)"
        foreach(QString comp, comps){ // gets the fracs and the mats
            if(comp.split("*").count()==2){
                fracs.append(comp.split("*").at(0).toDouble());
                mats.append(comp.split("*").at(1));
            }
            else {
                qDebug()<<"[7] Error creating material from formula. Incorrect number of parameters. Setting material to G4_AIR"; return nist->FindOrBuildMaterial("G4_AIR");
            }
        }


        if(state==0 && temp<-999998.0)
            new_material = new G4Material(name.toLatin1().data(), density*g/cm3, nComponents);
        else if(state!=0 && temp<999998.0){
            new_material = new G4Material(name.toLatin1().data(), density*g/cm3, nComponents, g4_state);
        }
        else {
            new_material = new G4Material(name.toLatin1().data(), density*g/cm3, nComponents,g4_state,temp*kelvin, press*atmosphere);
        }

        for(int ii=0; ii<nComponents;ii++)
        {
            if(!mats.at(ii).contains("G4_")){ // this is not a standard G4 material
                if(type=="material"){
                    QString s = QString("adding custom material %1 (%2) to main material %3").arg(mats.at(ii)).arg(fracs.at(ii)).arg(name);
                    qDebug()<<s;
                    new_material->AddMaterial(GetMaterial(mats.at(ii),dbManager->GetMaterialFormula(mats.at(ii))),fracs.at(ii));
                }
                else if (type=="element-fracmass") { // element
                    QString s = QString("adding G4Element %1 (%2) to main material %3").arg(mats.at(ii)).arg(fracs.at(ii)).arg(name);
                    qDebug()<<s;
                    new_material->AddElement(nist->FindOrBuildElement(mats.at(ii).toLatin1().data()),fracs.at(ii));
                }
                else if (type=="element-natoms") { // element
                    QString s = QString("adding G4Element %1 (%2 atoms) to main material %3").arg(mats.at(ii)).arg(fracs.at(ii)).arg(name);
                    qDebug()<<s;
                    new_material->AddElement(nist->FindOrBuildElement(mats.at(ii).toLatin1().data()),int(fracs.at(ii)));
                }
            }
            else{
                QString s = QString("adding G4Material %1 (%2) to main material %3").arg(mats.at(ii)).arg(fracs.at(ii)).arg(name);
                qDebug()<<s;
                new_material->AddMaterial(nist->FindOrBuildMaterial(mats.at(ii).toLatin1().data()),fracs.at(ii));
            }
        }

        qDebug()<<"Created new material "<<new_material->GetName();
    }
    // check if material property exists
    if(dbManager->GetMaterialProperty(name)!=""){
        qDebug()<<"Updating material properties...";
        SimpleMaterialPropertyBuilder::getInstance()->SetMaterialProperties(new_material,dbManager->GetMaterialProperty(name));
    }

    return new_material;
    
    
}

void SimpleMaterialBuilder::on_material_fraction_valueChanged(double arg1)
{
    if(ui->elementalcomposition->isChecked()&& ui->number_of_atoms->isChecked())
        ui->totfrac_label->setText(QString("Total atoms: %1").arg(totalFrac+ui->material_fraction->value()));
    else
        ui->totfrac_label->setText(QString("Total fraction: %1").arg(totalFrac+ui->material_fraction->value()));
}

void SimpleMaterialBuilder::on_addElement_clicked()
{
    
}

void SimpleMaterialBuilder::on_elementalcomposition_toggled(bool checked)
{
    ui->g4elementlist->setVisible(checked);
    ui->number_of_atoms->setVisible(checked);
    ui->frac_mass->setVisible(checked);
    ui->element_label->setVisible(checked);
    if(checked==true)
        ui->add_material->setText("Add element");
    else {
        ui->add_material->setText("Add Material");
    }
    ui->g4materiallist->setVisible(!checked);
    ui->materialList->clear();
    
}

void SimpleMaterialBuilder::on_frac_mass_toggled(bool checked)
{
    
}

void SimpleMaterialBuilder::on_number_of_atoms_toggled(bool checked)
{
    if(checked){
        ui->fraction_label->setText("No. of atoms");
        ui->material_fraction->setMinimum(1);
        ui->material_fraction->setDecimals(0);
        ui->material_fraction->setMaximum(99);
        ui->totfrac_label->setText("Total atoms: ");
    }
    else{
        ui->fraction_label->setText("Fraction");
        ui->material_fraction->setMinimum(0.000001);
        ui->material_fraction->setDecimals(6);
        ui->material_fraction->setMaximum(1);
        ui->totfrac_label->setText("Total fraction: ");
    }
    ui->materialList->clear();
}
