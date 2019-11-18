// File modified by Deepak Samuel on 25 Sep 2019
#include "ui_simple.h"
#include "Simple.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <G4PhysListFactory.hh>
#include <SimpleDetectorConstruction.h>
#include <SimpleRunAction.h>
#include <QWindow>
#include <QProcess>
#include <SimpleVis.h>
#include <SimplePhysicslistDialog.h>
#include <QSizePolicy>
#include <QDialogButtonBox>
#include <TGLEmbeddedViewer.h>
#include <iCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <QMdiSubWindow>
#include <iROOTWorksheet.h>
#include <QFileDialog>
#include <SimpleStoredVis.h>
#include <G4OpenGLStoredViewer.hh>
#include <G4Qt.hh>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>
#include <physics_lists/PhysicsList_Hadrontherapy.hh>
#include <physics_lists/PhysicsList_Opt.hh>
#include <physics_lists/PhysicsList_PolEM.hh>
#include <physics_lists/PhysicsList_RadDecay.hh>
#include <physics_lists/PhysListEmPolarized.hh>
#include <physics_lists/PhysicsList_DMX.hh>
#include <physics_lists/PhysicsList_DNA.hh>
#include <physics_lists/PhysicsList_MuDecayWithSpin.hh>
#include <G4OpticalPhysics.hh>
#include "FTFP_BERT.hh"
#include "G4GDMLParser.hh"


Simple::Simple(int argc, char *argv[],QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Simple)
{
    g4started=false;
    ui->setupUi(this);


    showMaximized();
    hide();


    if(InitDB()==0){
        InitEnvVars();
        InitMaterials();

        InitPhysicsLists();
        InitUI();
        if(!is_first_time_use){
            ui->fViewerTabWidget->setCurrentIndex(0);
            InitGeant4(argc,argv);
        }
        else {
            ui->fViewerTabWidget->setCurrentIndex(1);
        }
        InitRoot();
        CreateMenus();
    }
    else{

    }

    g4started=true;
}

void Simple::CreateMenus()
{
    objectMenu = new QMenu(ui->objectTree);


    object_delete_action = new QAction(tr("&Delete"), this);
    object_duplicate_action = new QAction(tr("&Duplicate"), this);



    connect(object_delete_action, SIGNAL(triggered()), this, SLOT(on_delete_object_clicked()));
    connect(object_duplicate_action, SIGNAL(triggered()), this, SLOT(on_duplicate_clicked()));
    objectMenu->addAction(object_delete_action);
    objectMenu->addAction(object_duplicate_action);


    meshMenu = new QMenu(ui->meshes);
    mesh_delete_action = new QAction(tr("&Delete"), this);
    mesh_plot_projection_action = new QAction(tr("&Plot Projection"), this);
    mesh_plot_column_action = new QAction(tr("&Plot Column"), this);
    // mesh_dump_qty_action = new QAction(tr("&Save Column"), this);
    meshMenu->addAction(mesh_delete_action);
    meshMenu->addAction(mesh_plot_projection_action);
    meshMenu->addAction(mesh_plot_column_action);


    connect(mesh_delete_action, SIGNAL(triggered()), this, SLOT(delete_current_mesh()));
    connect(mesh_plot_projection_action, SIGNAL(triggered()), this, SLOT(plot_mesh_projection()));
    connect(mesh_plot_column_action, SIGNAL(triggered()), this, SLOT(plot_mesh_column()));
    //connect(object_duplicate_action, SIGNAL(triggered()), this, SLOT(on_duplicate_clicked()));
}

void Simple::output(QString content, QString type)
{
    if(type=="error")
        ui->out->setTextColor( QColor( "red" ));
    else if(type=="warn")
        ui->out->setTextColor( QColor( "red" ));
    else if(type=="success")
        ui->out->setTextColor( QColor( "black" ));
    else if(type=="process")
        ui->out->setTextColor( QColor( "black" ));
    else if(type=="g4message"){
        ui->out->setTextColor( QColor( "gray" ));
    }
    else if(type=="g4error"){
        ui->out->setTextColor( QColor( "red" ));
    }
    else if(type=="")
        ui->out->setTextColor( QColor( "black" ));

    ui->out->append(content);


}
int Simple::InitDB()
{
    // output("Initializing database manager...");
    if (QSqlDatabase::drivers().isEmpty()){
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));
        return -1;
    }

    dbManager = new SimpleDatabaseManager;
    if(!dbManager){
        output("Unknown error in accessing database manager (SQLITE3)...","error");
        QMessageBox::information(this, tr("Database error"),
                                 tr("Unknown error in accessing database manager (SQLITE3)"));
        return -1;
    }
    else {
        if(dbManager->GetListOfShapes().count()==0) {// this is an indication that we are not able to access the db file, probably it is not in the right place!
            QMessageBox::information(this, tr("No database files found"),
                                     tr("The database file db.sqlite, usually placed next to the application, is missing or empty."));
            return -2;
        }
        else{

            //    output("Done!","success");
            //            qDebug()<<dbManager->GetListOfElements();
            //            qDebug()<<dbManager->GetListOfElementSymbols();
            //            qDebug()<<dbManager->GetListOfZ();
            //            qDebug()<<dbManager->GetListOfAtomicMass();


            return 0;
        }
    }

}

void Simple::InitEnvVars()
{

    // populate the table:
    //  output("Populating the database table...","process");
    db_table_model = new DBTableModel(ui->envTable, dbManager->currentDatabase() );
    ui->envTable->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    db_table_model->setTable("env_vars");
    db_table_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    db_table_model->select();
    ui->envTable->setModel(db_table_model);
    db_table_model->setHeaderData(0, Qt::Horizontal, tr("Env Var"));
    db_table_model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    db_table_model->select();
    if (db_table_model->lastError().type() != QSqlError::NoError){
        output(db_table_model->lastError().text(),"error");
        return;
    }

    ui->envTable->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    // set the variables

    output("Setting Geant4 dataset paths...","process");
    QStringList env_vars = dbManager->GetListOfEnvVars();
    QStringList values = dbManager->GetListOfEnvVarValues();

    ui->env_var_error->setStyleSheet("QLabel { color : green; }");
    ui->env_var_error->setText("All folder paths verified.");

    for(int ii=0; ii<env_vars.count();ii++){
        qputenv(env_vars.at(ii).toLatin1().data(),values.at(ii).toLatin1());
        // check if the directory exists:
        if(!QDir(values.at(ii)).exists()){
            output(QString("Error setting environment variable: The folder %1 does not exist!").arg(values.at(ii)),"error");
            ui->env_var_error->setStyleSheet("QLabel { color : red; }");
            ui->env_var_error->setText("Error: one of the folder does not exist - check output log.");
        }
    }

    output("If you are using the application for the first time, you may have to check if the locations are right in settings","warn");

}


void Simple::InitMaterials()
{
    materials = dbManager->GetListOfMaterials();
    ui->objMaterial->insertItems(0, materials);
}

