#include "iROOTWorksheet.h"
int IROOTWorksheet::wsN = 1;


IROOTWorksheet::IROOTWorksheet(int type)
{
    dataType = type;
    tree        =0;
    nColumns    =0;
    nRows       =0;
    makeMenus();

}

IROOTWorksheet::IROOTWorksheet(QList<QStringList> Data, int rows, int columns, QStringList Header)
{
    dataType = 0;
    rowData     = Data;
    nRows       = rows;
    nColumns    = columns;
    header      = Header;

    columnN=1;
    makeMenus();

}

void IROOTWorksheet::clearData()
{
    if(dataType==0 || dataType==2)
        undo_stack->push(new Clear(this));
    emit layoutChanged();
}

void IROOTWorksheet::convertData()
{
    //    static int nTable=1;
    //    if(dataType==0){
    //        convertor->initialize(this,getTableHeader());
    //        convertor->setOutputParams(tree,QString("IROOTTABLE%1").arg(nTable));
    //        //convertor->start(Qt::bl);
    //    }
}

tableHeader IROOTWorksheet::getTableHeader()
{
    tableHeader th;
    for(int ii=0;ii<nColumns;ii++){
        bool ok;
        th.columnName.append(headerData(ii,Qt::Horizontal,Qt::DisplayRole).toString().remove("\n").remove(QRegExp(QString::fromUtf8("[-`~!@#$%^&*()_�+=|:;<>��,.?/{}\'\"\\\[\\\]\\\\]"))));
        data(this->index(0,ii),Qt::DisplayRole).toDouble(&ok);
        if(ok)
            th.type.append("Double");
        else
            th.type.append("String");
    }
    return th;
}

QString IROOTWorksheet::saveData(QString fileName, QString type)
{
    if(type=="txt"){
        QFile file(fileName);
        QTextStream out (&file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return "Error: could not open file for writing";
        out<<header.join("\t");
        for(int rr=0;rr<nRows;rr++){
            for(int cc=0;cc<nColumns;cc++){
                if(cc==0)
                    out<<data(this->index(rr,cc),Qt::DisplayRole).toString();
                else
                    out<<"\t"<<data(this->index(rr,cc),Qt::DisplayRole).toString();
            }
            if(rr!=nRows-1)
                out<<"\r"; // use \r always for carriage return \n and endl seem to insert an additional line in Windows
        }
        file.close();
        return QString("File saved in %1").arg(fileName);
    }
    else if(type=="ROOT"){
        if(currentFile!=""){
            QFile::copy(currentFile, fileName);
        }

    }
    return "Error: invalid format";
}

int IROOTWorksheet::rowCount(const QModelIndex &parent) const
{
    return nRows;
}



int IROOTWorksheet::columnCount(const QModelIndex &parent) const
{
    return nColumns;
}

QVariant IROOTWorksheet::data(const QModelIndex &index, int role) const
{
    if((index.row()<0) || (index.column()<0))
        return QVariant::Invalid;
    if(dataType==1 && tree==NULL){
        return QVariant::Invalid;
    }
    if(role == Qt::DisplayRole)
    {
        if(dataType==0 || dataType==2){
            if(index.row()<rowData.count()){
                QStringList lineData = rowData.at(index.row());
                if(index.column()<lineData.count()){
                    return lineData.at(index.column());
                }
                else
                    return QVariant::Invalid;
            }
        }
        else if(dataType==1){// for tree
            if(index.column()<branchList.count()){
                return IROOTTree::GetValue(tree,branchList.at(index.column()),index.row());
            }
            else
                return QVariant::Invalid;
        }
        else
            return QVariant::Invalid;
    }

    else
        return QVariant::Invalid;

}

bool IROOTWorksheet::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    undo_stack->push(new SetData(index,value,this));
    return true;

}




QVariant IROOTWorksheet::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if(section < header.count())
            return header.at(section);
    }
    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
    {
        return section;
    }
    return QVariant::Invalid;
}

Qt::ItemFlags IROOTWorksheet::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(dataType==0)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

