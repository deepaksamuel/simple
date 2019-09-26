#include "iCanvas.h"

// File modified by Deepak Samuel on 25 Sep 2019


int                 iCanvas::canvasN        =0;

QStringList         iCanvas::markerNames;
QStringList         iCanvas::lineNames;
QStringList         iCanvas::fillNames;
QColorDialog*       iCanvas::colorDialog    =0;

bool                iCanvas::logX       =false;
bool                iCanvas::logY       =false;
bool                iCanvas::gridX      =false;
bool                iCanvas::gridY      =false;
bool                iCanvas::highlight  =false;


iCanvas::iCanvas(QWidget *parent):QWidget(parent)
{
    setMinimumSize(75, 75);
    setMouseTracking(kTRUE);
    int wid     = gVirtualX->AddWindow((ULong_t)winId(),75,75);
    fCanvas     = new TCanvas("glRootCanvas",width(),height(),wid);

    if(colorDialog==0){
        colorDialog =   new QColorDialog();
        //colorDialog->setOption(QColorDialog::NoButtons);
    }

    closed      = false;
    highlight   = false;

    markerNames << "dot"<<"plus"<<"star"<<"circle"<<"cross"<<"small dot"<<"medium dot"
                <<"large dot"<<"full circle"<<"full square"<<"full triangle up"<<"full triangle down"
               <<"open circle"<<"open square"<<"open triangle up"<<"open diamond"<<"open cross"<<"full star"
              <<"open star"<<"star 2"<<"open triangle down"<<"full diamond"<<"full cross";

    lineNames <<"solid"<<"dashes"<<"dots"<<"dash dot"<<"long dash dot"<<"dash 3 dots"<<"long dash"
             <<"dash 2 dots"<< "long dash 2"<< "long dash 2 dot";

    fillNames<<"No fill"<<"3001"<<"3002"<<"3003"<<"3004"<<"3005"
            <<"3006"<<"3007"<<"3008"<<"3009"<<"3010"
           <<"3011"<<"3012"<<"3013"<<"3014"<<"3015"
          <<"3016"<<"3017"<<"3018"<<"3019"<<"3020"
         <<"3021"<<"3022"<<"3023"<<"3024"<<"3025";

    canvasMenu          = new QMenu();
    markerStylesMenu    = new QMenu();
    markerSizeMenu      = new QMenu();
    lineSizeMenu        = new QMenu();
    lineStylesMenu      = new QMenu();
    fillStylesMenu      = new QMenu();

    objectNameAction   = new QAction("NULL",               canvasMenu);
    //fitAction          = new QAction("Fit Data",           canvasMenu);
    lineColorAction    = new QAction("Set line color",     canvasMenu);
    fillColorAction    = new QAction("Set fill color",     canvasMenu);
    fillStyleAction    = new QAction("Set fill style",    canvasMenu);
    markerColorAction  = new QAction("Set marker color",   canvasMenu);
    markerStyleAction  = new QAction("Set marker style",   canvasMenu);
    lineStyleAction    = new QAction("Set line style",     canvasMenu);
    markerSizeAction   = new QAction("Set marker size",    canvasMenu);
    lineSizeAction     = new QAction("Set line size",      canvasMenu);
    //fontStyleAction    = new QAction("Set font style",     canvasMenu);

    copytoClipBoardAction = new QAction("Copy canvas",             canvasMenu);



    canvasMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0)");
    markerStylesMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0)");
    lineStylesMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0)");
    markerSizeMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0)");
    lineSizeMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0)");
    fillStylesMenu->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0);QMenu {  icon-size: 40px;  }");
    canvasMenu->addAction(objectNameAction);
    canvasMenu->addSeparator();



    int jj=0;
    for(int ii=1;ii<35;ii++){
        QIcon icon(QString(":icons/icons/root/markers/%1.png").arg(ii));
        QAction *markerAction = new QAction(icon,markerNames.at(jj++),markerStylesMenu);
        markerStylesMenu->addAction(markerAction);
        connect(markerAction,SIGNAL(triggered()),this,SLOT(setMarker()));
        if(ii==8)ii=19;
    }
    for(int ii=1;ii<11;ii++){
        QIcon icon(QString(":icons/icons/root/lines/%1.png").arg(ii));
        QAction *lineAction = new QAction(icon,lineNames.at(ii-1),markerStylesMenu);
        lineStylesMenu->addAction(lineAction);
        connect(lineAction,SIGNAL(triggered()),this,SLOT(setLineStyle()));
    }
    for(int ii=0;ii<11;ii++){
        //   QIcon icon(QString(":icons/icons/root/lines/%1.png").arg(ii));
        QAction *markerSizesAction = new QAction(QString("%1").arg(ii),markerSizeMenu);
        markerSizeMenu->addAction(markerSizesAction);
        connect(markerSizesAction,SIGNAL(triggered()),this,SLOT(setMarkerSize()));
    }
    for(int ii=0;ii<11;ii++){
        //   QIcon icon(QString(":icons/icons/root/lines/%1.png").arg(ii));
        QAction *lineSizesAction = new QAction(QString("%1").arg(ii),lineSizeMenu);
        lineSizeMenu->addAction(lineSizesAction);
        connect(lineSizesAction,SIGNAL(triggered()),this,SLOT(setLineSize()));
    }
    for(int ii=-1;ii<26;ii++){
        if(ii==-1){
            QIcon icon(QString(":icons/icons/root/fill/00.png"));
            QAction *fillStyleAction = new QAction(icon,QString("%1").arg("Solid"),fillStylesMenu);
            fillStylesMenu->addAction(fillStyleAction);
            connect(fillStyleAction,SIGNAL(triggered()),this,SLOT(setFillStyle()));

        }
        else  if(ii==0){
            QIcon icon(QString(":icons/icons/root/fill/0.png"));
            QAction *fillStyleAction = new QAction(icon,QString("%1").arg("No fill"),fillStylesMenu);
            fillStylesMenu->addAction(fillStyleAction);
            connect(fillStyleAction,SIGNAL(triggered()),this,SLOT(setFillStyle()));
        }
        else{
            QIcon icon(QString(":icons/icons/root/fill/%1.png").arg(3001+ii));
            QAction *fillStyleAction = new QAction(icon,QString("%1").arg(3001+ii),fillStylesMenu);
            fillStylesMenu->addAction(fillStyleAction);
            connect(fillStyleAction,SIGNAL(triggered()),this,SLOT(setFillStyle()));
        }


    }

    markerStyleAction->setMenu(markerStylesMenu);
    lineStyleAction->setMenu(lineStylesMenu);
    markerSizeAction->setMenu(markerSizeMenu);
    lineSizeAction->setMenu(lineSizeMenu);
    fillStyleAction->setMenu(fillStylesMenu);

    //connect(fitAction,      SIGNAL(triggered()),this,SLOT(showFitModule()));
    connect(lineColorAction,SIGNAL(triggered()),this,SLOT(showLineColorDialog()));
    connect(fillColorAction,SIGNAL(triggered()),this,SLOT(showFillColorDialog()));
    connect(markerColorAction,  SIGNAL(triggered()),this,SLOT(showMarkerColorDialog()));
    connect(colorDialog,        SIGNAL(colorSelected(QColor)),this,SLOT(setColor(QColor)));
    connect(markerStyleAction,  SIGNAL(triggered()),markerStylesMenu,SLOT(show()));
    connect(copytoClipBoardAction,SIGNAL(triggered()),this,SLOT(copyPad()));


    canvasN++;


    timerEventN =0 ;
    // if(!fRootTimer){
    fRootTimer  = new QTimer(this);
    fRootTimer->start(150);
    //}
    QObject::connect( fRootTimer, SIGNAL(timeout()), this, SLOT(HandleRootEvents()));

    disableMenus = false;

}

