// File modified by Deepak Samuel on 25 Sep 2019
#include "SimplePhysicslistDialog.h"
#include "ui_physicslistdialog.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
PhysicsListDialog::PhysicsListDialog(QWidget *parent, SimpleDatabaseManager *dbM) :
    QDialog(parent),
    ui(new Ui::PhysicsListDialog)
{
    ui->setupUi(this);
    dbManager =dbM;
    LoadRecentFiles();
    ui->recent_file_tree->hide();
    ui->open_other_files->hide();
    ui->recent_file_label->hide();
}

void PhysicsListDialog::SetDatabaseManager(SimpleDatabaseManager *dbM)
{
    dbManager =dbM;
}
void PhysicsListDialog::LoadRecentFiles()
{
    //qDebug()<<dbManager->GetRecentFileList();
    QStringList files = dbManager->GetRecentFileList();
    ui->recent_file_tree->setColumnCount(2);
    QList<QTreeWidgetItem *> items;

    foreach(QString fileName, files) {
        QFile f(fileName);
        if(f.exists()){
            if (!f.open(QIODevice::ReadOnly)) {
                qDebug()<<"Could not open file "<<fileName;
            }
            QByteArray data = f.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(data));
            QJsonObject json = loadDoc.object();
            QString physics = json["physics"].toString();
            QStringList filelist = {fileName,physics};
            items.append(new QTreeWidgetItem((QTreeWidget*)0, filelist));
        }
        else {
            qDebug()<<"Could not find file in recent list: "<<fileName;
        }
    }
    ui->recent_file_tree->insertTopLevelItems(0, items);
    ui->recent_file_tree->resizeColumnToContents(0);
}
PhysicsListDialog::~PhysicsListDialog()
{
    delete ui;
}

void PhysicsListDialog::SetContents(QStringList items, QStringList help)
{
    helpTexts =help;
    ui->physicsLists->addItems(items);

}


void PhysicsListDialog::on_physicsLists_currentIndexChanged(int index)
{
    ui->physicsListHint->setText(helpTexts.at(index));
}

QString PhysicsListDialog::GetSelectedPhysicsList()
{
    return ui->physicsLists->currentText();
}

int PhysicsListDialog::GetNumberofThreads()
{
    return ui->nThreads->value();
}

bool PhysicsListDialog::isVisDisabled()
{
    return ui->disable_vis->isChecked();
}

bool PhysicsListDialog::isFirstTimeUse()
{
    return ui->firstTimeUse->isChecked();
}

void PhysicsListDialog::on_recent_file_tree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item!=nullptr){
        recent_file_name=item->text(0);
        recent_file_physicslist=item->text(1);
        int index = ui->physicsLists->findText(recent_file_physicslist);
        if(index<0) // this might happen if the file is gdml
            index=0;
        ui->physicsLists->setCurrentIndex(index); // set the current physics list to be of the recent file that is clicked.
        done(QDialog::Accepted);
    }
}

QString PhysicsListDialog::GetRecentFile()
{
    return recent_file_name;
}

void PhysicsListDialog::on_show_recent_files_clicked(bool checked)
{
//    if(checked){
//        ui->recent_file_tree->show();
//        ui->open_other_files->show();
//        ui->recent_file_label->show();
//        ui->buttonBox->hide();
//        ui->physicsLists->hide();
//        ui->physicsListHint->hide();
//    }
//    else {
//        ui->recent_file_tree->hide();
//        ui->open_other_files->hide();
//        ui->recent_file_label->hide();
//        ui->buttonBox->show();
//        ui->physicsLists->show();
//        ui->physicsListHint->show();

//    }

}

void PhysicsListDialog::on_open_other_files_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open geometry files"),
                                                    QDir::currentPath(),
                                                    tr("Simulation Files (*.sim);;GDML files (*.gdml)"));
    if(fileName!=""){
        recent_file_name = fileName;
        QFile f(fileName);
        if(f.exists() && fileName.contains(".sim")){
            //qDebug()<<"Simulation file...";
            if (!f.open(QIODevice::ReadOnly)) {
                qDebug()<<"Could not open file "<<fileName;
            }
            QByteArray data = f.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(data));
            QJsonObject json = loadDoc.object();
            QString physics = json["physics"].toString();

            recent_file_physicslist=physics;
            int index = ui->physicsLists->findText(recent_file_physicslist);
            ui->physicsLists->setCurrentIndex(index); // set the current physics list to be of the recent file that is clicked.

        }
        else if(f.exists() && fileName.contains(".gdml")){
            //qDebug()<<"GDML file...";
            recent_file_physicslist="FTFP_BERT";
            int index = ui->physicsLists->findText(recent_file_physicslist);
            ui->physicsLists->setCurrentIndex(index); // set the current physics list to be of the recent file that is clicked.
        }
    }
     done(QDialog::Accepted);
}

void PhysicsListDialog::on_show_recent_files_clicked()
{

}

void PhysicsListDialog::on_open_create_currentIndexChanged(int index)
{
    if(index==1){
        setWindowTitle("Open file ...");
        ui->recent_file_tree->show();
        ui->open_other_files->show();
        ui->recent_file_label->show();
        ui->buttonBox->hide();
        ui->physicsLists->hide();
        ui->physicsListHint->hide();
    }
    else {
        setWindowTitle("Select physics list and number of threads");
        ui->recent_file_tree->hide();
        ui->open_other_files->hide();
        ui->recent_file_label->hide();
        ui->buttonBox->show();
        ui->physicsLists->show();
        ui->physicsListHint->show();

    }
}