void Simple::InitPhysicsLists()
{
    is_user_physics_list=false;

    refPhysicsNames = dbManager->GetListOfRefPhysics();
    refPhysicsHints = dbManager->GetListOfRefPhysicsHints();
    PhysicsListDialog* dialog = new PhysicsListDialog(this);

    dialog->SetContents(refPhysicsNames,refPhysicsHints);
    dialog->exec();
    currentPhysicsList = dialog->GetSelectedPhysicsList();
    nThreads = dialog->GetNumberofThreads();
    is_vis_disabled = dialog->isVisDisabled();

    is_first_time_use = dialog->isFirstTimeUse();

    if(!is_first_time_use){
        QString physListName = currentPhysicsList;

        if(physListName == "Proton / Heavy Ion Therapy"){
            physicsList =   new  PhysicsList_Hadrontherapy();
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }
        if(physListName == "Radioactive decay"){
            qDebug()<<"Radioactive decay";
            physicsList =   new  PhysicsList_RadDecay;
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        if(physListName == "Optical (Cerenkov / Scintillation)"){
            physicsList =   new  PhysicsList_Opt();
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        if(physListName == "DNA physics"){
            physicsList =   new  PhysicsList_DNA();
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        if(physListName == "Underground experiment (Dark Matter)"){
            physicsList =   new  PhysicsList_DMX();
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        if(physListName == "Polarized EM physics (spin transfer of electrons)"){
            physicsList =   new  PhysicsList_PolEM();
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        if(physListName == "Muon decay with spin"){
            physicsList =   new  PhysicsList_MuDecayWithSpin("FTFP_BERT");
            is_user_physics_list=true;
            qDebug()<<"Current physics List (application based): "<<physListName;
            return;
        }

        // this gets executed if none of the above custom physics list are chosen.
        is_user_physics_list = false;
        G4PhysListFactory *physListFactory = new G4PhysListFactory();
        physicsList =   physListFactory->GetReferencePhysList(currentPhysicsList.toLatin1().data());
        qDebug()<<"Current physics List (G4): "<<physListName;
    }
    else
        QMessageBox::warning(this,"Set environment varibles", "If you are using this application for the first time, please enter the Geant4 environment variables for datasets, click update and restart!");

}


Simple::~Simple()
{
    delete mat_builder;
    delete SimpleMaterialPropertyBuilder::getInstance();
    delete ui;
}

void Simple::InitUI()
{
    // make a data directory if it did not exist
    ui->progressBar->hide();
    QDir dir(QDir::currentPath());
    dir.mkdir("data");

    // remove file with prefix "temp-" so that they can be overwritten
    dir.cd("data");
    QFileInfoList l = dir.entryInfoList();
    foreach(QFileInfo file, l){
        qDebug()<<file.fileName();
        if(file.fileName().contains("temp-"));
        dir.remove(file.fileName());
    }

    ui->gunStack->hide();
    on_particleSource_toggled(true);
    ui->sourceShape->setCurrentIndex(0);
    ui->eSpectType->setCurrentIndex(0);

    object_table_model= new SimpleObjectTableModel();
    object_table_model->setMaterialList(materials);
    ui->objectPropertiesTable->setModel(object_table_model);


    QStringList scoring_string_helper = {"Energy deposited", "Population", "Dose deposited", "Number of steps", "Number of collisions", "Number of secondaries", "Number of tracks", "Number of terminated tracks", "Track length", "Cell charge", "Cell flux", "Passage cell current", "Passage track length", "Flat surface current", "Flat surface flux"};
    ui->scoringParameter->insertItems(0,scoring_string_helper);
    mesh_table_model = new SimpleMeshTableModel();
    ui->scoring_table_view->setModel(mesh_table_model);
    // ui->sc->setModel(mesh_table_model);


    connect(object_table_model,SIGNAL(objectUpdated(SimpleObject*, int)), this,SLOT(UpdateObject(SimpleObject*, int)));
    on_eSpectType_currentIndexChanged("Monoenergetic");
    ui->out->clear();

    fileSystemModel = new QFileSystemModel();

    fileSystemModel->setRootPath( qApp->applicationDirPath().append("/data/"));
    fileSystemModel->setNameFilters({"*.root"});
    fileSystemModel->setNameFilterDisables(false);
    ui->fileDirectoryView->setModel(fileSystemModel);
    ui->fileDirectoryView->setRootIndex(fileSystemModel->index(qApp->applicationDirPath().append("/data/"))); // or use Current Index

    QList <int> size;
    size.append(250);
    size.append(750);
    ui->directoryViewSplitter->setSizes(size);

    mat_builder = SimpleMaterialBuilder::getInstance();
    mat_builder->SetDataBaseManager(dbManager);

    ui->addNewMaterial->hide();
}

void Simple::InitRoot()
{
    iRootApp = new TApplication("iRootApp",nullptr,0);
    iRootApp->SetReturnFromRun(true);  // tell application to return from run
    gROOT->SetStyle("ATLAS");
    gStyle->SetHistFillColor(38);
    gStyle->SetHistLineStyle(0);
    gStyle->SetOptStat("nemr");

    //  ui->plotTemplate->setCurrentIndex(4);
    gROOT->ForceStyle();
    gStyle->UseCurrentStyle();



    QMdiSubWindow *subWindow = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    iCanvas *widget = new iCanvas(subWindow);
    subWindow->setWidget(widget);
    ui->mdiArea->addSubWindow(subWindow);
    subWindow->showMaximized();

    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->activateWindow();
    subWindow->setOption (QMdiSubWindow::RubberBandResize);
    ui->mdiArea->setActiveSubWindow(subWindow);
    setCursor(Qt::ArrowCursor);

    widget->GetCanvas()->cd(1);
    widget->GetCanvas()->Update();
    gPad->Clear();
    gPad->SetGrid(1,1);




    worksheet = new IROOTWorksheet(1);
    ui->worksheetView->setModel(worksheet);
    worksheet->SetTable(ui->worksheetView);
    // worksheet->SetCurrentFile("simple_out_file.root");
}

QWidget* Simple::GetSceneTreeWidget()
{
    //return ui->sceneTree;
}
void Simple::AddG4Widget(QWidget* w)
{
    qDebug()<<"I am here";
    //    if(w==nullptr)
    //        qDebug()<<"Error in QGL pointer";
    //    if(w!=nullptr)
    //        ui->glWidgetLayout->addWidget(w);
    //    w->show();

}
void Simple::InitGeant4(int argc, char *argv[])
{

    showMaximized();


#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
    G4String ver = G4RunManagerKernel::GetRunManagerKernel()->GetVersionString();
    ui->g4_settings->setText(QString(ver).append(" / ").append("Physics list: ").append(currentPhysicsList));
    ui->actionGeant4_version->setText(QString(ver));
    ui->actionPhysics_List->setText("Physics list: "+ currentPhysicsList);
    runManager->SetNumberOfThreads(nThreads);//
    ui->nthreads->setText(QString("Threads: %1").arg(nThreads));

#else
    G4RunManager* runManager = new G4RunManager;
    G4String ver = G4RunManagerKernel::GetRunManagerKernel()->GetVersionString();
    ui->g4_settings->setText(QString(ver).append(" / ").append("Physics list: ").append(currentPhysicsList));
    output(QString(ver));
#endif


    scManager = G4ScoringManager::GetScoringManager();

    G4RunManager::GetRunManager()->SetUserInitialization(physicsList);
    G4RunManager::GetRunManager()->SetUserInitialization(new SimpleDetectorConstruction());
    gsact = new SimpleAction();
    G4RunManager::GetRunManager()->SetUserInitialization(gsact);

    connect(gsact->getGunPointer(),SIGNAL(nextEvent(int)),ui->progressBar,SLOT(setValue(int)),Qt::QueuedConnection);


    if(is_vis_disabled){
        qDebug()<<"Disabling Visualization...";
        ui->isWireframe->setDisabled(true);
    }

    if(!is_vis_disabled){
        qDebug()<<"Enabling Visualization...";
        ui->viewerLayout->removeItem(ui->viewerSpacer);

        G4VisManager* visManager = new G4VisExecutive();
        SimpleVis* vis = new SimpleVis();
        visManager->RegisterGraphicsSystem(vis);

        SimpleStoredVis* vis2 = new SimpleStoredVis();
        visManager->RegisterGraphicsSystem(vis2);


        //    visManager->RegisterGraphicsSystem(new G4OpenGLStoredQt);
        visManager->Initialise();
    }


    UImanager = G4UImanager::GetUIpointer();
    UImanager->SetSession(this);
    UImanager->SetG4UIWindow(this);
    UImanager->SetCoutDestination(this); // enable this to see output in this window


    G4UIExecutive *user_interface = new G4UIExecutive(argc, argv);

    InitViewer();
    //UImanager->ApplyCommand("/control/execute visinit.mac");
    // user_interface->SessionStart();

    SessionStart();

    UpdateCommandCompleter();

    QList <int> size;
    size.append(1);
    size.append(2);
    ui->splitter->setSizes(size);
    ui->splitter_2->setSizes(size);
    ui->splitter_3->setSizes(size);
    on_addObject_clicked();


}




void Simple::on_add_envVar_clicked()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->envTable->model());
    if (!model)
        return;

    QModelIndex insertIndex = ui->envTable->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    ui->envTable->setCurrentIndex(insertIndex);
    ui->envTable->edit(insertIndex);
    //model->select();
}

void Simple::on_del_env_var_clicked()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->envTable->model());
    if (!model)
        return;

    QModelIndexList currentSelection = ui->envTable->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }
    model->select();
}

void Simple::on_update_env_var_clicked()
{
    InitEnvVars();
    InitMaterials();
    object_table_model->setMaterialList(materials);

}


G4UIsession* Simple::SessionStart ()
{
    //    G4Qt* interactorManager = G4Qt::getInstance ();


    //    QCoreApplication::sendPostedEvents () ;



    //    interactorManager->DisableSecondaryLoop (); // TO KEEP
    //    if ((QApplication*)interactorManager->GetMainInteractor())
    //        ((QApplication*)interactorManager->GetMainInteractor())->exec();

    //    interactorManager->EnableSecondaryLoop ();
    return this;
}


void Simple::PauseSessionStart (const G4String& aState)
{
    if (!aState) return;

    if(aState=="G4_pause> ") {  // TO KEEP
        // SecondaryLoop ("Pause, type continue to exit this state"); // TO KEEP
    } // TO KEEP

    if(aState=="EndOfEvent") { // TO KEEP
        // Picking with feed back in event data Done here !!!
        //SecondaryLoop ("End of event, type continue to exit this state"); // TO KEEP
    } // TO KEEP
}


#ifdef G4MULTITHREADED
#include "G4Threading.hh"
#include "G4AutoLock.hh"
namespace {
G4Mutex ReceiveG4coutMutex = G4MUTEX_INITIALIZER;
G4Mutex ReceiveG4cerrMutex = G4MUTEX_INITIALIZER;
}
#endif
G4int Simple::ReceiveG4cout (const G4String& aString)
{

#ifdef G4MULTITHREADED
    G4AutoLock al(&ReceiveG4coutMutex);
#endif
    if(!ui->hide_g4_messages->isChecked()){
        output(QString(aString),"g4message");
    }
    return 0;
}



G4int Simple::ReceiveG4cerr (const G4String& aString)
{

#ifdef G4MULTITHREADED
    G4AutoLock al(&ReceiveG4cerrMutex);
#endif
    if(!ui->hide_g4_messages->isChecked()){
        output(QString(aString),"g4error");
    }
    return 0;
}

void Simple::UpdateCommandCompleter() {

    // remove previous one
    ui->fCommandArea->setCompleter(NULL);
    if (fCompleter) {
        //        if (fCompleter->popup()) {
        //            fCompleter->popup()->removeEventFilter(this);
        //        }
    }

    QStandardItemModel* model = CreateCompleterModel("/");
    fCompleter = new QCompleter(model);

    // set all dir visibles in completion
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4UIcommandTree * commandTreeTop = UI->GetTree();
    G4UIcommandTree* aTree = commandTreeTop->FindCommandTree("/");
    if (aTree) {
        int Ndir= aTree-> GetTreeEntry();
        fCompleter->setMaxVisibleItems(Ndir);
    }
    ui->fCommandArea->setCompleter(fCompleter);
    fCompleter->popup()->installEventFilter(this);

}