void iCanvas::SavePlot(QString filename, QString file_extension)
{
    bool saveAll=false;
    if(gPad==NULL){ // check for active canvas
        return;
    }
    gPad->GetCanvas()->Update();
    gPad->GetCanvas()->SetName("iRoot");
    if(saveAll)
        gPad->GetCanvas()->Print(filename.toLatin1().data(),file_extension.toLatin1().data());
    else
        gPad->GetCanvas()->Print(filename.append(".").append(file_extension).toLatin1().data(),file_extension.toLatin1().data());
    //Output(QString("Plot saved in file %1").arg(filename),"SUCCESS");
}

void iCanvas::copyPad()
{
    if(gPad!=NULL){
        SavePlot("cb","png");
        QImage clipboardImage("cb.png");
        QApplication::clipboard()->setImage(clipboardImage);
    }

}
void iCanvas::setLogX(bool val)
{
    if(gPad){
        logX=val;
        if(val==false)
            gPad->SetLogx(0);
        else
            gPad->SetLogx(1);
    }
}
void iCanvas::setLogY(bool val)
{
    if(gPad){
        logY=val;
        if(val==false)
            gPad->SetLogy(0);
        else
            gPad->SetLogy(1);
    }
}
void iCanvas::setGridX(bool val)
{

    if(gPad){
        gridX=val;
        if(val==false)
            gPad->SetGridx(0);
        else
            gPad->SetGridx(1);
    }
}

