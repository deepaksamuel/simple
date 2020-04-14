#ifndef SIMPLE_H
#define SIMPLE_H
// File modified by Deepak Samuel on 25 Sep 2019
#include <QMainWindow>
#include <G4RunManager.hh>
#include <G4MTRunManager.hh>
#include <G4VisManager.hh>
#include <G4VisExecutive.hh>
#include <G4UImanager.hh>
#include <G4UIExecutive.hh>
#include <G4ScoringManager.hh>
#include <SimpleDatabaseManager.h>
#include <SimpleAction.h>
#include <QFileDialog>
#include <QDebug>
#include <QSqlTableModel>
#include <G4VBasicShell.hh>
#include <G4VInteractiveSession.hh>
#include <QStandardItem>
#include <QStandardItemModel>
#include <G4UIcommandTree.hh>
#include <QCompleter>
#include <SimpleObject.h>
#include <QTreeWidgetItem>
#include <SimpleObjectTableModel.h>
#include <SimpleMeshTableModel.h>
#include <QSettings>
#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <iROOTWorksheet.h>
#include <QGLWidget>
#include <QFileSystemModel>
#include <QJsonArray>
#include <SimpleMaterialBuilder.h>
#include <SimpleMaterialPropertyBuilder.h>
#include <iCanvas.h>

namespace Ui {
class Simple;
}

class Simple : public QMainWindow,  public G4VBasicShell, public G4VInteractiveSession
{
    Q_OBJECT

public:
    explicit Simple(int argc, char *argv[], QWidget *parent = nullptr);
    void InitRoot();
    void InitGeant4(int argc, char *argv[]);
    int InitDB();
    void InitUI();
    void InitEnvVars();
    void InitMaterials();
    void InitPhysicsLists();
    void CreateMenus();
    void OpenFile(QString fileName);
    void output(QString content, QString type="");
    QStringList refPhysicsNames;
    QStringList refPhysicsHints;
    SimpleAction* gsact;
    G4UIsession* SessionStart();
    virtual void PauseSessionStart(const G4String&);
    virtual G4bool GetHelpChoice(G4int&){return true;}
    virtual void ExitHelp() const{}
    virtual G4int ReceiveG4cout(const G4String&);
    virtual G4int ReceiveG4cerr(const G4String&);
    QStandardItemModel* CreateCompleterModel(G4String aCmd);
    void UpdateCommandCompleter();
    void AddObject(SimpleObject *obj);
    void AddObjects (QList <SimpleObject*> objects);
    void AddObject(SimpleMesh *mesh);
    void AddObject(QJsonObject obj);
    void AddObjects(QList <SimpleMesh *> mesh);
    void AddG4Widget(QWidget *w);
    QWidget* GetSceneTreeWidget();
    void UpdateGeometry();
    void Execute(QString command);
    void Execute(QStringList commands);
    QStringList CreateParticleSource(QString particle, QString sourceType, QString sourceShape, QVector3D sourceCentre, QList <float> sourceDims, QString angularType, QString energyDist, QList <float> eSpectParams, float eMin, float eMax, QVector3D directions, QVector3D pol);
    SimpleObject* MakeCopy(SimpleObject* obj, int alongaxis, float sep);
    void InitRecordParameters();
    void InitScoringParameters();
    void InitViewer();
    void UpdateViewer();
    void SetRecordParameters(QJsonObject jsonObject);
    void SetGunParameters(QJsonObject jsonObject);
    void SetUIParameters(QJsonObject jsonObject);
    void SaveProject(QString fileName);
    void IncrementRunId();
    void GetOutputDirectory();
    void HandleOutputFiles();
    QStringList GetRecentFileList();
    QString GetPhysicsList(QString fileName);// physics list of recent files
    bool AddTabWidget(QWidget* w,const QString s);
    QTabWidget* GetViewerTabWidget();
    QJsonArray gunParams();



    ~Simple();



private slots:


    void delete_current_mesh();
    void plot_mesh_projection();
    void plot_mesh_column();
    void on_add_envVar_clicked();

    void on_del_env_var_clicked();

    void on_update_env_var_clicked();


    void on_fCommandArea_returnPressed();


    void on_addObject_clicked();

    void on_shapesList_currentIndexChanged(int index);

    //void on_hide_clicked();

    void on_objectTree_itemClicked(QTreeWidgetItem *item, int column);
    void UpdateObject(SimpleObject *obj, int index);

    void on_objectTree_itemSelectionChanged();

    //void on_duplicate_clicked();
    //void on_delete_object_clicked();

    void on_db_type_currentIndexChanged(const QString &arg1);

    void on_objectTree_customContextMenuRequested(const QPoint &pos);



    void on_isWireframe_stateChanged(int arg1);

    void on_objectPropertiesTable_entered(const QModelIndex &index);

    void on_hide_gunStack_clicked();

    void on_sel_sourceType_clicked();

    //void on_sel_sourceShape_clicked();

    void on_sel_angDist_clicked();

    void on_sel_energyDist_clicked();

    //void on_sel_pol_clicked();