void IROOTWorksheet::makeMenus()
{
    menu            =new QMenu();
    histogramMenu   =new QMenu();
    graphMenu       =new QMenu();

    newRowAbove    = new QAction("Insert Row Above",this);
    newRowBelow    = new QAction("Insert Row Below",this);
    newColLeft     = new QAction("Insert Column Left",this);
    newColRight    = new QAction("Insert Column Right",this);
    delRow          = new QAction("Remove Row",this);
    delCol          = new QAction("Remove Column",this);
    copyAction      = new QAction("Copy",this);
    pasteAction     = new QAction("Paste",this);
    plotHistogram   =new QAction("Plot histogram",this);
    plotGraph       =new QAction("Plot graph",this);
    plotBarHistogram=new QAction("Bar",this);
    plotPieChart    =new QAction("Pie chart",this);
    plotMarkerGraph =new QAction("Markers",this);
    plotLineGraph   =new QAction("Line",this);
    plotCurveGraph  =new QAction("Curve",this);
    fillRowNum      =new QAction("Fill with row numbers",this);


    //nBins           = IROOTPad::xbins;

    menu->setStyleSheet("background-color: white; font: 8pt \"Segoe UI\"");
    histogramMenu->setStyleSheet("background-color: white; font: 8pt \"Segoe UI\"");
    graphMenu->setStyleSheet("background-color: white; font: 8pt \"Segoe UI\"");

    if(dataType==0){
        menu->addAction(newRowAbove);
        menu->addAction(newRowBelow);
        menu->addAction(newColLeft);
        menu->addAction(newColRight);
        menu->addAction(delRow);
        menu->addAction(delCol);
        menu->addSeparator();
        menu->addAction(plotHistogram);
        menu->addAction(plotGraph);
        menu->addSeparator();
        menu->addAction(fillRowNum);
        menu->addSeparator();
        menu->addAction(copyAction);
        menu->addAction(pasteAction);

        histogramMenu->addAction(plotBarHistogram);
        histogramMenu->addAction(plotPieChart);
        plotHistogram->setMenu(histogramMenu);

        graphMenu->addAction(plotMarkerGraph);
        graphMenu->addAction(plotLineGraph);
        graphMenu->addAction(plotCurveGraph);
        plotGraph->setMenu(graphMenu);

        undo_stack = new QUndoStack();
        undo_stack->setUndoLimit(25);
        menu->addAction(undo_stack->createUndoAction(this, tr("&Undo")));

        connect(newRowAbove,SIGNAL(triggered()),this,SLOT(insertRowAbove()));
        connect(newRowBelow,SIGNAL(triggered()),this,SLOT(insertRowBelow()));
        connect(newColLeft,SIGNAL(triggered()),this,SLOT(insertColumnLeft()));
        connect(newColRight,SIGNAL(triggered()),this,SLOT(insertColumnRight()));
        connect(delRow,SIGNAL(triggered()),this,SLOT(deleteRow()));
        connect(delCol,SIGNAL(triggered()),this,SLOT(deleteColumn()));
        connect(fillRowNum,SIGNAL(triggered()),this,SLOT(fillRowNumbers()));

    }
    else{
        menu->addAction(plotHistogram);
        menu->addAction(plotGraph);
        menu->addSeparator();
        menu->addAction(copyAction);

        histogramMenu->addAction(plotBarHistogram);
        histogramMenu->addAction(plotPieChart);
        plotHistogram->setMenu(histogramMenu);

        graphMenu->addAction(plotMarkerGraph);
        graphMenu->addAction(plotLineGraph);
        graphMenu->addAction(plotCurveGraph);
        plotGraph->setMenu(graphMenu);

        undo_stack = new QUndoStack();
        undo_stack->setUndoLimit(25);
        menu->addAction(undo_stack->createUndoAction(this, tr("&Undo")));

        //        connect(plotHistogram,SIGNAL(triggered()),this,SLOT(PlotHistogram()));
        //        connect(plotGraph,SIGNAL(triggered()),this,SLOT(PlotGraph()));
    }
    connect(plotBarHistogram,SIGNAL(triggered()),this,SLOT(PlotBarHistogram()));
    connect(plotPieChart,SIGNAL(triggered()),this,SLOT(PlotPieChart()));
    connect(plotMarkerGraph,SIGNAL(triggered()),this,SLOT(PlotMarkerGraph()));
    connect(plotLineGraph,SIGNAL(triggered()),this,SLOT(PlotLineGraph()));
    connect(plotCurveGraph,SIGNAL(triggered()),this,SLOT(PlotCurveGraph()));
    connect(copyAction,SIGNAL(triggered()),this,SLOT(Copy()));
    connect(pasteAction,SIGNAL(triggered()),this,SLOT(Paste()));

}



void IROOTWorksheet::insertRowAbove()
{
    convertData();


    //    int atRow= table->currentIndex().row();
    //    if(atRow<0) atRow=0;
    //    undo_stack->push(new AppendRow(atRow,this));
    //    emit layoutChanged();

}