void iCanvas::setGridY(bool val)
{
    if(gPad){
        gridY=val;
        if(val==false)
            gPad->SetGridy(0);
        else
            gPad->SetGridy(1);
    }
}
//void iCanvas::showFitModule()
//{
//    fitModule->raise();
//    fitModule->show();
//}
void iCanvas::showLineColorDialog()
{
    colorMethod="SetLineColor";
    colorDialog->exec();
}

void iCanvas::showFillColorDialog()
{
    colorMethod="SetFillColor";
    colorDialog->exec();
}

void iCanvas::showMarkerColorDialog()
{
    colorMethod="SetMarkerColor";
    colorDialog->exec();
}


void iCanvas::setColor(QColor color)
{
    int tcolor = TColor::GetColor(color.red(),color.green(),color.blue());
    if(lastObject!=NULL){

        TClass c(fCanvas->GetSelected()->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject(colorMethod.toLatin1().data());
        TObjString *color= new TObjString(QString("%1").arg(tcolor).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(color);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);

            gPad->Update();
        }

    }

}

int iCanvas::GetLineColor(TObject *object)
{

    int lc=0;
    TAttLine *line = dynamic_cast <TAttLine *>(object);
    if(line!=NULL){
        lc= line->GetLineColor();

    }
    return lc;
}

int iCanvas::GetMarkerColor(TObject *object)
{

    int mc=0;
    TAttMarker *mark = dynamic_cast <TAttMarker *>(object);
    if(mark!=NULL){
        mc= mark->GetMarkerColor();

    }
    return mc;
}
void iCanvas::setLineStyle()
{
    QAction* action = (QAction*) QObject::sender();
    QString lineStyle= action->iconText();
    int lineIndex = lineNames.indexOf(lineStyle)+1;

    if(lastObject!=NULL){
        TClass c(lastObject->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject("SetLineStyle");
        TObjString *line= new TObjString(QString("%1").arg(lineIndex).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(line);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);
            gPad->Update();
        }

    }

}


void iCanvas::setMarker()
{
    QAction* action = (QAction*) QObject::sender();
    QString markerName= action->iconText();
    int markerIndex = markerNames.indexOf(markerName)+1;
    if(markerIndex>8)markerIndex = markerIndex+11;


    if(lastObject!=NULL){
        TClass c(lastObject->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject("SetMarkerStyle");
        TObjString *marker= new TObjString(QString("%1").arg(markerIndex).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(marker);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);
            gPad->Update();
        }

    }

}
void iCanvas::setFillStyle()
{
    QAction* action = (QAction*) QObject::sender();
    QString fillStyle= action->iconText();
    if(fillStyle=="No fill")
        fillStyle = "0";
    else if(fillStyle == "Solid")
        fillStyle = "1001";

    // qDebug()<<fillStyle;
    if(lastObject!=NULL){
        TClass c(lastObject->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject("SetFillStyle");
        TObjString *marker= new TObjString(QString("%1").arg(fillStyle).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(marker);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);
            gPad->Update();
        }

    }



}

