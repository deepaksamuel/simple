#ifndef PHYSICSLISTDIALOG_H
#define PHYSICSLISTDIALOG_H

#include <QDialog>

namespace Ui {
class PhysicsListDialog;
}

class PhysicsListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhysicsListDialog(QWidget *parent = nullptr);
    void SetContents(QStringList items, QStringList help);
    QString GetSelectedPhysicsList();
    int GetNumberofThreads();
    bool isVisDisabled();
    ~PhysicsListDialog();

private slots:
    void on_physicsLists_currentIndexChanged(int index);

private:
    Ui::PhysicsListDialog *ui;
    QStringList helpTexts;
};

#endif // PHYSICSLISTDIALOG_H