void IROOTWorksheet::insertRowBelow()
{
    int atRow= table->currentIndex().row();
    if(atRow<0) atRow=0;
    undo_stack->push(new AppendRow(atRow,this));
    emit layoutChanged();

}

void IROOTWorksheet::deleteRow()
{
    int atRow= table->currentIndex().row();
    undo_stack->push(new RemoveRow(atRow,this));
    emit layoutChanged();
}

void IROOTWorksheet::appendRow()
{
    undo_stack->push(new AppendRow(nRows,this));
    emit layoutChanged();
}


void IROOTWorksheet::insertColumnLeft()
{
    int atColumn =table->currentIndex().column();
    if(atColumn<0) atColumn=0;
    undo_stack->push(new AppendColumn(atColumn,this));
    emit layoutChanged();

}

void IROOTWorksheet::insertColumnRight()
{
    int atColumn =table->currentIndex().column();
    undo_stack->push(new AppendColumn(atColumn+1,this));
    emit layoutChanged();
}

void IROOTWorksheet::deleteColumn()
{

    int atColumn =table->currentIndex().column();
    if(atColumn<0) atColumn=0;
    //    for(int ii=0;ii<nRows;ii++)
    //        rowData[ii].removeAt(atColumn);
    //    header.removeAt(atColumn);
    //    nColumns--;
    undo_stack->push(new RemoveColumn(atColumn,this));
    emit layoutChanged();
}

void IROOTWorksheet::appendColumn()
{
    //    for(int ii=0;ii<nRows;ii++)
    //        rowData[ii].append("");
    //    header.append(QString("New Column %1").arg(columnN++));
    //    nColumns++;
    undo_stack->push(new AppendColumn(nColumns,this));
    emit layoutChanged();
}

void IROOTWorksheet::fillRowNumbers()
{
    int atColumn =table->currentIndex().column();
    if(atColumn<0) return;
    undo_stack->push(new RowNumberFiller(this,atColumn));
    emit layoutChanged();

}

void IROOTWorksheet::SetCurrentFile(QString rootFile)
{
    TFile *f = new TFile(rootFile.toLatin1().data(),"READ");
    QList <TTree*> treeList = GetTree(f);
    if(treeList.count()>0){
        TTree* t= treeList.at(0);
        QStringList branches = IROOTTree::GetListofBranches(t);
        SetCurrentTree(t,branches);
        currentFile = rootFile;
    }
    else {
        qDebug()<<"No trees found in current file";
    }


}

void IROOTWorksheet::ApplyFilter(QString cut)
{
    if(tree!=nullptr){
        QString error;
        IROOTTree::FilterTree(tree,cut,error);
        refreshTree();
        qDebug()<<"Applied filter..."<<cut<<error;
    }
}

void IROOTWorksheet::PlotData(QString plotString, QString cutString, QString opt)
{
    if(tree!=nullptr){
        tree->Draw(plotString.toLatin1().data(), cutString.toLatin1().data(),opt.toLatin1().data());
    }
}

QList <TTree*> IROOTWorksheet::GetTree(TFile *f)
{
    QList <TTree*> treeList;
    if(f!=NULL){
        TList *list = f->GetListOfKeys();//StreamerInfo
        TKey *keys;
        if(list==NULL){
            //mainWindow->ShowMessage("Error: No keys found for finding tree in file...","ERR");
            return treeList;
        }
        TIter next(list);
        TString className="TTree";
        TTree *tree=0;
        while ((keys = (TKey *)next())){
            if(keys->GetClassName()==className){
                tree = (TTree*)f->Get(keys->GetName());
                if(tree!=NULL){
                    if(tree->GetEntry(tree->GetEntryNumber(0))==-1){
                        //mainWindow->ShowMessage("Error: No entries found in tree...","ERR");
                    }
                    else{
                        treeList.append(tree);
                    }
                }
            }
        }

    }
    else{
        //mainWindow->ShowMessage("Error: No tree found in file...","ERR");
        return treeList;
    }
    return treeList;
}

void IROOTWorksheet::SetCurrentTree(TTree *t, QStringList branches)
{
    tree        =t;
    branchList  =branches;
    header      =branches;
    nColumns    =branches.count();
    if(tree->GetEventList())
        nRows = tree->GetEventList()->GetN();
    else
        nRows = tree->GetEntries();
    emit layoutChanged();
}