void iCanvas::setMarkerSize()
{
    QAction* action = (QAction*) QObject::sender();
    QString markerSize= action->iconText();
    double markerIndex = markerSize.toDouble();


    if(lastObject!=NULL){
        TClass c(lastObject->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject("SetMarkerSize");
        TObjString *msize= new TObjString(QString("%1").arg(markerIndex).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(msize);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);
            gPad->Update();
        }


    }
}

void iCanvas::setLineSize()
{
    QAction* action = (QAction*) QObject::sender();
    QString lineSize= action->iconText();
    double lineIndex = lineSize.toDouble()*1.4;

    if(lastObject!=NULL){
        TClass c(lastObject->ClassName());
        const TList *l = c.GetListOfAllPublicMethods();
        TMethod *method =  (TMethod*)l->FindObject("SetLineWidth");
        TObjString *line= new TObjString(QString("%1").arg(lineIndex).toLatin1().data());
        TObjArray *objArray = new TObjArray();
        objArray->Add(line);
        objArray->Expand(1); // needed to shrink the size

        if(method!=NULL){
            lastObject->Execute(method,objArray);
            gPad->Update();
        }

    }
}

//void iCanvas::setFont(int font)
//{

//    if(lastObject!=NULL){
//        TClass c(lastObject->ClassName());
//        TList *l = c.GetListOfAllPublicMethods();
//        for(int ii=0;ii<l->GetEntries();ii++)
//            qDebug()<< l->At(ii)->GetName();
//        TMethod *method;
//        method =  (TMethod*)l->FindObject("SetTextFont");
//        TObjString *fontnumber= new TObjString(QString("%1").arg((font*10)+2).toLatin1().data());
//        TObjArray *objArray = new TObjArray();
//        objArray->Add(fontnumber);
//        objArray->Expand(1); // needed to shrink the size

//        if(method!=NULL){
//            lastObject->Execute(method,objArray);
//            gPad->Update();
//        }

//    }



//}

void iCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if(!disableMenus){
        if (fCanvas) {
            if (e->buttons() & Qt::LeftButton) {
                fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
            } else if (e->buttons() & Qt::MidButton) {
                fCanvas->HandleInput(kButton2Motion, e->x(), e->y());
            } else if (e->buttons() & Qt::RightButton) {
                fCanvas->HandleInput(kButton3Motion, e->x(), e->y());
            } else {
                fCanvas->HandleInput(kMouseMotion, e->x(), e->y());
            }
            TString objectName = fCanvas->GetSelected()->GetName();
            if(objectName=="xaxis" || objectName =="yaxis" || objectName =="zaxis"){
                setToolTip("Right click here to change axis title and properties");
            }
            else{
                setToolTip(QString(objectName));
            }
        }
    }
}

void iCanvas::mousePressEvent( QMouseEvent *e )
{
    if(!disableMenus){
        if (fCanvas) {
            TString objectName = "";

            if(fCanvas->GetSelected()){
                lastObject  =fCanvas->GetSelected();
                objectName  =fCanvas->GetSelected()->GetName();
                CheckAvailableMethods(fCanvas->GetSelected());
            }
            else{
                return;
            }
            fCanvas->GetSelectedPad()->cd();
            //fitModule->SetROOTObjectName(QString(objectName));

            objectNameAction->setText(QString(objectName).prepend("::").prepend(lastObject->ClassName()));

            switch (e->button()) {
            case Qt::LeftButton :
                fCanvas->HandleInput(kButton1Down, e->x(), e->y());
                break;
            case Qt::MidButton :
                fCanvas->HandleInput(kButton2Down, e->x(), e->y());
                break;
            case Qt::RightButton :
                if(objectName=="xaxis" || objectName =="yaxis" || objectName =="zaxis"){
                    //axisEditor->show();
                    //axisEditor->raise();
                }
                else{

                   // fitModule->SetListofObjects(GetListofFittableObjects());
                    canvasMenu->exec(QCursor::pos());
                }
                //fCanvas->HandleInput(kButton3Down, e->x(), e->y());
                //#ifdef WIN32
                //fCanvas->HandleInput(kButton3Down, e->x(), e->y());
                //#endif
                break;
            default:
                break;
            }
        }
    }
}


void iCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(!disableMenus){
        if (fCanvas) {
            switch (e->button()) {
            case Qt::LeftButton :
                fCanvas->HandleInput(kButton3Down, e->x(), e->y());

                break;
            case Qt::MidButton :
                break;
            case Qt::RightButton :
                //fCanvas->HandleInput(kButton3Down, e->x(), e->y());
                //#ifdef WIN32
                //fCanvas->HandleInput(kButton3Down, e->x(), e->y());
                //#endif
                break;
            default:
                break;
            }
        }
    }
}



void iCanvas::mouseReleaseEvent( QMouseEvent *e )
{
    if(!disableMenus){
        if (fCanvas) {
            switch (e->button()) {
            case Qt::LeftButton :
                fCanvas->HandleInput(kButton1Up, e->x(), e->y());
                break;
            case Qt::MidButton :
                fCanvas->HandleInput(kButton2Up, e->x(), e->y());
                break;
            case Qt::RightButton :
                fCanvas->HandleInput(kButton3Up, e->x(), e->y());
                // does not work properly on Linux...
                //#ifdef WIN32
                //          fCanvas->HandleInput(kButton3Up, e->x(), e->y());
                //#endif
                break;
            default:
                break;
            }
        }
    }
}

void iCanvas::resizeEvent( QResizeEvent * )
{
    if (fCanvas) {
        fCanvas->Resize();
        fCanvas->Update();
    }
}

void iCanvas::paintEvent( QPaintEvent * )
{
    if (fCanvas){
        fCanvas->Resize();
        fCanvas->Update();
    }
}

void iCanvas::wheelEvent(QWheelEvent *e)
{
    if(!disableMenus){
        //e->accept();
        if (fCanvas) {
            TString objectName = "";

            if(fCanvas->GetSelected()){
                lastObject  =fCanvas->GetSelected();
                objectName  =fCanvas->GetSelected()->GetName();
            }
            else{
                return;
            }
            fCanvas->GetSelectedPad()->cd();

            if(objectName=="xaxis" || objectName =="yaxis" || objectName =="zaxis"){
                TAxis * axis = ((TAxis*)(lastObject));
                gPad->SetView();

                //unzoom object owning this axis
                axis->SetRange(0,0);
                TH1 *hobj1 = (TH1*)axis->GetParent();
                if (!strstr(axis->GetName(),"xaxis")) {
                    if (!hobj1) return;
                    if (hobj1->GetDimension() == 2) {
                        if (strstr(axis->GetName(),"zaxis")) {
                            hobj1->SetMinimum();
                            hobj1->SetMaximum();
                            hobj1->ResetBit(TH1::kIsZoomed);
                            return;
                        }
                    }
                    if (strcmp(hobj1->GetName(),"hframe") == 0 ) {
                        hobj1->SetMinimum(axis->GetXmin());
                        hobj1->SetMaximum(axis->GetXmax());
                    } else {
                        hobj1->SetMinimum();
                        hobj1->SetMaximum();
                        hobj1->ResetBit(TH1::kIsZoomed);
                    }
                }
                //must unzoom all histograms in the pad
                TIter next(gPad->GetListOfPrimitives());
                TObject *obj;
                while ((obj= next())) {
                    if (!obj->InheritsFrom(TH1::Class())) continue;
                    TH1 *hobj = (TH1*)obj;
                    if (hobj == hobj1) continue;
                    if (!strstr(axis->GetName(),"xaxis")) {
                        if (hobj->GetDimension() == 2) {
                            if (strstr(axis->GetName(),"zaxis")) {
                                hobj->SetMinimum();
                                hobj->SetMaximum();
                                hobj->ResetBit(TH1::kIsZoomed);
                            } else {
                                hobj->GetYaxis()->SetRange(0,0);
                            }
                            return;
                        }
                        if (strcmp(hobj->GetName(),"hframe") == 0 ) {
                            hobj->SetMinimum(axis->GetXmin());
                            hobj->SetMaximum(axis->GetXmax());
                        } else {
                            hobj->SetMinimum();
                            hobj->SetMaximum();
                            hobj->ResetBit(TH1::kIsZoomed);
                        }
                    } else {
                        hobj->GetXaxis()->SetRange(0,0);
                    }
                }
            }
            gPad->Update();
            gPad->RedrawAxis();

        }
    }

}

