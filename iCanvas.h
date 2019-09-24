#ifndef ICANVAS_H
#define ICANVAS_H


#include <TCanvas.h>
#include <QWidget>
#include <TH1F.h>
#include <TVirtualX.h>
#include <TRootCanvas.h>
#include <TCanvas.h>
#include <TVirtualX.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TFormula.h>
#include <TFrame.h>
#include <TTimer.h>
#include <QDebug>
#include <QPushButton>
#include <QLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QColorDialog>
#include <QIcon>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <TClass.h>
#include <TList.h>
#include <TApplication.h>
#include <TMethodCall.h>
#include <TDataMember.h>
#include <TMethodCall.h>


class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QPushButton;
class QTimer;
class TCanvas;
class TRootCanvas;
class iCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit iCanvas(QWidget *parent = 0);
    static int                  canvasN;
    virtual ~iCanvas()
    {
        delete fCanvas;
        delete fRootTimer;
       // delete colorDialog;
    }
    TCanvas* GetCanvas() {return fCanvas;}
    TVirtualPad* GetPad(int n){return fCanvas->GetPad(n);}
    void cd(int n);
    void SavePlot(QString filename, QString extension);
    bool closed; // flag to indicate that the canvas is closed
    void Refresh();
    void CheckAvailableMethods(TObject* obj);
    void HighlightObject(QString objectName);
    int  GetLineColor(TObject *object);
    int  GetMarkerColor(TObject *object);
    QStringList GetListofFittableObjects();
protected:
    TCanvas                     *fCanvas;
    QTimer                      *fRootTimer;





    QMenu                       *canvasMenu;
    QMenu                       *markerStylesMenu;
    QMenu                       *lineStylesMenu;
    QMenu                       *lineSizeMenu;
    QMenu                       *markerSizeMenu;
    QMenu                       *fillStylesMenu;


    static QColorDialog         *colorDialog;
    static QStringList          markerNames;
    static QStringList          lineNames;
    static QStringList          fillNames;
    static bool                 logX;
    static bool                 logY;
    static bool                 gridX;
    static bool                 gridY;
    bool                        disableMenus;
    static bool                 highlight;




signals:

public slots:
    void HandleRootEvents();
    virtual void    mouseMoveEvent( QMouseEvent *e );
    virtual void    mousePressEvent( QMouseEvent *e );
    virtual void    mouseDoubleClickEvent(QMouseEvent *);
    virtual void    mouseReleaseEvent( QMouseEvent *e );
    virtual void    wheelEvent(QWheelEvent *e);
    virtual void    paintEvent( QPaintEvent *e );
    virtual void    resizeEvent( QResizeEvent *e );
   // void            showFitModule();
    void            showLineColorDialog();
    void            showFillColorDialog();
    void            showMarkerColorDialog();
    void            setColor(QColor color);
    void            setMarker();
    void            setMarkerSize();
    void            setLineSize();
    void            setLineStyle();
    void            setFillStyle();
    void            setLogX(bool val);
    void            setLogY(bool val);
    void            setGridX(bool val);
    void            setGridY(bool val);
    void            Highlight(QString objectId);
    virtual void    closeEvent(QCloseEvent *);
    void            copyPad();

private:
    TObject*       lastObject;// object where the menu was executed
    TObject*       highlightObject;
    QString        colorMethod;// string to tell which method is to be executed: SetLineColor, SetFillColor..
    QAction*       objectNameAction;
    QAction*       copytoClipBoardAction;
   // QAction*       fitAction;
    QAction*       lineColorAction;
    QAction*       fillColorAction;
    QAction*       fillStyleAction;
    QAction*       markerColorAction;
    QAction*       markerStyleAction;
    QAction*       lineStyleAction;
    QAction*       markerSizeAction;
    QAction*       lineSizeAction;
   // QAction*       fontStyleAction;
    QAction*       enableLogX;
    QAction*       enableLogY;
    QAction*       enableGridX;
    QAction*       enableGridY;
    int            timerEventN;



};

#endif // ICANVAS_H