void IROOTWorksheet::resetTree()
{
    tree =0;
    nRows=0;
    nColumns =0;
    header.clear();
    emit layoutChanged();
}

void IROOTWorksheet::refreshTree()
{
    if(tree->GetEventList())
        nRows = tree->GetEventList()->GetN();
    else
        nRows = tree->GetEntries();
    emit layoutChanged();

}

void IROOTWorksheet::showMenu(QPoint p)
{
    menu->exec(QCursor::pos());
    position = QCursor::pos();
}

void IROOTWorksheet::Copy()
{
    //QItemSelectionModel * selection = selectionModel();
    QModelIndexList indexes = table->selectionModel()->selectedIndexes();

    if(indexes.size() < 1)
        return;

    // QModelIndex::operator < sorts first by row, then by column.
    // this is what we need
    std::sort(indexes.begin(), indexes.end());



    // You need a pair of indexes to find the row changes
    QModelIndex previous = indexes.first();
    QString selected_text;

    if(indexes.size() == 1){
        QVariant copydata = data(previous, Qt::DisplayRole);
        selected_text.append(copydata.toString());

        QApplication::clipboard()->setText(selected_text);
        return;
    }

    indexes.removeFirst();




    QModelIndex current;
    Q_FOREACH(current, indexes)
    {
        QVariant copydata = data(previous,Qt::DisplayRole);
        QString text = copydata.toString();
        // At this point `text` contains the text in one cell
        selected_text.append(text);
        // If you are at the start of the row the row number of the previous index
        // isn't the same.  Text is followed by a row separator, which is a newline.
        if (current.row() != previous.row())
        {
            selected_text.append(QLatin1Char('\r')); // use \r instead of \n or endl
        }
        // Otherwise it's the same row, so append a column separator, which is a tab.
        else
        {
            selected_text.append(QLatin1Char('\t'));
        }
        previous = current;
    }

    // add last element

    selected_text.append(data(current,Qt::DisplayRole).toString());

    QApplication::clipboard()->setText(selected_text);
    //qDebug()<<selected_text;
}

void IROOTWorksheet::Paste()
{

    undo_stack->push(new PasteData(this));
    emit layoutChanged();
}


void IROOTWorksheet::PlotBarHistogram()
{
    PlotHistogram();
}
void IROOTWorksheet::PlotPieChart()
{
    PlotHistogram("PIE");
}
void IROOTWorksheet::PlotHistogram(QString type)
{
    //    QModelIndexList indexes = table->selectionModel()->selectedIndexes();
    //    if(indexes.size() < 1)
    //        return;
    //    QList <int> cols    = GetSelectedColumns(indexes);
    //    QList <double> range= GetRange(indexes);
    //    int nCols           = cols.count();
    //    if(nCols>1 && type=="PIE"){
    //        Message("Select a single column for pie charts","ERR");
    //        return;
    //    }
    //    //if(gPad==NULL)
    //    CreatePad();
    //    TH1F **histograms = new TH1F*[nCols];

    //    int ii;
    //    QString name,title;

    //    for(ii=0;ii<nCols;ii++){
    //        name    = QString("iRootWorksheet%1_%2").arg(wsN++).arg(header.at(cols.at(ii)));
    //        title   = header.at(cols.at(ii));

    //        histograms[ii] = new TH1F(name.toLatin1().data(),
    //                                  title.toLatin1().data(),
    //                                  nBins,range.at(0),range.at(1));
    //    }
    //    int col;
    //    for(ii=0;ii<indexes.count();ii++){
    //        col = cols.lastIndexOf(indexes.at(ii).column());
    //        histograms[col]->Fill(data(indexes.at(ii),Qt::DisplayRole).toFloat());
    //    }

    //    if(type=="PIE"){
    //        histograms[0]->Draw("PIE");
    //        return;
    //    }

    //    for(ii=0;ii<nCols;ii++){
    ////        TList *l = gPad->GetListOfPrimitives();
    ////        for(int i=0; i<l->GetEntries();i++)
    ////            qDebug()<<l->At(i)->GetName();
    ////        TObject *obj = gPad->FindObject("TFrame");
    //        if(ii==0){
    //            histograms[ii]->Draw();
    //        }
    //        else{
    //            histograms[ii]->Draw("SAME");
    //        }
    //    }
    //    if(nCols>0)
    //        IROOTPad::SetUniqueLineColors();
}

void IROOTWorksheet::PlotMarkerGraph()
{
    PlotGraph("MARKER");
}