void iCanvas::HandleRootEvents()
{
    //call the inner loop of ROOT

    if(highlight){ // for highlighting
        static int col =-1;
        static int markerCol=3;

        if(col==-1){
            col = GetLineColor(highlightObject);
            markerCol=GetMarkerColor(highlightObject);
            int highlightColor=0;
            if(col==0)highlightColor=1;
            TClass c(highlightObject->ClassName());
            const TList *l = c.GetListOfAllPublicMethods();
            TMethod *method1 =  (TMethod*)l->FindObject("SetLineColor");
            if(method1 !=NULL){
                TObjString *color         = new TObjString(QString("%1").arg(highlightColor).toLatin1().data());
                TObjArray *objArray       = new TObjArray();
                objArray->Add(color);
                objArray->Expand(1); // needed to shrink the size
                highlightObject->Execute(method1,objArray);
                delete color;
                delete objArray;
            }
            TMethod *method2 =  (TMethod*)l->FindObject("SetMarkerColor");
            if(method2 !=NULL){
                TObjString *color         = new TObjString(QString("%1").arg(highlightColor).toLatin1().data());
                TObjArray *objArray       = new TObjArray();
                objArray->Add(color);
                objArray->Expand(1); // needed to shrink the size
                highlightObject->Execute(method2,objArray);
                delete color;
                delete objArray;
            }

        }
        else{
            TClass c(highlightObject->ClassName());
            const TList *l = c.GetListOfAllPublicMethods();
            TMethod *method1 =  (TMethod*)l->FindObject("SetLineColor");
            if(method1 !=NULL){
                TObjString *color         = new TObjString(QString("%1").arg(col).toLatin1().data());
                TObjArray *objArray       = new TObjArray();
                objArray->Add(color);
                objArray->Expand(1); // needed to shrink the size
                highlightObject->Execute(method1,objArray);
            }
            TMethod *method2 =  (TMethod*)l->FindObject("SetMarkerColor");
            if(method2 !=NULL){
                TObjString *color         = new TObjString(QString("%1").arg(markerCol).toLatin1().data());
                TObjArray *objArray       = new TObjArray();
                objArray->Add(color);
                objArray->Expand(1); // needed to shrink the size
                highlightObject->Execute(method2,objArray);
                delete color;
                delete objArray;
            }
            col=-1;
            highlight=false;


        }
    }
    gSystem->ProcessEvents();
    if(fCanvas!=NULL && closed==false)
        fCanvas->Update();


}

void iCanvas::cd(int n)
{
    if(fCanvas!=NULL){
        fCanvas->cd(n);
        //fitModule->SetListofObjects(GetListofFittableObjects());
    }
}
/*
 * Will disable/enable menu items accordingly
 *
 */
