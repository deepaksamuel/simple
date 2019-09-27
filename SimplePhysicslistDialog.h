#ifndef PHYSICSLISTDIALOG_H
#define PHYSICSLISTDIALOG_H
// File modified by Deepak Samuel on 25 Sep 2019
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
    bool isFirstTimeUse();
    ~PhysicsListDialog();

private slots:
    void on_physicsLists_currentIndexChanged(int index);

private:
    Ui::PhysicsListDialog *ui;
    QStringList helpTexts;
};

#endif // PHYSICSLISTDIALOG_H