QStandardItemModel* Simple::CreateCompleterModel(G4String aCmd) {

    QList< QStandardItem*> dirModelList;
    QList< QStandardItem*> commandModelList;
    QList< QStandardItem*> subDirModelList;
    QList< QStandardItem*> subCommandModelList;

    G4String strtmp;
    G4int nMatch= 0;

    G4String pName = aCmd;
    G4String remainingPath = aCmd;
    G4String empty = "";
    G4String matchingPath = empty;

    // find the tree
    G4int jpre= pName.last('/');
    if(jpre != G4int(G4String::npos)) pName.remove(jpre+1);
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4UIcommandTree * commandTreeTop = UI->GetTree();
    G4UIcommandTree* aTree = commandTreeTop->FindCommandTree(pName);
    if (aTree) {
        int Ndir= aTree-> GetTreeEntry();
        int Ncmd= aTree-> GetCommandEntry();

        // directory ...
        for(G4int idir=1; idir<=Ndir; idir++) {
            G4String fpdir= aTree-> GetTree(idir)-> GetPathName();
            // matching test
            if( fpdir.index(remainingPath, 0) == 0) {
                if(nMatch==0) {
                    matchingPath = fpdir;
                } else {
                    matchingPath = aTree->GetFirstMatchedString(fpdir,matchingPath);
                }
                nMatch++;

                // append to dir model list
                QStandardItem* item1 = new QStandardItem(fpdir.data());
                //QIcon i = QIcon(*fDirIcon);
                item1->setData(1); // dir
                //item1->setIcon(QIcon(*fDirIcon));
                dirModelList.append(item1);

                // Go recursively
                QStandardItemModel* subModel = CreateCompleterModel(fpdir.data());
                for (int a=0; a< subModel->rowCount(); a++) {

                    // copy item (an item could only be part of one model
                    QStandardItem* tempItem = new QStandardItem(subModel->item(a)->text());
                    tempItem->setIcon(subModel->item(a)->icon());
                    tempItem->setToolTip(subModel->item(a)->toolTip());
                    tempItem->setData(subModel->item(a)->data());

                    // dir
                    if (tempItem->data() == 1) {
                        subModel->item(a);
                        subDirModelList.append(tempItem);
                    }
                    // command
                    else if (tempItem->data() == 0) {
                        subCommandModelList.append(tempItem);
                    }
                }
            }
        }

        // command ...
        G4int n_parameterEntry;
        G4String rangeString;
        G4int n_guidanceEntry;
        G4UIcommand * command;
        G4UIparameter *param;
        std::string tooltip;
        G4String params;

        for(G4int icmd=1; icmd<=Ncmd; icmd++){
            tooltip = "";
            params = " ";
            command = aTree-> GetCommand(icmd);
            G4String longCommandName= aTree-> GetPathName() +
                    command -> GetCommandName();
            rangeString = command->GetRange();
            n_guidanceEntry = command->GetGuidanceEntries();
            n_parameterEntry = command->GetParameterEntries();


            // matching test
            if( longCommandName.index(remainingPath, 0) ==0) {
                if(nMatch==0) {
                    matchingPath= longCommandName + " ";
                } else {
                    strtmp= longCommandName + " ";
                    matchingPath= aTree->GetFirstMatchedString(matchingPath, strtmp);
                }

                // guidance
                for( G4int i_thGuidance=0; i_thGuidance < n_guidanceEntry; i_thGuidance++ ) {
                    tooltip += std::string((command->GetGuidanceLine(i_thGuidance)).data());
                    if (i_thGuidance < n_guidanceEntry-1 ) {
                        tooltip += "\n";
                    }
                }

                // parameters
                for( G4int a=0; a<n_parameterEntry; a++ ) {
                    param = command->GetParameter(a);
                    params += "<" + param->GetParameterName()+"> ";
                }
                nMatch++;

                // Append to command model list
                QStandardItem* item = new QStandardItem(G4String(longCommandName + params).data());
                item->setData(0); // command
                //item->setIcon(QIcon(*fCommandIcon));
                item->setToolTip(tooltip.c_str());

                commandModelList.append(item);
            }
        }
    }

    QStandardItemModel* model = new QStandardItemModel();
    // initialize the model
    model->setColumnCount(1);

    // concat models
    for (int a= 0; a< dirModelList.size(); a++) {
        model->appendRow(dirModelList.at(a));
    }
    for (int a= 0; a< subDirModelList.size(); a++) {
        model->appendRow(subDirModelList.at(a));
    }
    for (int a= 0; a< commandModelList.size(); a++) {
        model->appendRow(commandModelList.at(a));
    }
    for (int a= 0; a< subCommandModelList.size(); a++) {
        model->appendRow(subCommandModelList.at(a));
    }

    return model;
}

void Simple::on_fCommandArea_returnPressed()
{
    UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand(ui->fCommandArea->text().toLatin1().data());
}



