#ifndef PHYSICSLISTDIALOG_H
#define PHYSICSLISTDIALOG_H
// File modified by Deepak Samuel on 25 Sep 2019
#include <QDialog>
#include <SimpleDatabaseManager.h>
#include <QTreeWidgetItem>

namespace Ui {
class PhysicsListDialog;
}

class PhysicsListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhysicsListDialog(QWidget *parent = nullptr, SimpleDatabaseManager *dbM=nullptr);
    void SetContents(QStringList items, QStringList help);
    QString GetSelectedPhysicsList();
    int GetNumberofThreads();
    bool isVisDisabled();
    bool isFirstTimeUse();
    void SetDatabaseManager(SimpleDatabaseManager* dbM);
    void LoadRecentFiles();
    QString GetRecentFile();
    ~PhysicsListDialog();

private slots:
    void on_physicsLists_currentIndexChanged(int index);

    void on_recent_file_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_show_recent_files_clicked(bool checked);

    void on_open_other_files_clicked();

    void on_show_recent_files_clicked();

    void on_open_create_currentIndexChanged(int index);

private:
    Ui::PhysicsListDialog *ui;
    QStringList helpTexts;
    SimpleDatabaseManager* dbManager;
    QString recent_file_name, recent_file_physicslist; // this will be filled if the user wants to open a recent file.
};

#endif // PHYSICSLISTDIALOG_H