    void on_particleSource_toggled(bool checked);

    void on_sourceShape_currentIndexChanged(const QString &arg1);

    void on_eSpectType_currentIndexChanged(const QString &arg1);

    void on_shoot_clicked();

    //void on_nthreads_valueChanged(const QString &arg1);

    //void on_g4_verbosity_currentIndexChanged(const QString &arg1);

    void on_clearOutput_clicked();

    void on_shapesList_currentIndexChanged(const QString &arg1);

    void on_export_dataframe_clicked();

    void on_filterDataframe_clicked();

    void on_filterString_editingFinished();

    void on_plot_clicked();

    void on_plotString_returnPressed();

    void on_cutString_returnPressed();

    void on_plot_options_returnPressed();

    //void on_pushButton_clicked();

    //void on_createMesh_clicked();

    //void on_clear_scoring_commands_clicked();

    //void on_meshShape_itemClicked(QTreeWidgetItem *item, int column);

    void on_add_box_mesh_clicked();

    void on_add_cylindrical_mesh_clicked();

    void on_meshes_itemClicked(QTreeWidgetItem *item, int column);

    void on_meshes_customContextMenuRequested(const QPoint &pos);

    void on_setLogX_clicked(bool checked);

    void on_setLogY_clicked(bool checked);

    void on_run_macro_clicked();

    void on_clear_all_mesh_clicked();

    void on_fileDirectoryView_doubleClicked(const QModelIndex &index);

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_download_g4_data_files_clicked();

    void on_actionSave_as_triggered();

    void on_actionExit_2_triggered();

    void on_actionBox_triggered();

    void on_actionCylinder_triggered();

    void on_actionSphere_triggered();

    void on_actionWedge_triggered();

    void on_actionBox_Mesh_triggered();

    void on_actionCylinder_Mesh_triggered();

    void on_actionShoot_beam_triggered();

    void on_actionMacro_triggered();

    void on_actionEnvironment_variables_triggered();

    void on_actionMaterial_list_triggered();

    void on_actionDetecor_view_triggered();

    void on_actionPlots_triggered();

    void on_actionScene_Object_Window_triggered();

    void on_actionMesh_Window_triggered();

    void on_actionProperty_Editor_triggered();

    void on_actionOutput_file_settings_triggered();

    void on_actionSource_Type_triggered();

    void on_actionBeam_energy_settings_triggered();

    void on_actionBeam_type_settings_triggered();

    void on_actionBeam_angular_distribution_triggered();

    void on_actionHelp_triggered();

    //void on_build_from_materials_toggled(bool checked);

    void on_addNewMaterial_clicked();

    void on_actionNew_Material_triggered();

    void on_actionMaterial_property_triggered();

    void on_switch_off_mag_fields_stateChanged(int arg1);

    void on_auto_search_g4variables_clicked();

    void on_actionExport_triggered();

    void on_show_file_explorer_clicked();

    void on_auto_search_cvmfs_clicked();

    void set_output_directory(QString s);

    void on_select_directory_clicked();

    void on_db_type_currentIndexChanged(int index);

    void on_show_output_directory_clicked();

    //void on_clear_output_directory_clicked();

    void on_fileList_currentIndexChanged(const QString &arg1);

    void on_fit_clicked();




    void on_show_plot_window_toggled(bool checked);

private:
    Ui::Simple *ui;
    G4UImanager* UImanager;
    G4MTRunManager * runManager;
    G4ScoringManager * scManager;
    SimpleDatabaseManager* dbManager;
    QCompleter* fCompleter;
    bool g4started;
    QString currentPhysicsList;
    G4VUserPhysicsList *physicsList; // if user chooses a customized physics list.

    QStringList materials;
    QList <SimpleObject*> objectList;
    QList <SimpleMesh*> meshList, used_meshList;
    //QList <SimpleObject*> scoringObjects; // these will be used for scoring
    //QList <SimpleObject*> detectorObjects; // these will be used by detector construction
    SimpleObjectTableModel *object_table_model;
    SimpleMeshTableModel *mesh_table_model;
    DBTableModel *db_table_model;
    SimpleMaterialBuilder *mat_builder;
    QMenu *objectMenu;
    QMenu *meshMenu;
    QAction *object_delete_action;
    QAction *object_duplicate_action;
    QAction *mesh_delete_action;
    QAction *mesh_plot_projection_action;
    QAction *mesh_plot_column_action;
    QAction *mesh_dump_qty_action;
    int nThreads;
    TApplication* iRootApp;
    IROOTWorksheet* worksheet;
    int mesh_id;
    QFileSystemModel *fileSystemModel;
    QString last_opened_file;
    bool is_user_physics_list;
    bool is_vis_disabled;
    bool is_first_time_use;
    bool is_gdml;
    int run_id;
    QString output_directory;
    QString output_filename;// this will change for every run
    QString mesh_file_prefix;
    QStringList output_filelist;
    QString recent_file; // the file that user opened from the recent list
    iCanvas* canvas;




};






#endif // SIMPLE_H