void Simple::on_addObject_clicked()
{



    QList <double> parameters;
    if(ui->shapesList->currentText()=="Box"){
        if(objectList.count()==0){ // world Box
            parameters.append(100);
            parameters.append(100);
            parameters.append(100);
        }
        else {
            parameters.append(10);
            parameters.append(10);
            parameters.append(10);
        }
    }
    else if(ui->shapesList->currentText()=="Cylinder")
    {
        parameters.append(0);
        parameters.append(10);
        parameters.append(10);
        parameters.append(0);
        parameters.append(360);
    }
    else if(ui->shapesList->currentText()=="Sphere")
    {
        parameters.append(9.9);
        parameters.append(10);
        parameters.append(0);
        parameters.append(360);
        parameters.append(0);
        parameters.append(180);
    }
    else if(ui->shapesList->currentText()=="Wedge")
    {
        parameters.append(10);
        parameters.append(10);
        parameters.append(10);
        parameters.append(0.1);
    }
    else if(ui->shapesList->currentText()=="Wedge")
    {
        parameters.append(10);
        parameters.append(10);
        parameters.append(10);
        parameters.append(0.1);
    }
    else if(ui->shapesList->currentText()=="Box Mesh")
    {
        parameters.append(10);
        parameters.append(10);
        parameters.append(10);
    }
    else if(ui->shapesList->currentText()=="Cylinder Mesh")
    {
        parameters.append(10);
        parameters.append(10);
    }
    G4ThreeVector position(0,0,0);
    G4ThreeVector rotation(0,0,0);
    G4ThreeVector mag(0,0,0);


    //    G4ThreeVector bins(1,1,1);
    //    if(ui->shapesList->currentText().contains("Mesh"))
    //    {
    //        SimpleObject *obj = new SimpleObject(ui->shapesList->currentText(),position,rotation,bins);
    //        obj->SetDimensions(parameters);
    //        obj->AppendMeasurementQuantity(ui->scoring_qty->currentText());
    //        obj->AppendMeasurementQuantityFilter(ui->scoring_filter->currentText());
    //        AddObject(obj);
    //    }
    // else{


    QString materialFormula =dbManager->GetMaterialFormula(ui->objMaterial->currentText());
    SimpleObject *obj = new SimpleObject(ui->shapesList->currentText(),position, rotation, mag,
                                         ui->objMaterial->currentText(),
                                         materialFormula,
                                         ui->objColor->currentText(),
                                         false);
    obj->SetDimensions(parameters);
    AddObject(obj);
    UpdateGeometry();
    //    }




}
void Simple::UpdateGeometry()
{
    SimpleDetectorConstruction* detector = (SimpleDetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    if(!is_gdml)
    detector->SetObjectList(objectList);

    G4RunManager::GetRunManager()->ReinitializeGeometry(); //Add here this line
    G4RunManager::GetRunManager()->GeometryHasBeenModified(); //
    //UImanager->ApplyCommand("/control/execute visupdate.mac");
    UpdateViewer();
    on_isWireframe_stateChanged(0);
}

void Simple::AddObject(QJsonObject json)
{
    QJsonArray arr = json["objects"].toArray();
    for(int i=0;i<arr.count();i++){
        QJsonObject object = arr.at(i).toObject();
        SimpleObject *obj = new SimpleObject(object);
        AddObject(obj);
    }


    QJsonArray arr2 = json["meshes"].toArray();
    for(int i=0;i<arr2.count();i++){
        QJsonObject object = arr2.at(i).toObject();
        SimpleMesh *mesh = new SimpleMesh(object);
        AddObject(mesh);
    }

    UpdateGeometry();
}


void Simple::AddObject(SimpleObject *obj)
{
    //simple_objects.append(obj);
    QStringList s = {obj->GetObjectName(),obj->GetMaterial()};
    QTreeWidgetItem *t = new QTreeWidgetItem(s);
    t->setFlags(t->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    t->setCheckState(0,Qt::Checked);
    t->setTextColor(0,obj->GetColor());
    ui->objectTree->insertTopLevelItem(ui->objectTree->topLevelItemCount(),t);
    objectList.append(obj);
    //    if(obj->isMesh())
    //        scoringObjects.append(obj);
    //    else {
    //        detectorObjects.append(obj);
    //    }

    if(objectList.count()>=1)
        ui->shapesList->setEnabled(true);
    ui->objectTree->setCurrentItem(t);
    on_objectTree_itemClicked(t,0);
    //UpdateGeometry();
    ui->addObject->setText("Add "+ ui->shapesList->currentText());

}

void Simple::AddObjects(QList<SimpleObject *> objects)
{
    foreach(SimpleObject* o, objects)
        AddObject(o);
}


void Simple::AddObject(SimpleMesh *mesh)
{
    //simple_objects.append(obj);
    QStringList s = {mesh->type()};
    QTreeWidgetItem *t = new QTreeWidgetItem(s);
    t->setFlags(t->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    t->setCheckState(0,Qt::Checked);
    //t->setTextColor(0,obj->GetColor());
    ui->meshes->insertTopLevelItem(ui->meshes->topLevelItemCount(),t);
    meshList.append(mesh);
    ui->meshes->setCurrentItem(t);
    on_meshes_itemClicked(t,0);

}

void Simple::AddObjects(QList <SimpleMesh*> meshes)
{
    foreach(SimpleMesh* mesh, meshes){
        AddObject(mesh);
    }
}

void Simple::on_shapesList_currentIndexChanged(int index)
{
    //  ui->parameters_stack->setCurrentIndex(index);
}

void Simple::on_hide_clicked()
{



}

void Simple::on_objectTree_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item!=NULL){
        int row = ui->objectTree->indexOfTopLevelItem(item);
        SimpleObject *obj = objectList.at(row);
        if(obj!=NULL){
            object_table_model->setObject(obj,row);
        }

        if(item->checkState(0)==Qt::Checked){
            UImanager->ApplyCommand(QString("/vis/geometry/set/visibility %1 0 true").arg(item->text(0)).toLatin1().data());
            obj->SetVisiblity(true);
        }
        else{
            UImanager->ApplyCommand(QString("/vis/geometry/set/visibility %1 0 false").arg(item->text(0)).toLatin1().data());
            obj->SetVisiblity(false);
        }
    }
}

void Simple::UpdateObject(SimpleObject *obj, int index)
{
    output("Object properties modified...");
    qDebug()<<"object properties modified...";
    UpdateGeometry();
}

void Simple::on_delete_object_clicked()
{
    QList <QTreeWidgetItem*> l= ui->objectTree->selectedItems();
    //if(l.count()==1){

    foreach(QTreeWidgetItem *t, l){
        int row = ui->objectTree->indexOfTopLevelItem(t);
        if(row!=0) { // row==0 is the World - do not allow to delete
            object_table_model->setObject(nullptr,-1); // set invalid pointers
            SimpleObject* obj =objectList.at(row);
            objectList.removeAt(row);
            //            if(obj->isMesh()){
            //                for (int ii=0;ii<scoringObjects.count();ii++) {
            //                    if(scoringObjects.at(ii)==obj)
            //                        scoringObjects.removeAt(ii);
            //                }
            //            }
            //            else {
            //                for (int ii=0;ii<detectorObjects.count();ii++) {
            //                    if(detectorObjects.at(ii)==obj)
            //                        detectorObjects.removeAt(ii);
            //                }
            //            }


            ui->objectTree->takeTopLevelItem(row);
        }
    }
    UpdateGeometry();
    // }
}

void Simple::on_objectTree_itemSelectionChanged()
{
    QList <QTreeWidgetItem*> l= ui->objectTree->selectedItems();
    if(l.count()==1){
        on_objectTree_itemClicked(l.at(0),0);
    }
}

void Simple::on_duplicate_clicked()
{
    QList <QTreeWidgetItem*> l= ui->objectTree->selectedItems();
    if(l.count()>0){
        QDialog dialog(this); // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog); // Add some text above the fields
        form.addRow(new QLabel("Duplicate objects"));


        QString label = "Axis";
        QComboBox *axis_choices = new QComboBox(&dialog);
        axis_choices->addItems({"along x","along y","along z"});
        form.addRow(label, axis_choices);

        label = "Number of copies";
        QSpinBox *nCopies = new QSpinBox(&dialog);
        nCopies->setRange(1,1000);
        form.addRow(label, nCopies);

        label = "Spacing between copies (mm)";
        QDoubleSpinBox *spacing = new QDoubleSpinBox(&dialog);
        spacing->setRange(-99999999,99999999);
        spacing->setDecimals(2);
        form.addRow(label, spacing);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        // Show the dialog as modal
        if (dialog.exec() == QDialog::Accepted) {
            int alongaxis=axis_choices->currentIndex();
            int ncopies = nCopies->value();
            double separation = spacing->value();

            foreach(QTreeWidgetItem *t, l){
                int row = ui->objectTree->indexOfTopLevelItem(t);
                if(row!=0) { // row==0 is the World - do not allow to duplicate
                    SimpleObject* obj = objectList.at(row);
                    SimpleObject* new_object =obj;
                    for(int ii=0; ii<ncopies;ii++){
                        new_object =  MakeCopy(new_object, alongaxis, separation);

                    }
                }
            }
            UpdateGeometry();

        }

    }
}

SimpleObject* Simple::MakeCopy(SimpleObject *obj, int alongaxis, float sep)
{
    G4ThreeVector new_position = obj->GetPosition();
    if(alongaxis==0)
        new_position.setX(new_position.x() + sep);
    else if(alongaxis==1)
        new_position.setY(new_position.y() + sep);
    else if(alongaxis==2)
        new_position.setZ(new_position.z() + sep);


    SimpleObject *new_object = new SimpleObject(obj->GetObjectType(),new_position,obj->GetRotation(),obj->GetMagField(),obj->GetMaterial(),obj->GetMaterialFormula(),obj->GetColor());

    new_object->SetDimensions(obj->GetDimensions());
    new_object->SetDataStorage(obj->storeData());
    AddObject(new_object);
    return new_object;
}

void Simple::on_db_type_currentIndexChanged(const QString &arg1)
{
    if(arg1=="Environment Variables")
    {
        db_table_model->setTable("env_vars");
        ui->addNewMaterial->hide();
    }
    else if (arg1=="Materials") {
        db_table_model->setTable("materials");
        ui->addNewMaterial->show();
    }
    db_table_model->select();
}

void Simple::on_objectTree_customContextMenuRequested(const QPoint &pos)
{
    QList <QTreeWidgetItem*> l= ui->objectTree->selectedItems();
    if(l.count()>0){ //only if items exist
        QTreeWidgetItem* t = l.at(0);
        if(!(l.count()==1 && (ui->objectTree->indexOfTopLevelItem(t)==0))) // do not execute for world
        {
            objectMenu->exec(QCursor::pos());
        }

    }
}


void Simple::on_isWireframe_stateChanged(int arg1)
{
    if(ui->isWireframe->isChecked())
        UImanager->ApplyCommand(QString("/vis/geometry/set/forceWireframe World 1 true ").toLatin1().data());
    else
        UImanager->ApplyCommand(QString("/vis/geometry/set/forceSolid World 1 true ").toLatin1().data());
}

void Simple::on_objectPropertiesTable_entered(const QModelIndex &index)
{

}


void Simple::on_hide_gunStack_clicked()
{
    if(ui->gunStack->isHidden())
        ui->gunStack->show();
    else {
        ui->gunStack->hide();
    }
}

void Simple::on_sel_sourceType_clicked()
{
    ui->gunStack->setCurrentIndex(0);
    ui->gunStack->show();
    //    ui->source_group->show();
    //    ui->energy_group->hide();
    //    ui->ang_group->hide();
    //    ui->gunStack->show();

}

void Simple::on_sel_sourceShape_clicked()
{
    //     ui->gunStack->setCurrentIndex(3);
    //    ui->source_group->hide();
    //    ui->energy_group->show();
    //    ui->ang_group->hide();
    //    ui->gunStack->show();
}

void Simple::on_sel_angDist_clicked()
{
    ui->gunStack->setCurrentIndex(1);
    ui->gunStack->show();
    //    ui->source_group->hide();
    //    ui->energy_group->hide();
    //    ui->ang_group->show();
    //    ui->gunStack->show();
}

void Simple::on_sel_energyDist_clicked()
{
    ui->gunStack->setCurrentIndex(2);
    ui->gunStack->show();
    //    ui->source_group->hide();
    //    ui->energy_group->show();
    //    ui->ang_group->hide();
    //    ui->gunStack->show();
}

void Simple::on_sel_pol_clicked()
{
    //    ui->gunStack->setCurrentIndex(4);
    //    ui->gunStack->show();
}

void Simple::on_particleSource_toggled(bool checked)
{
    if(checked){
        ui->particleType->show();
        ui->ion_A->hide();
        ui->ion_z->hide();
        ui->ion_alabel->hide();
        ui->ion_zlabel->hide();
    }
    else{
        ui->particleType->hide();
        ui->ion_A->show();
        ui->ion_z->show();
        ui->ion_alabel->show();
        ui->ion_zlabel->show();
    }

}

void Simple::on_sourceShape_currentIndexChanged(const QString &arg1)
{
    ui->shape_label1->setText("");
    ui->shape_label_2->setText("");
    ui->shape_label_3->setText("");

    if((arg1=="Rectangle") || (arg1=="Ellipse") )
    {
        ui->shape_label1->setText("half x (mm)");
        ui->shape_label_2->setText("half y (mm)");
        ui->shape_label_3->setText("");

    }

    if(arg1=="Cube"){
        ui->shape_label1->setText("half x (mm)");
        ui->shape_label_2->setText("half y (mm)");
        ui->shape_label_3->setText("half z (mm)");
    }

    if(arg1=="Cylinder"){
        ui->shape_label1->setText("radius (mm)");
        ui->shape_label_2->setText("half z (mm)");
        ui->shape_label_3->setText("");
    }

    if((arg1=="Circle") || (arg1=="Sphere") )
    {
        ui->shape_label1->setText("radius (mm)");
        //ui->shape_label1->setText("");
    }

    if((arg1=="Beam") )
    {
        ui->shape_label1->setText("Beam position spread (mm)");
        //ui->shape_label1->setText("");
    }
}

void Simple::on_eSpectType_currentIndexChanged(const QString &arg1)
{
    ui->energy_label1->setText("");
    ui->energy_label2->setText("");
    ui->energy_label3->setText("");

    if(arg1=="Monoenergetic")
    {
        ui->espectParam1->hide();
        ui->espectParam2->hide();
        ui->espectParam3->hide();
        ui->spectlabel->hide();

    }
    if(arg1=="Lin")
    {
        ui->energy_label1->setText("slope");
        ui->energy_label2->setText("intercept");
        ui->espectParam1->show();
        ui->espectParam2->show();
        ui->espectParam3->show();
        ui->spectlabel->show();
    }

    if(arg1=="Pow")
    {
        ui->energy_label1->setText("alpha");
        ui->espectParam1->show();
        ui->espectParam2->show();
        ui->espectParam3->show();
        ui->spectlabel->show();
    }

    if(arg1=="Exp")
    {
        ui->energy_label1->setText("ezero");
        ui->espectParam1->show();
        ui->espectParam2->show();
        ui->espectParam3->show();
        ui->spectlabel->show();
    }

}



QStringList Simple::CreateParticleSource(QString particle, QString sourceType, QString sourceShape, QVector3D sourceCentre, QList <float> sourceDims, QString angularType, QString energyDist, QList <float> eSpectParams, float eMin, float eMax, QVector3D directions, QVector3D pol)
{
    QStringList commands;

    //TODO: no ui variables should appear in this function, they should come as part of the arguments
    if(particle.contains(":")){ // ion source
        QString str= particle.split(":").at(1);
        commands.append(QString("/gps/particle ion"));
        commands.append(QString("/gps/ion %1").arg(str));
    }
    else     // particle source
        commands.append( QString("/gps/particle %1").arg(particle)); // particle type

    if(sourceType=="Point")
    {
        commands.append("/gps/pos/type Point");
    }
    //TODO
    //For a Plane this can be Circle, Annulus, Ellipse, Square, Rectangle. For both Surface or Volume sources this can be Sphere, Ellipsoid, Cylinder, Para (parallelepiped).

    else if((sourceType=="Rectangle") || (sourceType=="Circle") || (sourceType=="Elliptical"))
    {
        commands.append("/gps/pos/type Plane");
        commands.append(QString("/gps/pos/shape %1").arg(sourceType) );
    }

    //    else if((sourceType=="Sphere"))
    //    {
    //        commands.append("/gps/pos/type Surface");
    //        commands.append(QString("/gps/pos/shape %1").arg(sourceType) );
    //    }

    else if ((sourceType=="Cube" )|| (sourceType =="Cylinder") || (sourceType=="Sphere")) {
        commands.append("/gps/pos/type Volume");
        if(sourceType=="Cube")
            sourceType="Para";
        commands.append(QString("/gps/pos/shape %1").arg(sourceType) );
    }

    else if(sourceType.contains("Beam")){
        commands.append("/gps/pos/type Beam");
    }

    // source dimensions
    if((sourceType=="Rectangle") || (sourceType=="Elliptical"))
    {
        commands.append(QString("/gps/pos/halfx %1 mm").arg(sourceDims.at(0)) );
        commands.append(QString("/gps/pos/halfy %1 mm").arg(sourceDims.at(1)) );

    }

    else if((sourceType=="Circle") || (sourceType=="Sphere"))
    {
        commands.append(QString("/gps/pos/radius %1 mm").arg(sourceDims.at(0)) );
    }

    else if(sourceType=="Para"){ // Cube = Para --> changed a few lines above
        commands.append(QString("/gps/pos/halfx %1 mm").arg(sourceDims.at(0)) );
        commands.append(QString("/gps/pos/halfy %1 mm").arg(sourceDims.at(1)) );
        commands.append(QString("/gps/pos/halfz %1 mm").arg(sourceDims.at(2)) );
    }
    else if(sourceType=="Cylinder"){ // Cube = Para --> changed a few lines above
        commands.append(QString("/gps/pos/radius %1 mm").arg(sourceDims.at(0)) );
        commands.append(QString("/gps/pos/halfz %1 mm").arg(sourceDims.at(1)) );
    }
    else if(sourceType=="Beam"){
        commands.append(QString("/gps/pos/sigma_r %1 deg").arg(sourceDims.at(0)) );
    }


    // centre
    commands.append(QString("/gps/pos/centre %1 %2 %3 mm").arg(sourceCentre.x()).arg(sourceCentre.y()).arg(sourceCentre.z()));


    // if a angular distribution is given, directions will not be required, therefore this command should preced the ang. dist command so that the ang. dist commands always overrides it

    if(angularType=="none")
        commands.append(QString("/gps/direction %1 %2 %3").arg(directions.x()).arg(directions.y()).arg(directions.z()));

    else{
        // angular distribution
        if(angularType=="gauss" || angularType=="beam1d"){
            commands.append(QString("/gps/ang/sigma_r %1 deg").arg(ui->sigma->value()));
        }
        else { //TODO: the ui-> parameters should ideally be part of the function as arguments.
            commands.append(QString("/gps/ang/type %1").arg(angularType));
            commands.append(QString("/gps/ang/mintheta %1").arg(ui->theta_min->value()));
            commands.append(QString("/gps/ang/maxtheta %1").arg(ui->theta_max->value()));
            commands.append(QString("/gps/ang/minphi %1").arg(ui->phi_min->value()));
            commands.append(QString("/gps/ang/maxphi %1").arg(ui->phi_max->value()));
        }
    }
    // energy type


    // energy parameters
    if(energyDist=="Monoenergetic")
    {
        commands.append(QString("/gps/energy %1 MeV").arg(eMin));
    }

    else{
        commands.append(QString("/gps/ene/type %1").arg(energyDist));
        commands.append(QString("/gps/ene/min %1 MeV").arg(eMin));
        commands.append(QString("/gps/ene/max %1 MeV").arg(eMax));
    }

    if(energyDist=="Lin")
    {
        commands.append(QString("/gps/ene/gradient %1").arg(eSpectParams.at(0)));
        commands.append(QString("/gps/ene/intercept %1 ").arg(eSpectParams.at(1)));

    }

    if(energyDist=="Pow")
    {
        commands.append(QString("/gps/ene/alpha %1").arg(eSpectParams.at(0)));
    }

    if(energyDist=="Exp")
    {
        commands.append(QString("/gps/ene/ezero %1").arg(eSpectParams.at(0)));
    }



    commands.append(QString("/gps/polarization %1 %2 %3").arg(pol.x()).arg(pol.y()).arg(pol.z()));

    qDebug()<<commands;
    return commands;



}

void Simple::on_shoot_clicked()
{


    // ((GSRunAction*)(G4RunManager::GetRunManager()->GetUserRunAction()))->setRecordParameters(rp);

    InitRecordParameters();
    InitScoringParameters();
    ui->gunStack->hide();
    gsact->getGunPointer()->initialize();
    ui->progressBar->setMaximum(ui->nEvents->value());
    ui->progressBar->setValue(0);
    //gunprogress = 0;


    // on_execute_scoringCode_clicked();


    QVector3D sourceCentre;
    sourceCentre.setX(ui->posX->value());
    sourceCentre.setY(ui->posY->value());
    sourceCentre.setZ(ui->posZ->value());

    QList<float> sourceDims;
    sourceDims.append(ui->dimX->value());
    sourceDims.append(ui->dimY->value());
    sourceDims.append(ui->dimZ->value());

    QList<float> energyParams;
    energyParams.append(ui->espectParam1->value());
    energyParams.append(ui->espectParam2->value());
    energyParams.append(ui->espectParam3->value());

    QVector3D gunDirection(ui->dirx->value(),ui->diry->value(),ui->dirz->value());
    QVector3D polarization(ui->polx->value(),ui->poly->value(),ui->polz->value());

    QString particle;
    if(ui->particleSource->isChecked())
        particle=ui->particleType->currentText();
    else
        particle=QString("ion:%1 %2").arg(ui->ion_z->value()).arg(ui->ion_A->value());

    QStringList commands = CreateParticleSource(particle,

                                                ui->sourceShape->currentText(),
                                                ui->sourceShape->currentText(),
                                                sourceCentre,
                                                sourceDims,
                                                ui->angulardist->currentText(),
                                                ui->eSpectType->currentText(),
                                                energyParams,
                                                ui->emin->value(),
                                                ui->emax->value(),
                                                gunDirection,
                                                polarization
                                                );

    if(ui->gpsShapePlane->currentText()=="XY plane")
    {
        commands.append("/gps/pos/rot1 1 0 0");
        commands.append("/gps/pos/rot2 0 1 0");

    }
    if(ui->gpsShapePlane->currentText()=="YZ plane")
    {
        commands.append("/gps/pos/rot1 0 1 0");
        commands.append("/gps/pos/rot2 0 0 1");

    }
    if(ui->gpsShapePlane->currentText()=="ZX plane")
    {
        commands.append("/gps/pos/rot1 0 0 1");
        commands.append("/gps/pos/rot2 1 0 0");

    }
    for(int ii=0;ii<commands.count();ii++)
    {
        Execute(commands.at(ii));
    }


    QString showTracks = QString("/tracking/storeTrajectory %1").arg(ui->showTracks->isChecked());
    Execute(showTracks.toLatin1().data());

    QString accm = QString("/vis/scene/endOfEventAction accumulate 100");
    Execute(accm.toLatin1().data());
    QString beamOn = QString("/run/beamOn %1").arg(ui->nEvents->value());
    Execute(beamOn.toLatin1().data());
    //QString verbosity = QString("/vis/verbose 0");
    //Execute(verbosity);

    commands.append(showTracks);
    commands.append(accm);
    //   commands.append(verbosity);
    commands.append(beamOn);
    worksheet->SetCurrentFile("data/temp-det-output.root");
    foreach(SimpleMesh* mesh, meshList){
        mesh->writeToFile();
    }
    QCompleter *c = new QCompleter(worksheet->getHeader());
    ui->plotString->setCompleter(c);
    ui->cutString->setCompleter(c);


}


void Simple::Execute(QString command)
{
    UImanager->ApplyCommand(command.toLatin1().data());
}

void Simple::Execute(QStringList commands)
{
    for(int ii=0;ii<commands.count();ii++)
        Execute(commands.at(ii));
}

void Simple::InitScoringParameters()
{
    foreach(SimpleMesh* mesh, meshList){
        if(used_meshList.contains(mesh))
            mesh->makeUnique();
        else{
            mesh->createMesh();
        }

    }
    used_meshList = meshList;
}
void Simple::InitRecordParameters()
{
    // delete previous temporary files.
    QDir dir(QDir::current());
    dir.mkdir("data");
    dir.cd("data");
    QFileInfoList l = dir.entryInfoList();
    foreach(QFileInfo file, l){
        qDebug()<<file.fileName();
        if(file.fileName().contains("temp-"));
        dir.remove(file.fileName());
    }


    foreach(SimpleObject* o, objectList)
    {
        o->SetRecordParameters(ui->recordPosition->isChecked(),
                               ui->recordTiming->isChecked(),
                               ui->recordEnergy->isChecked(),
                               ui->recordMomentum->isChecked(),
                               ui->recordProcessName->isChecked(),
                               ui->filterParticleIds->text());
    }

    SimpleDetectorConstruction* det = (SimpleDetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();

    det->setRecordParameter(ui->recordPosition->isChecked(),
                            ui->recordTiming->isChecked(),
                            ui->recordEnergy->isChecked(),
                            ui->recordMomentum->isChecked(),
                            ui->recordProcessName->isChecked());
}

void Simple::on_nthreads_valueChanged(const QString &arg1)
{
    runManager->SetNumberOfThreads(arg1.toInt());//
}

void Simple::on_g4_verbosity_currentIndexChanged(const QString &arg1)
{
    //    QString verbosity = QString("/vis/verbose %1").arg(ui->g4_verbosity->currentIndex());
    //    Execute(verbosity);
}

void Simple::on_clearOutput_clicked()
{
    ui->out->clear();
}


void Simple::on_shapesList_currentIndexChanged(const QString &arg1)
{
    ui->addObject->setText("Add "+ ui->shapesList->currentText());
    //    if(!arg1.contains("Mesh")){
    //        ui->scoringqty_label->hide();
    //        ui->scoring_qty->hide();
    //        ui->scoringfilter_label->hide();
    //        ui->scoring_filter->hide();

    //        ui->material_label->show();
    //        ui->objectColor_label->show();
    //        ui->objMaterial->show();
    //        ui->objColor->show();
    //    }
    //    else{
    //        ui->scoringqty_label->show();
    //        ui->scoring_qty->show();
    //        ui->scoringfilter_label->show();
    //        ui->scoring_filter->show();

    //        ui->material_label->hide();
    //        ui->objectColor_label->hide();
    //        ui->objMaterial->hide();
    //        ui->objColor->hide();
    //    }
}

void Simple::on_export_dataframe_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enter file name with .root or .txt extension"),                           "",
                                                    tr("(*.root *.txt)"));
    if(fileName!="")
    {
        if(fileName.contains(".root")){
            worksheet->saveData(fileName, "ROOT");
        }
        else if(fileName.contains(".txt")){
            worksheet->saveData(fileName, "txt");
        }
    }
}

