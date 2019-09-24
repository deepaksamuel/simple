#ifndef IROOTWORKSHEET_H
#define IROOTWORKSHEET_H

#include <QApplication>
#include <QAbstractTableModel>
#include <QAction>
#include <QClipboard>
#include <QMenu>
#include <QTableView>
#include <QUndoStack>
#include <QFile>
#include <QTextStream>
#include <QThread>

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH1S.h>
#include <TEventList.h>

#include <iROOTTree.h>
//#include <IROOTPad.h>

typedef struct {
    QStringList columnName;
    QStringList type;
} tableHeader;

class IROOTWorksheet : public QAbstractTableModel
{
    Q_OBJECT
public:
    IROOTWorksheet(int type=0);
    IROOTWorksheet(QList <QStringList> Data, int rows, int columns, QStringList Header);
    //IROOTWorksheet(QString fileName);


    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void SetTable(QTableView* view){table = view;}
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void makeMenus();
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    void resetTree();
    void refreshTree();
    void clearData();
    void convertData();
   // QList <int>     GetSelectedColumns(QModelIndexList list);
   // QList <int>     GetXColumnRowIndexes(QModelIndexList list);
    QList<double>   GetRange(QModelIndexList list);
    QString saveData(QString fileName, QString type);
    tableHeader getTableHeader();
    QStringList getHeader(){return header;}
    void SetCurrentTree(TTree* t, QStringList branches);
    void SetCurrentFile(QString rootFile);
    void ApplyFilter(QString cut);
    QList <TTree*> GetTree(TFile* f);
    void PlotData(QString plotString, QString cutString="", QString opt="");

private slots:
    void insertRowAbove();
    void insertRowBelow();
    void deleteRow();
    void appendRow();
    void insertColumnLeft();
    void insertColumnRight();
    void deleteColumn();
    void appendColumn();
    void fillRowNumbers();



    void showMenu(QPoint p);
    void Copy();
    void Paste();
    void PlotBarHistogram();
    void PlotPieChart();
    void PlotMarkerGraph();
    void PlotLineGraph();
    void PlotCurveGraph();
    void PlotHistogram(QString type="BAR");
    void PlotGraph(QString type="MARKER");

signals:
    void Message(QString message, QString type);
    void CreatePad();
private:
    friend class SetData;
    friend class AppendRow;
    friend class RemoveRow;
    friend class AppendColumn;
    friend class RemoveColumn;
    friend class PasteData;
    friend class Clear;
    friend class RowNumberFiller;
    QList <QStringList> rowData;
    int nColumns;
    int nRows;
    static int wsN;
    QStringList header;
    QStringList branchList;
    QMenu *menu;
    QMenu *histogramMenu;
    QMenu *graphMenu;
    QTableView* table; // the table connected to this model
    QPoint position;
    int columnN; // used for naming columns
    TTree *tree; // the tree, is zero if not from tree

    QAction *newRowAbove;
    QAction *newRowBelow;
    QAction *newColLeft;
    QAction *newColRight;
    QAction *delRow;
    QAction *delCol;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *plotHistogram;
    QAction *plotGraph;
    QAction *plotBarHistogram;
    QAction *plotPieChart;
    QAction *plotMarkerGraph;
    QAction *plotLineGraph;
    QAction *plotCurveGraph;
    QAction *fillRowNum;
    QString currentFile;

    QUndoStack *undo_stack;
    int         dataType; //0: Normal, editable, 1: Tree, 2: Fit data
    int     nBins;

};

class SetData : public QUndoCommand
{
public:
    SetData(const QModelIndex & index, const QVariant & value, IROOTWorksheet* wsModel) : QUndoCommand("set data"),m_model(wsModel),old_index(index)
    {
        oldData =  m_model->data(index,Qt::DisplayRole);
        if(index.isValid()){
            m_model->rowData[index.row()][index.column()]=value.toString();
        }
    }
    void redo()
    {

    }
    void undo()
    {
        if(old_index.isValid()){
            m_model->rowData[old_index.row()][old_index.column()]=oldData.toString();
        }
    }
private:
    QVariant oldData;
    IROOTWorksheet *m_model;
    QModelIndex     old_index;

};

class AppendRow : public QUndoCommand
{
public:
    AppendRow(int atRow, IROOTWorksheet* wsModel) : QUndoCommand("add row"),m_model(wsModel),old_row(atRow)
    {
        QStringList l;
        if(atRow<0) atRow=0;
        for(int ii=0;ii<wsModel->nColumns;ii++)
            l.append("");
        wsModel->rowData.insert(atRow,l);
        wsModel->nRows++;
    }
    void redo()
    {

    }
    void undo()
    {
        m_model->rowData.removeAt(old_row);
        m_model->nRows--;
        emit m_model->layoutChanged();
    }
private:
    int             old_row;
    IROOTWorksheet *m_model;

};


class RemoveRow : public QUndoCommand
{
public:
    RemoveRow(int atRow, IROOTWorksheet* wsModel) : QUndoCommand("remove row"),m_model(wsModel),old_row(atRow)
    {
        old_row_data = wsModel->rowData.at(atRow);
        wsModel->rowData.removeAt(atRow);
        wsModel->nRows--;
    }
    void redo()
    {

    }
    void undo()
    {
        m_model->rowData.insert(old_row,old_row_data);
        m_model->nRows++;
        emit m_model->layoutChanged();
    }
private:
    int             old_row;
    QStringList     old_row_data;
    IROOTWorksheet *m_model;
};

