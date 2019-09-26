#ifndef SIMPLEOPENGLSTOREDQTVIEWER_H
#define SIMPLEOPENGLSTOREDQTVIEWER_H
//---
// File modified by Deepak Samuel on 25 Sep 2019
#include "G4OpenGLStoredViewer.hh"
#include "SimpleOpenGLQtViewer.h"
#include <qgl.h>  // include qglwidget

class QMouseEvent;
class QWheelEvent;
class QContextMenuEvent;

class G4OpenGLStoredSceneHandler;

class SimpleOpenGLStoredQtViewer:
   public SimpleOpenGLQtViewer, public G4OpenGLStoredViewer, public QGLWidget {
   //public SimpleOpenGLQtViewer, public G4OpenGLStoredViewer {
public:
  SimpleOpenGLStoredQtViewer (G4OpenGLStoredSceneHandler& scene,
                const G4String& name = "");
  ~SimpleOpenGLStoredQtViewer ();
  void Initialise ();
  void initializeGL ();
  void DrawView ();
  void resizeGL(int width,int height);
  void paintGL();
  void updateQWidget();
  void ShowView ();
  void DisplayTimePOColourModification (G4Colour&,size_t);

protected:

  // Special version for Qt - avoid comparing VisAttributesModifiers.
  G4bool CompareForKernelVisit(G4ViewParameters&);

  // Two virtual functions to return sub-class selection.
  G4bool POSelected(size_t POListIndex);
  G4bool TOSelected(size_t TOListIndex);

  void showEvent(QShowEvent * event );
  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void contextMenuEvent(QContextMenuEvent *e);
  void keyPressEvent (QKeyEvent * event);
  void keyReleaseEvent (QKeyEvent * event);
  void paintEvent(QPaintEvent *event);
private:
  void ComputeView ();

  //  QImage glBufferImage;
};


#endif // SIMPLEOPENGLSTOREDQTVIEWER_H