void Simple::on_filterDataframe_clicked()
{
    worksheet->ApplyFilter(ui->filterString->text());
}

void Simple::on_filterString_editingFinished()
{
    on_filterDataframe_clicked();
}

void Simple::on_plot_clicked()
{
    worksheet->PlotData(ui->plotString->text(),ui->cutString->text(),ui->plot_options->text());
}

void Simple::on_plotString_returnPressed()
{
    on_plot_clicked();
}

void Simple::on_cutString_returnPressed()
{
    on_plot_clicked();
}

void Simple::on_plot_options_returnPressed()
{
    on_plot_clicked();
}

void Simple::on_pushButton_clicked()
{

    Execute("/score/colorMap/floatMinMax");
    //Execute(QString("/score/drawProjection %1 %2%1").arg(scoringObjects.at(0)->GetObjectName()).arg(scoringObjects.at(0)->MeasurementQuantities().at(0)));

}

void Simple::on_createMesh_clicked()
{
    //    if(ui->meshShape->selectedItems().count()==0){
    //        QMessageBox::warning(this,"Mesh shape not selected", "Atleast one mesh shape should be selected from the list!");
    //        return;
    //    }
    //    if(ui->scoring_qty->selectedItems().count()==0) {
    //        QMessageBox::warning(this,"Scoring parameter not selected", "Atleast one measurement parameter should be selected from the list!");
    //        return;
    //    }

}