void IROOTWorksheet::PlotLineGraph()
{
    PlotGraph("LINE");
}
void IROOTWorksheet::PlotCurveGraph()
{
    PlotGraph("CURVE");
}
void IROOTWorksheet::PlotGraph(QString type)
{
    //    QModelIndexList indexes = table->selectionModel()->selectedIndexes();
    //    if(indexes.size() < 1)
    //        return;
    //    QList <int> cols                = GetSelectedColumns(indexes);
    //    QList <double> range            = GetRange(indexes);
    //    QList <int> firstColumnRowIndexes  = GetXColumnRowIndexes(indexes);
    //    int nCols           = cols.count();
    //    //TGraph **graphs = new TGraph*[nCols];

    //    int ii;
    //    QString name,title;
    //    if(nCols<2){
    //        Message("Select more than 1 column for graphs","ERR");
    //        return;
    //    }
    //    for(ii=0;ii<nCols-1;ii++){
    //        name    = QString("iRootWorksheet%1_%2").arg(wsN++).arg(header.at(cols.at(ii+1)));
    //        title   = header.at(cols.at(ii+1));

    //        graphs[ii] = new TGraph();
    //        graphs[ii]->SetName(name.toLatin1().data());
    //        graphs[ii]->SetTitle(title.toLatin1().data());
    //    }


    //    for(int cc=1;cc<nCols;cc++){
    //        int pointN =0;
    //        for(ii=0;ii<firstColumnRowIndexes.count();ii++){
    //            if(indexes.contains(createIndex(firstColumnRowIndexes.at(ii),cols.at(cc)))){
    //                graphs[cc-1]->SetPoint(pointN++,
    //                                       data(createIndex(firstColumnRowIndexes.at(ii),cols.at(0)),Qt::DisplayRole).toDouble(),
    //                                       data(createIndex(firstColumnRowIndexes.at(ii),cols.at(cc)),Qt::DisplayRole).toDouble());
    //            }

    //        }
    //    }
    //    //if(gPad==NULL)
    //    CreatePad();
    //    for(ii=0;ii<nCols-1;ii++){
    //        if(ii==0){
    //            if(type=="MARKER"){
    //                graphs[ii]->Draw("*A");
    //            }
    //            else if(type=="LINE"){
    //                graphs[ii]->Draw("pLA");
    //            }
    //            else if(type=="CURVE"){
    //                graphs[ii]->Draw("PCA");
    //            }

    //        }
    //        else{
    //            if(type=="MARKER"){
    //                graphs[ii]->Draw("*SAME");
    //            }
    //            else if(type=="LINE"){
    //                graphs[ii]->Draw("pLSAME");
    //            }
    //            else if(type=="CURVE"){
    //                graphs[ii]->Draw("PCSAME");
    //            }
    //        }
    //    }
    //    if(nCols>0){
    //        if(type=="MARKER"){
    //            IROOTPad::SetUniqueMarkerStyles();
    //        }
    //        else {
    //            IROOTPad::SetUniqueLineColors();
    //        }
    //    }
    //}

    //QList <int> IROOTWorksheet::GetSelectedColumns(QModelIndexList list)
    //{
    //    std::sort(list.begin(), list.end());
    //    QList <int> cols;
    //    for(int ii=0;ii<list.count();ii++){
    //        cols.append(list.at(ii).column());
    //    }
    //    QList <int> nCols = (QList <int>::fromSet(QSet <int>::fromList(cols)));
    //    std::sort(nCols.begin(), nCols.end());
    //    return nCols;
}

//QList <int> IROOTWorksheet::GetXColumnRowIndexes(QModelIndexList list)
//{

//    int firstCol = GetSelectedColumns(list).at(0);
//    QList <int> firstColIndexes;
//    for(int ii=0;ii<list.count();ii++){
//        if(list.at(ii).column()==firstCol){
//            firstColIndexes.append(list.at(ii).row());
//        }
//    }

//    return firstColIndexes;
//}


QList <double> IROOTWorksheet::GetRange(QModelIndexList list)
{
    std::sort(list.begin(), list.end());
    double min,max,value;

    for(int ii=0;ii<list.count();ii++){
        value =  data(list.at(ii),Qt::DisplayRole).toDouble();
        if(ii==0){
            min = value;
            max = min;
        }
        else{
            if(max<value)
                max = value;
            if(min>value)
                min = value;
        }
    }
    QList <double> range;
    range.append(min);
    range.append(max);
    return range;
}


