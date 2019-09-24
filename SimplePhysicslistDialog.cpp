#include "SimplePhysicslistDialog.h"
#include "ui_physicslistdialog.h"

PhysicsListDialog::PhysicsListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhysicsListDialog)
{
    ui->setupUi(this);
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