void Simple::on_clear_scoring_commands_clicked()
{

}

void Simple::on_meshShape_itemClicked(QTreeWidgetItem *item, int column)
{
    //    QString objType = item->text(0);
    //    QString createMesh_command;
    //    QString meshSize_command;
    //    if(objType=="Box"){
    //        createMesh_command = QString("/score/create/boxMesh Box-Mesh-%1").arg(mesh_id++,3,10, QChar('0');
    //        meshSize_command = QString("/score/mesh/boxSize 10 10 10 mm");
    //    }
    //    else if(objType=="Cylinder"){
    //        createMesh_command = QString("/score/create/cylinderMesh Cyl-Mesh-%1").arg(mesh_id++,3,10, QChar('0');
    //        meshSize_command = QString("/score/mesh/cylinderSize 10 10 mm");
    //    }

    //    QString meshPos_command = QString("/score/mesh/translate/xyz 0 0 0 mm");

    //    QString meshrotx_command = QString("/score/mesh/rotate/rotateX 0 deg")));
    //    QString meshroty_command = QString("/score/mesh/rotate/rotateY 0 deg");
    //    QString meshrotz_command = QString("/score/mesh/rotate/rotateZ 0 deg");


    //    QString meshBin_command = QString("/score/mesh/nBin %1 %2 %3").arg(bins.x()).arg(bins.y()).arg(bins.z());

}

void Simple::on_add_box_mesh_clicked()
{

    SimpleMesh *mesh = new SimpleMesh("Box",ui->scoringParameter->currentText(),
                                      ui->charge_filter->isChecked(),
                                      ui->neutral_filter->isChecked(),
                                      ui->particle_id_filter->isChecked(),
                                      ui->scoring_pid_filter->currentText(),
                                      ui->ke_filter->isChecked(),
                                      ui->scoring_ke_min->value(),
                                      ui->scoring_ke_max->value());
    AddObject(mesh);
}


void Simple::on_add_cylindrical_mesh_clicked()
{
    SimpleMesh *mesh = new SimpleMesh("Cyl",ui->scoringParameter->currentText(),
                                      ui->charge_filter->isChecked(),
                                      ui->neutral_filter->isChecked(),
                                      ui->particle_id_filter->isChecked(),
                                      ui->scoring_pid_filter->currentText(),
                                      ui->ke_filter->isChecked(),
                                      ui->scoring_ke_min->value(),
                                      ui->scoring_ke_max->value());
    AddObject(mesh);
}

void Simple::on_meshes_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item!=nullptr){
        int row =ui->meshes->indexOfTopLevelItem(item);
        mesh_table_model->setMesh(meshList.at(row));
    }
}

void Simple::delete_current_mesh()
{

    if(ui->meshes->selectedItems().count()==1){
        QTreeWidgetItem* t = ui->meshes->selectedItems().at(0);
        int row = ui->meshes->indexOfTopLevelItem(t);
        meshList.removeAt(row);
        ui->meshes->takeTopLevelItem(row);

    }

}

void Simple::plot_mesh_projection()
{
    if(ui->meshes->selectedItems().count()==1){
        QTreeWidgetItem* t = ui->meshes->selectedItems().at(0);
        int row = ui->meshes->indexOfTopLevelItem(t);
        meshList.at(row)->plotProjection();

    }
}

void Simple::plot_mesh_column()
{
    QList <QTreeWidgetItem*> l= ui->meshes->selectedItems();
    if(l.count()==1){
        QDialog dialog(this); // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog); // Add some text above the fields
        form.addRow(new QLabel("Plane"));


        QString label = "Axis";
        QComboBox *axis_choices = new QComboBox(&dialog);
        axis_choices->addItems({"X","Y","Z"});
        form.addRow(label, axis_choices);

        label = "Column";
        QSpinBox *column= new QSpinBox(&dialog);
        column->setRange(0,100000000);
        form.addRow(label, column);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        // Show the dialog as modal
        if (dialog.exec() == QDialog::Accepted) {
            QTreeWidgetItem* t = ui->meshes->selectedItems().at(0);
            int row = ui->meshes->indexOfTopLevelItem(t);
            meshList.at(row)->plotColumn(axis_choices->currentText(),column->value());
        }
    }
}
void Simple::on_meshes_customContextMenuRequested(const QPoint &pos)
{
    QList <QTreeWidgetItem*> l= ui->meshes->selectedItems();
    if(l.count()>0){ //only if items exist
        meshMenu->exec(QCursor::pos());
    }
}

void Simple::on_setLogX_clicked(bool checked)
{
    if(gPad!=nullptr)
        gPad->SetLogx(checked);

}

void Simple::on_setLogY_clicked(bool checked)
{
    if(gPad!=nullptr)
        gPad->SetLogy(checked);
}

void Simple::on_run_macro_clicked()
{
    QStringList mac = ui->macros->toPlainText().split("\n");
    Execute(mac);
    //Execute("/score/drawProjection Box-Mesh-000 cellFluxBox-Mesh-000");
}

bool Simple::AddTabWidget(QWidget *w, const QString s)
{
    qDebug()<<"Add tabwidget";
    ui->fViewerTabWidget->addTab(w, s);
}

QTabWidget* Simple::GetViewerTabWidget()
{
    return ui->fViewerTabWidget;
}

void Simple::on_clear_all_mesh_clicked()
{
    ui->meshes->clear();
    meshList.clear();

}

void Simple::on_fileDirectoryView_doubleClicked(const QModelIndex &index)
{

    QString fileName = fileSystemModel->filePath(index);
    if(fileName!="" && fileName.contains(".root")){
        worksheet->SetCurrentFile(fileName);
        QCompleter *c = new QCompleter(worksheet->getHeader());
        ui->plotString->setCompleter(c);
        ui->cutString->setCompleter(c);
    }
}

QJsonArray Simple::gunParams()
{
    QJsonArray arr;
    QJsonObject gunSourceJsonObject;


    gunSourceJsonObject["nevents"] = ui->nEvents->value();
    gunSourceJsonObject["showevents"] = ui->showTracks->isChecked();


    gunSourceJsonObject["particleSource"] = ui->particleSource->isChecked();
    gunSourceJsonObject["particleType"] = ui->particleType->currentText();
    gunSourceJsonObject["ionZ"] = ui->ion_z->value();
    gunSourceJsonObject["ionA"] = ui->ion_A->value();
    gunSourceJsonObject["sourceShape"] = ui->sourceShape->currentText();
    gunSourceJsonObject["gpsshapePlane"] = ui->gpsShapePlane->currentText();

    gunSourceJsonObject["dimX"] = ui->dimX->value();
    gunSourceJsonObject["dimY"] = ui->dimY->value();
    gunSourceJsonObject["dimZ"] = ui->dimZ->value();

    gunSourceJsonObject["posX"] = ui->posX->value();
    gunSourceJsonObject["posY"] = ui->posY->value();
    gunSourceJsonObject["posZ"] = ui->posZ->value();

    QJsonObject gunAngJsonObject;

    gunAngJsonObject["angularDist"] = ui->angulardist->currentText();
    gunAngJsonObject["sigma"] = ui->sigma->value();

    gunAngJsonObject["dirx"] = ui->dirx->value();
    gunAngJsonObject["diry"] = ui->diry->value();
    gunAngJsonObject["dirz"] = ui->dirz->value();

    gunAngJsonObject["theta_min"] = ui->theta_min->value();
    gunAngJsonObject["theta_max"] = ui->theta_max->value();
    gunAngJsonObject["phi_min"] = ui->phi_min->value();
    gunAngJsonObject["phi_max"] = ui->phi_max->value();


    QJsonObject gunEnergyJsonObject;
    gunEnergyJsonObject["espectType"] = ui->eSpectType->currentText();
    gunEnergyJsonObject["espectParam1"] = ui->espectParam1->value();
    gunEnergyJsonObject["espectParam2"] = ui->espectParam2->value();
    gunEnergyJsonObject["espectParam3"] = ui->espectParam3->value();


    gunEnergyJsonObject["emin"] = ui->emin->value();
    gunEnergyJsonObject["emax"] = ui->emax->value();

    gunEnergyJsonObject["polx"] = ui->polx->value();
    gunEnergyJsonObject["poly"] = ui->poly->value();
    gunEnergyJsonObject["polz"] = ui->polz->value();

    arr.append(gunSourceJsonObject);
    arr.append(gunAngJsonObject);
    arr.append(gunEnergyJsonObject);

    return arr;
}

