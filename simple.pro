#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T01:14:38
#
#-------------------------------------------------

QT       += core gui sql opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simple
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    iCanvas.cpp \
    iROOTWorksheet.cpp \
    iROOTTTree.cpp \
    physics_lists/DMXMaxTimeCuts.cc \
    physics_lists/DMXMinEkineCuts.cc \
    physics_lists/DMXSpecialCuts.cc \
    physics_lists/HadrontherapyStepMax.cc \
    physics_lists/PhysicsList_DMX.cc \
    physics_lists/PhysicsList_DNA.cc \
    physics_lists/PhysicsList_Hadrontherapy.cc \
    physics_lists/PhysicsList_Opt.cc \
    physics_lists/PhysicsList_PolEM.cc \
    physics_lists/PhysicsList_RadDecay.cc \
    physics_lists/PhysListEmPolarized.cc \
    physics_lists/StepMax.cc \
    Simple.cpp \
    SimpleAction.cpp \
    SimpleDatabaseManager.cpp \
    SimpleDetectorConstruction.cpp \
    SimpleGun.cpp \
    SimpleMesh.cpp \
    SimpleMeshTableModel.cpp \
    SimpleObject.cpp \
    SimpleObjectTableModel.cpp \
    SimpleOpenGLImmediateQtViewer.cpp \
    SimpleOpenGLQtViewer.cpp \
    SimpleOpenGLStoredQtScenehandler.cpp \
    SimplePhysicslist.cpp \
    SimplePhysicslistDialog.cpp \
    SimpleRunAction.cpp \
    SimpleSteppingAction.cpp \
    SimpleStoredVis.cpp \
    SimpleVis.cpp \
    SimpleMaterialBuilder.cpp \
    SimpleMaterialPropertyBuilder.cpp \
    SimpleOpenGLStoredQtViewer.cpp



HEADERS += \
    iCanvas.h \
    iROOTWorksheet.h \
    iROOTTree.h \
    physics_lists/DMXMaxTimeCuts.hh \
    physics_lists/DMXMinEkineCuts.hh \
    physics_lists/DMXSpecialCuts.hh \
    physics_lists/HadrontherapyStepMax.hh \
    physics_lists/PhysicsList_DMX.hh \
    physics_lists/PhysicsList_DNA.hh \
    physics_lists/PhysicsList_Hadrontherapy.hh \
    physics_lists/PhysicsList_Opt.hh \
    physics_lists/PhysicsList_PolEM.hh \
    physics_lists/PhysicsList_RadDecay.hh \
    physics_lists/PhysListEmPolarized.hh \
    physics_lists/StepMax.hh \
    Simple.h \
    SimpleAction.h \
    SimpleDatabaseManager.h \
    SimpleDetectorConstruction.h \
    SimpleGun.h \
    SimpleMesh.h \
    SimpleMeshTableModel.h \
    SimpleObject.h \
    SimpleObjectTableModel.h \
    SimpleOpenGLStoredQtScenehandler.h \
    SimpleOpenGLStoredQtViewer.h \
    SimplePhysicslist.h \
    SimplePhysicslistDialog.h \
    SimpleRunAction.h \
    SimpleSteppingAction.h \
    SimpleStoredVis.h \
    SimpleVis.h \
    SimpleMaterialBuilder.h \
    SimpleMaterialPropertyBuilder.h \
    SimpleOpenGLImmediateQtViewer.h \
    SimpleOpenGLQtViewer.h



FORMS += \
        simple.ui \
    physicslistdialog.ui \
    simpleobject.ui \
    SimpleMaterialBuilder.ui \
    SimpleMaterialPropertyBuilder.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#generated from Geant4-config
QMAKE_CXXFLAGS=-DG4INTY_USE_XT -DG4VIS_USE_OPENGL -DG4UI_USE_TCSH -DG4INTY_USE_QT -DG4UI_USE_QT -DG4VIS_USE_OPENGLQT -I/usr/include/x86_64-linux-gnu/qt5/ -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/lib/x86_64-linux-gnu/qt5//mkspecs/linux-g++ -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -DG4VIS_USE_OPENGLX -W -Wall -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-variadic-macros -Wshadow -pipe -pthread -ftls-model=initial-exec -std=c++11 -I/home/samuel/g4/g4-install/bin/../include/Geant4
LIBS+=-L/home/samuel/g4/g4-install/bin/../lib -lG4OpenGL -lG4gl2ps -lG4Tree -lG4FR -lG4GMocren -lG4visHepRep -lG4RayTracer -lG4VRML -lG4vis_management -lG4modeling -lG4interfaces -lG4persistency -lG4analysis -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -lG4clhep -lG4expat -lG4zlib

# the following line is required for Geant4 to work
LIBS+= -L/usr/lib/x86_64-linux-gnu/ -lxerces-c-3.2

#generated from root-config
LIBS+=-L/home/samuel/root_v6.18.04.source/root-install/lib -lGui -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -pthread -lm -ldl -rdynamic
QMAKE_CXXFLAGS+=-pthread -std=c++11 -m64 -I/home/samuel/root_v6.18.04.source/root-install/include

DISTFILES += \
    db.sqlite \
    bragg-peak.sim \
    cobalt-decay.sim \
    visinit.mac \
    visupdate.mac \
    cerenkov.sim

RESOURCES += \
    resources.qrc