void iCanvas::CheckAvailableMethods(TObject* obj)
{
    if(!disableMenus){
        if(obj!=NULL){
            TClass c(obj->ClassName());
            const TList *listofMethods = c.GetListOfAllPublicMethods();
            //Fitting
            if(listofMethods->FindObject("Fit")){
                //canvasMenu->removeAction(fitAction);
                //canvasMenu->addAction(fitAction);
                canvasMenu->addSeparator();
            }
            else{
                //canvasMenu->removeAction(fitAction);
            }

            //lines
            if(listofMethods->FindObject("SetLineColor")){
                canvasMenu->removeAction(lineColorAction);
                canvasMenu->removeAction(lineStyleAction);
                canvasMenu->removeAction(lineSizeAction);

                canvasMenu->addAction(lineColorAction);
                canvasMenu->addAction(lineStyleAction);
                canvasMenu->addAction(lineSizeAction);
                canvasMenu->addSeparator();

            }
            else{
                canvasMenu->removeAction(lineColorAction);
                canvasMenu->removeAction(lineStyleAction);
                canvasMenu->removeAction(lineSizeAction);
            }

            // marker
            if(listofMethods->FindObject("SetMarkerColor")){
                canvasMenu->removeAction(markerColorAction);
                canvasMenu->removeAction(markerStyleAction);
                canvasMenu->removeAction(markerSizeAction);

                canvasMenu->addAction(markerColorAction);
                canvasMenu->addAction(markerStyleAction);
                canvasMenu->addAction(markerSizeAction);
                canvasMenu->addSeparator();

            }
            else{
                canvasMenu->removeAction(markerColorAction);
                canvasMenu->removeAction(markerStyleAction);
                canvasMenu->removeAction(markerSizeAction);
            }
            // Fill color
            if(listofMethods->FindObject("SetFillColor")){
                canvasMenu->removeAction(fillColorAction);
                canvasMenu->addAction(fillColorAction);

            }
            else{
                canvasMenu->removeAction(fillColorAction);
            }
            if(listofMethods->FindObject("SetFillStyle")){
                canvasMenu->removeAction(fillStyleAction);
                canvasMenu->addAction(fillStyleAction);
                canvasMenu->addSeparator();

            }
            else{
                canvasMenu->removeAction(fillStyleAction);
            }


            //font
            if(listofMethods->FindObject("SetTextFont")){
//                canvasMenu->removeAction(fontStyleAction);
//                canvasMenu->addAction(fontStyleAction);
//                canvasMenu->addSeparator();
            }
            else{
                //canvasMenu->removeAction(fontStyleAction);
            }
                canvasMenu->addAction(copytoClipBoardAction);

        }

    }

}

void iCanvas::Refresh()
{
    if(fCanvas){
        fCanvas->Modified();
        fCanvas->Resize();
        fCanvas->Update();
    }
}

void iCanvas::closeEvent(QCloseEvent *)
{
    fRootTimer->stop();
    closed=true;


}

QStringList iCanvas::GetListofFittableObjects()
{
    QStringList list;
    if(gPad!=NULL){
        for(int ii=0;ii<gPad->GetListOfPrimitives()->GetEntries();ii++){
            TClass c(gPad->GetListOfPrimitives()->At(ii)->ClassName());
            const TList *l = c.GetListOfAllPublicMethods();
            TMethod *method =  (TMethod*)l->FindObject("Fit");
            if(method!=NULL){
                list.append(QString("[%1] %2").arg(ii).arg(gPad->GetListOfPrimitives()->At(ii)->GetName()));
            }
        }
    }
    return list;

}

void iCanvas::HighlightObject(QString objectName)
{
    if(gPad!=NULL){
        TList *l = gPad->GetListOfPrimitives();
        TObject *o = l->FindObject(objectName.toLatin1().data());
        if(o!=NULL){
            highlightObject=o;
            highlight    =true;
            //highlightTimer->start();
        }
    }
}

void iCanvas::Highlight(QString id)
{
    if(gPad!=NULL){
        int padN = id.split(".").at(0).toInt();
        int primitiveN = id.split(".").at(1).toInt();
        gPad->GetCanvas()->cd(padN);
        TList *l = gPad->GetCanvas()->GetPad(padN)->GetListOfPrimitives();
        TObject *o = l->At(primitiveN);
        if(o!=NULL){
            highlightObject=o;
            highlight    =true;
            //highlightTimer->start();
        }
    }
}