void Simple::SaveProject(QString fileName)
{
    QJsonArray objectArray, meshArray, recordArray, uiArray;
    qDebug()<<objectList.count()<<meshList.count();

    foreach(SimpleObject* obj, objectList){
        objectArray.append( obj->jsonobject());
    }
    foreach(SimpleMesh* obj, meshList){
        meshArray.append( obj->jsonobject());
    }


    QJsonObject o;
    o["macro-script"] = ui->macros->toPlainText();
    o["scratch"] = ui->scratch->toPlainText();
    o["active-out-widget"] = ui->outtabWidget->currentIndex();
    uiArray.append(o);

    QJsonObject recordJsonObject;
    recordJsonObject["recordPosition"] =ui->recordPosition->isChecked();
    recordJsonObject["recordTiming"] =ui->recordTiming->isChecked();
    recordJsonObject["recordEnergy"] =ui->recordEnergy->isChecked();
    recordJsonObject["recordMomentum"] =ui->recordMomentum->isChecked();
    recordJsonObject["recordProcessName"] =ui->recordProcessName->isChecked();
    recordJsonObject["filterParticleIds"] = ui->filterParticleIds->text();
    recordArray.append(recordJsonObject);


    QJsonArray arr;
    QJsonObject json;
    json["objects"] = objectArray;
    json["meshes"] = meshArray;
    json["record"] = recordArray;
    json["ui"] = uiArray;
    json["physics"] = currentPhysicsList;
    json["gun"]=gunParams();

    if(!fileName.contains(".sim"))
        fileName.append(".sim");

    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return ;
    }

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
    last_opened_file = fileName;
    setWindowTitle(QString("Simple (%1)").arg(fileName));
}
void Simple::on_actionSave_triggered()
{
    if(last_opened_file!="")
        SaveProject(last_opened_file);
    else
        on_actionSave_as_triggered();
}


void Simple::SetRecordParameters(QJsonObject jsonObject)
{
    QJsonArray arr = jsonObject["record"].toArray();
    jsonObject = arr.at(0).toObject();
    ui->recordPosition->setChecked(jsonObject["recordPosition"].toBool());
    ui->recordTiming->setChecked(jsonObject["recordTiming"].toBool());
    ui->recordEnergy->setChecked(jsonObject["recordEnergy"].toBool());
    ui->recordMomentum->setChecked( jsonObject["recordMomentum"].toBool());
    ui->recordProcessName->setChecked(jsonObject["recordProcessName"].toBool());
    ui->filterParticleIds->setText(jsonObject["filterParticleIds"].toString());
}

void Simple::SetUIParameters(QJsonObject jsonObject)
{
    QJsonArray arr = jsonObject["ui"].toArray();
    jsonObject = arr.at(0).toObject();

    ui->macros->setPlainText(jsonObject["macro-script"].toString());
    ui->scratch->setPlainText(jsonObject["scratch"].toString());
    ui->outtabWidget->setCurrentIndex(jsonObject["active-out-widget"].toInt());
}

void Simple::SetGunParameters(QJsonObject jsonObject)
{
    QJsonArray arr = jsonObject["gun"].toArray();
    jsonObject = arr.at(0).toObject();
    ui->nEvents->setValue(jsonObject["nevents"].toInt());
    ui->showTracks->setChecked(jsonObject["showevents"].toBool());
    if(jsonObject["particleSource"].toBool())
        ui->particleSource->setChecked(true);
    else {
        ui->ionSource->setChecked(true);
    }
    ui->particleType->setCurrentText(jsonObject["particleType"].toString());
    ui->ion_z->setValue(jsonObject["ionZ"].toInt());
    ui->ion_A->setValue(jsonObject["ionA"].toInt());
    ui->sourceShape->setCurrentText(jsonObject["sourceShape"].toString());
    ui->gpsShapePlane->setCurrentText(jsonObject["gpsshapePlane"].toString());

    ui->dimX->setValue(jsonObject["dimX"].toDouble());
    ui->dimY->setValue(jsonObject["dimY"].toDouble());
    ui->dimZ->setValue(jsonObject["dimZ"].toDouble());

    ui->posX->setValue(jsonObject["posX"].toDouble());
    ui->posY->setValue(jsonObject["posY"].toDouble());
    ui->posZ->setValue(jsonObject["posZ"].toDouble());

    // QJsonObject gunAngJsonObject;
    jsonObject = arr.at(1).toObject();
    ui->angulardist->setCurrentText(jsonObject["angularDist"].toString());
    ui->sigma->setValue(jsonObject["sigma"].toDouble());
    ui->dirx->setValue(jsonObject["dirx"].toDouble());
    ui->diry->setValue(jsonObject["diry"].toDouble());
    ui->dirz->setValue(jsonObject["dirz"].toDouble());

    ui->theta_min->setValue(jsonObject["theta_min"].toDouble());
    ui->theta_max->setValue(jsonObject["theta_max"].toDouble());

    ui->phi_min->setValue(jsonObject["phi_min"].toDouble());
    ui->phi_max->setValue(jsonObject["phi_max"].toDouble());

    jsonObject = arr.at(2).toObject();
    ui->eSpectType->setCurrentText(jsonObject["espectType"].toString());
    ui->espectParam1->setValue(jsonObject["espectParam1"].toDouble());
    ui->espectParam2->setValue(jsonObject["espectParam2"].toDouble());
    ui->espectParam3->setValue(jsonObject["espectParam3"].toDouble());

    ui->emin->setValue(jsonObject["emin"].toDouble());
    ui->emax->setValue(jsonObject["emax"].toDouble());

    ui->polx->setValue(jsonObject["polx"].toDouble());
    ui->poly->setValue(jsonObject["poly"].toDouble());
    ui->polz->setValue(jsonObject["polz"].toDouble());


}

void Simple::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open geometry files"),
                                                    QDir::currentPath(),
                                                    tr("Simulation Files (*.sim);;GDML files (*.gdml)"));

    //    if(!fileName.contains(".sim"))
    //        fileName.append(".sim");

    if(fileName.contains(".gdml")){
        is_gdml=true;
        SimpleDetectorConstruction* detector = (SimpleDetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
        detector->SetParser(fileName);
        UpdateGeometry();
    }

    else{
        is_gdml=false;
        QFile openFile(fileName);
        if (!openFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return ;
        }

        QByteArray data = openFile.readAll();

        QJsonDocument loadDoc(QJsonDocument::fromJson(data));
        QJsonObject json = loadDoc.object();

        if(json["physics"].toString()!=currentPhysicsList)
        {
            QMessageBox msgBox;
            QString err = QString("This project was saved with the %1 physics list but the current physics list is %2.\nPlease restart the program to change the physics list.").arg(json["physics"].toString()).arg(currentPhysicsList);
            msgBox.setText(err);
            msgBox.exec();
        }


        ui->objectTree->clear();
        ui->meshes->clear();
        objectList.clear();
        meshList.clear();


        AddObject(json);
        SetRecordParameters(json);
        SetUIParameters(json);
        SetGunParameters(json);

        openFile.close();
        last_opened_file = fileName;

    }
    setWindowTitle(QString("Simple (%1)").arg(fileName));


}

void Simple::InitViewer()
{

    QFile file("visupdate.mac");
    if(file.exists()){
        Execute("/control/execute visinit.mac");
        return;
    }
    qDebug()<<"Executing init commands from internal list";
    QStringList commands ={
        "/run/initialize",
        "/vis/open OGLSQt 800x800-0+0",
        "/vis/viewer/set/background black",
        "/vis/drawVolume",
        "/vis/viewer/set/viewpointVector -1 0 1",
        "/vis/viewer/set/lightsVector -1 0 0",
        "/vis/viewer/set/viewpointThetaPhi 120 150",
        "/vis/viewer/set/style wireframe",
        "/vis/scene/add/trajectories smooth",
        "/vis/modeling/trajectories/create/drawByCharge",
        "/vis/scene/add/hits",
        "/vis/scene/endOfEventAction accumulate 100",
        "/vis/scene/add/scale",
        "/vis/scene/add/axes",
        "/vis/ogl/set/displayListLimit 500000000"
    };

    Execute(commands);

}

void Simple::UpdateViewer()
{

    QFile file("visupdate.mac");
    if(file.exists()){
        Execute("/control/execute visupdate.mac");
        return;
    }
    else{
        qDebug()<<"Executing viewer commands from internal list";
        QStringList commands=
        {"/run/initialize",
         "/vis/viewer/set/autoRefresh true",
         "/vis/drawVolume",
         "/vis/viewer/set/viewpointVector -1 0 1",
         "/vis/viewer/set/viewpointThetaPhi 120 150",
         "/vis/viewer/set/lightsVector -1 0 0",
         "/vis/viewer/set/auxiliaryEdge true",
         "/vis/viewer/set/lineSegmentsPerCircle 100",
         "/vis/scene/add/scale",
         "/vis/scene/add/axes",
         "/vis/scene/add/eventID",
         "/vis/scene/add/trajectories smooth",
         "/vis/scene/endOfEventAction accumulate 100",
         "/vis/modeling/trajectories/create/drawByCharge",
         "/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true",
         "/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2",
         "/vis/scene/add/hits",
         "/vis/geometry/set/visibility World 0 true",
         "/vis/viewer/flush"};
        Execute(commands);
    }
}
void Simple::on_download_g4_data_files_clicked()
{
    QDesktopServices::openUrl(QUrl("https://geant4.web.cern.ch/support/download"));
}

void Simple::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QDir::currentPath(),
                                                    tr("Simulation Files (*.sim)"));
    if(fileName!="")SaveProject(fileName);
}

void Simple::on_actionExit_2_triggered()
{
    close();
}

void Simple::on_actionBox_triggered()
{
    ui->shapesList->setCurrentText("Box");
    on_addObject_clicked();
    ui->sceneTreeWidget->setCurrentIndex(0);
}

void Simple::on_actionCylinder_triggered()
{
    ui->shapesList->setCurrentText("Cylinder");
    on_addObject_clicked();
    ui->sceneTreeWidget->setCurrentIndex(0);
}