class AppendColumn : public QUndoCommand
{
public:
    AppendColumn(int atCol, IROOTWorksheet* wsModel) : QUndoCommand("add column"),m_model(wsModel),old_col(atCol)
    {
        for(int ii=0;ii<wsModel->nRows;ii++)
            wsModel->rowData[ii].insert(atCol,"");
        wsModel->header.insert(atCol,QString("New Column %1").arg(wsModel->columnN++));
        wsModel->nColumns++;
    }
    void redo()
    {

    }
    void undo()
    {
        for(int ii=0;ii<m_model->nRows;ii++){
            m_model->rowData[ii].removeAt(old_col);
        }
        m_model->header.removeAt(old_col);
        m_model->nColumns--;
        emit m_model->layoutChanged();
    }
private:
    int             old_col;
    IROOTWorksheet *m_model;

};

class RemoveColumn : public QUndoCommand
{
public:
    RemoveColumn(int atCol, IROOTWorksheet* wsModel) : QUndoCommand("remove column"),m_model(wsModel),old_col(atCol)
    {
        for(int ii=0;ii<wsModel->nRows;ii++){
            old_col_data.append(wsModel->rowData[ii].at(atCol));
            wsModel->rowData[ii].removeAt(atCol);
        }
        old_header = wsModel->header.at(atCol);
        wsModel->header.removeAt(atCol);
        wsModel->nColumns--;
    }
    void redo()
    {

    }
    void undo()
    {
        for(int ii=0;ii<m_model->nRows;ii++){
            m_model->rowData[ii].insert(old_col,old_col_data.at(ii));
        }
        m_model->header.insert(old_col,old_header);
        m_model->nColumns++;
        emit m_model->layoutChanged();
    }
private:
    int             old_col;
    QStringList     old_col_data;
    QString         old_header;
    IROOTWorksheet *m_model;

};

class PasteData : public QUndoCommand
{
public:
    PasteData(IROOTWorksheet* wsModel) : QUndoCommand("paste data"),m_model(wsModel)
    {
        old_data    = wsModel->rowData;
        old_nRows   = wsModel->nRows;
        old_nCols   = wsModel->nColumns;
        QString string = QApplication::clipboard()->text();
        QStringList l = string.split("\n");
        int currentRow  = wsModel->table->currentIndex().row();
        int currentCol  = wsModel->table->currentIndex().column();

        for(int rr=0;rr<l.count();rr++){

            QStringList row = l.at(rr).split("\t");
            for(int cc=0;cc<row.count();cc++){

                if((currentRow+rr)<wsModel->rowData.count()){ // enough rows
                    if(cc+currentCol<wsModel->rowData[currentRow+rr].count()){ // enough columns
                        wsModel->rowData[currentRow+rr][cc+currentCol]=row.at(cc);
                    }
                    else{
                        wsModel->appendColumn();
                        wsModel->rowData[currentRow+rr][cc+currentCol]=row.at(cc);
                    }
                }
                else{
                    wsModel->appendRow();
                    if(cc+currentCol<wsModel->rowData[currentRow+rr].count()){ // enough columns
                        wsModel->rowData[currentRow+rr][cc+currentCol]=row.at(cc);
                    }
                    else{
                        wsModel->appendColumn();
                        wsModel->rowData[currentRow+rr][cc+currentCol]=row.at(cc);
                    }
                }

            }
        }

    }
    void redo()
    {

    }
    void undo()
    {
        m_model->rowData = old_data;
        m_model->nRows = old_nRows;
        m_model->nColumns = old_nCols;
        emit m_model->layoutChanged();
    }
private:
    QList <QStringList> old_data;
    int old_nRows;
    int old_nCols;
    IROOTWorksheet *m_model;

};


class Clear : public QUndoCommand
{
public:
    Clear(IROOTWorksheet* wsModel) : QUndoCommand("clear data"),m_model(wsModel)
    {
        old_data    = wsModel->rowData;
        old_nRows   = wsModel->nRows;
        old_nCols   = wsModel->nColumns;

        wsModel->rowData.clear();
        wsModel->nRows=0;
        wsModel->nColumns=0;
    }
    void redo()
    {

    }
    void undo()
    {
        m_model->rowData = old_data;
        m_model->nRows = old_nRows;
        m_model->nColumns = old_nCols;
        emit m_model->layoutChanged();
    }
private:
    QList <QStringList> old_data;
    int old_nRows;
    int old_nCols;
    IROOTWorksheet *m_model;

};


class RowNumberFiller: public QUndoCommand
{
public:
    RowNumberFiller(IROOTWorksheet* wsModel, int col) : QUndoCommand("fill with row numbers"),m_model(wsModel), lastCol(col)
    {
        for(int ii=0;ii<wsModel->nRows;ii++){
            old_data.append(wsModel->rowData.at(ii).at(col));
            wsModel->rowData[ii][col]=QString("%1").arg(ii);
        }
    }
    void redo()
    {

    }
    void undo()
    {
        for(int ii=0;ii<m_model->nRows;ii++){
            m_model->rowData[ii][lastCol]=old_data.at(ii);
        }
        emit m_model->layoutChanged();
    }
private:
    QStringList old_data;
    int lastCol;
    IROOTWorksheet *m_model;

};

#endif // IROOTWORKSHEET_H