void Simple::on_actionSphere_triggered()
{
    ui->shapesList->setCurrentText("Sphere");
    on_addObject_clicked();
    ui->sceneTreeWidget->setCurrentIndex(0);
}

void Simple::on_actionWedge_triggered()
{
    ui->shapesList->setCurrentText("Wedge");
    on_addObject_clicked();
    ui->sceneTreeWidget->setCurrentIndex(0);
}

void Simple::on_actionBox_Mesh_triggered()
{
    on_add_box_mesh_clicked();
    ui->sceneTreeWidget->setCurrentIndex(1);
}

void Simple::on_actionCylinder_Mesh_triggered()
{
    on_add_cylindrical_mesh_clicked();
    ui->sceneTreeWidget->setCurrentIndex(1);
}

void Simple::on_actionShoot_beam_triggered()
{
    on_shoot_clicked();
}

void Simple::on_actionMacro_triggered()
{
    ui->outtabWidget->setCurrentIndex(2);
    on_run_macro_clicked();
}

void Simple::on_actionEnvironment_variables_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(1);
    ui->toolBox->setCurrentIndex(0);
    ui->db_type->setCurrentIndex(0);
    on_db_type_currentIndexChanged(ui->db_type->itemText(0));

}

void Simple::on_actionMaterial_list_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(1);
    ui->toolBox->setCurrentIndex(0);
    ui->db_type->setCurrentIndex(0);
    on_db_type_currentIndexChanged(ui->db_type->itemText(1));
}

void Simple::on_actionDetecor_view_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(0);
}

void Simple::on_actionPlots_triggered()
{
    ui->outtabWidget->setCurrentIndex(1);
}

void Simple::on_actionScene_Object_Window_triggered()
{
    ui->sceneTreeWidget->setCurrentIndex(0);
}

void Simple::on_actionMesh_Window_triggered()
{
    ui->sceneTreeWidget->setCurrentIndex(1);
}

void Simple::on_actionProperty_Editor_triggered()
{
    ui->sceneTreeWidget->setCurrentIndex(2);
}

void Simple::on_actionOutput_file_settings_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(1);
    ui->toolBox->setCurrentIndex(1);

}

void Simple::on_actionSource_Type_triggered()
{

}

void Simple::on_actionBeam_energy_settings_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(0);
    ui->gunStack->show();
    ui->gunStack->setCurrentIndex(2);
}

void Simple::on_actionBeam_type_settings_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(0);
    ui->gunStack->show();
    ui->gunStack->setCurrentIndex(0);
}

void Simple::on_actionBeam_angular_distribution_triggered()
{
    ui->fViewerTabWidget->setCurrentIndex(0);
    ui->gunStack->show();
    ui->gunStack->setCurrentIndex(1);
}

void Simple::on_actionHelp_triggered()
{
    QMessageBox::information(this, tr("Developer"),
                             tr("Please write to me at deepaksamuel@gmail.com"));
}

void Simple::on_build_from_materials_toggled(bool checked)
{
    //    if(checked){
    //        ui->materialStack->setCurrentIndex(1);
    //    }
    //    else
    //        ui->materialStack->setCurrentIndex(0);
}

void Simple::on_addNewMaterial_clicked()
{
    mat_builder->ShowUI();
}

void Simple::on_actionNew_Material_triggered()
{
    mat_builder->ShowUI();
}

void Simple::on_actionMaterial_property_triggered()
{
    SimpleMaterialPropertyBuilder::getInstance()->ShowUI();
}

void Simple::on_switch_off_mag_fields_stateChanged(int arg1)
{
    foreach(SimpleObject *o, objectList)
        o->setMagFieldOff(ui->switch_off_mag_fields->isChecked());
    UpdateGeometry();
}

void Simple::on_auto_search_g4variables_clicked()
{
    QMessageBox::information(this, tr("Warning"),
                             tr("For this process to work properly, all your geant4 data files must be inside a single folder, say G4DATA, which you will set in the next dialog box. The program will then automatically set the paths to the datasets."));

    QString dir = QFileDialog::getExistingDirectory(this, "Select a directory inside which all your G4 datasets reside..");

    QDirIterator dirIt(dir, QDirIterator::Subdirectories);

    QStringList dir_keys{"pixe/alpha/l/pwba/l3-24.dat","z72.a185","RoughESRGrease_LUTR.dat","pi0pi0.dat","ENSDFSTATE.dat","table_radius_hfb.dat","frldm.dat","ThermalScattering/Coherent/FS/al_metal.z","livermore/comp/ce-cs-47.dat","z65.a150","alpha/inel34"};
    QStringList g4_ds_env_var{"G4PIIDATA","G4RADIOACTIVEDATA","G4REALSURFACEDATA","G4SAIDXSDATA","G4ENSDFSTATEDATA","G4INCLDATA","G4ABLADATA","G4NEUTRONHPDATA","G4LEDATA","G4LEVELGAMMADATA","G4PARTICLEXSDATA"};
    // QStringList g4_ds_env_paths;


    // we search for the keys in the folder and retrieve the folder name. This will ensure that this works even if the version of the dataset changes
    while(dirIt.hasNext()){
        //qDebug()<<dirIt.next();
        QString folder = dirIt.next();
        //int i=0;
        if(folder.contains(dir_keys.at(0))){
            folder= folder.remove(dir_keys.at(0));
            dbManager->SetEnvVar(g4_ds_env_var.at(0),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(0)).arg(folder));

        }

        else if(folder.contains(dir_keys.at(1)) && folder.contains("RadioactiveDecay")){
            folder= folder.remove(dir_keys.at(1));
            dbManager->SetEnvVar(g4_ds_env_var.at(1),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(1)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(2))){
            folder= folder.remove(dir_keys.at(2));
            dbManager->SetEnvVar(g4_ds_env_var.at(2),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(2)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(3))){
            folder= folder.remove(dir_keys.at(3));
            dbManager->SetEnvVar(g4_ds_env_var.at(3),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(3)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(4))){
            folder= folder.remove(dir_keys.at(4));
            dbManager->SetEnvVar(g4_ds_env_var.at(4),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(4)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(5))){
            folder= folder.remove(dir_keys.at(5));
            dbManager->SetEnvVar(g4_ds_env_var.at(5),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(5)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(6))){
            folder= folder.remove(dir_keys.at(6));
            dbManager->SetEnvVar(g4_ds_env_var.at(6),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(6)).arg(folder));
        }

        else if(folder.contains(dir_keys.at(7))){
            folder= folder.remove(dir_keys.at(7));
            dbManager->SetEnvVar(g4_ds_env_var.at(7),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(7)).arg(folder));
        }
        else if(folder.contains(dir_keys.at(8))){
            folder= folder.remove(dir_keys.at(8));
            dbManager->SetEnvVar(g4_ds_env_var.at(8),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(8)).arg(folder));
        }
        else if(folder.contains(dir_keys.at(9)) && folder.contains("PhotonEvaporation")){
            folder= folder.remove(dir_keys.at(9));
            dbManager->SetEnvVar(g4_ds_env_var.at(9),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(9)).arg(folder));
        }
        else if(folder.contains(dir_keys.at(10))){
            folder= folder.remove(dir_keys.at(10));
            dbManager->SetEnvVar(g4_ds_env_var.at(10),folder);
            output(QString("Setting %1 to %2").arg(g4_ds_env_var.at(10)).arg(folder));
        }

    }
    //refresh
    ui->db_type->setCurrentIndex(1);
    ui->db_type->setCurrentIndex(0);


    // G4_ENV_VARIABLE    FILE INSIDE THE FOLDER                  DATASET LOCATION
    //0 G4PIIDATA          g4data/G4PII.1.3/G4PII1.3/pixe/alpha/l/pwba/l3-24.dat                                   /home/samuel/g4/g4-data/G4PII1.3
    //1 G4RADIOACTIVEDATA  g4data/G4RadioactiveDecay.5.3/RadioactiveDecay5.3/z72.a185                             /home/samuel/g4/g4-data/RadioactiveDecay5.3
    //2 G4REALSURFACEDATA  g4data/G4RealSurface.2.1.1/RealSurface2.1.1/RoughESRGrease_LUTR.dat                  /home/samuel/g4/g4-data/RealSurface2.1.1
    //3 G4SAIDXSDATA       g4data/G4SAIDDATA.2.0/G4SAIDDATA2.0/pi0pi0.dat                                        /home/samuel/g4/g4-data/G4SAIDDATA2.0
    //4 G4ENSDFSTATEDATA   g4data/G4ENSDFSTATE.2.2/G4ENSDFSTATE2.2/ENSDFSTATE.dat                            /home/samuel/g4/g4-data/G4ENSDFSTATE2.2
    //5 G4INCLDATA         g4data/G4INCL.1.0/G4INCL1.0/table_radius_hfb.dat                            /home/samuel/g4/g4-data/G4INCL1.0
    //6 G4ABLADATA         g4data/G4ABLA.3.1/G4ABLA3.1/frldm.dat                                     /home/samuel/g4/g4-data/G4ABLA3.1
    //7 G4NEUTRONHPDATA    g4data/G4NDL.4.5/G4NDL4.5/ThermalScattering                                  /home/samuel/g4/g4-data/G4NDL4.5
    //8 G4LEDATA           g4data/G4EMLOW.7.7/G4EMLOW7.7/comp/ce-cs-47.dat                              /home/samuel/g4/g4-data/G4EMLOW7.7
    //9 G4LEVELGAMMADATA   g4data/G4PhotonEvaporation.5.3/PhotonEvaporation5.3/z65.a150                 /home/samuel/g4/g4-data/PhotonEvaporation5.3
    //10 G4PARTICLEXSDATA   g4data/G4PARTICLEXS.2.0/G4PARTICLEXS2.0/alpha/inel34                           /home/samuel/g4/g4-data/G4PARTICLEXS2.0


}

void Simple::on_actionExport_triggered()
{
    G4GDMLParser parser;
    parser.SetRegionExport(true);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to GDML format..."),                           "",
                                                    tr("(*.gdml)"));
    if(!fileName.contains(".gdml"))
        fileName = fileName+(".gdml");

    parser.Write(fileName.toLatin1().data(),G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());





}
