#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T01:14:38
#
#-------------------------------------------------

#QT       += core gui opengl


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
    physics_lists/PhysicsList_MuDecayWithSpin.cc \
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
    SimpleOpenGLStoredQtViewer.cpp \
    physics_lists/F04StepMax.cc \
    physics_lists/PhysListEmStandard.cc \
    physics_lists/F04ExtraPhysics.cc \
    physics_lists/MyG4EmStandardPhysics_option4.cc \
    physics_lists/F04PhysicsListMessenger.cc






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
    physics_lists/PhysicsList_MuDecayWithSpin.hh \
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
    SimpleOpenGLQtViewer.h \
    physics_lists/F04StepMax.hh \
    physics_lists/PhysListEmStandard.hh \
    physics_lists/F04ExtraPhysics.hh \
    physics_lists/MyG4EmStandardPhysics_option4.hh \
    physics_lists/F04PhysicsListMessenger.hh




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



message("Please run Qtcreator from the command line and make sure that ROOTSYS and GEANT4 environment variables are set in order to automatically set the paths for compilation")
#Geant4 settings
QMAKE_CXXFLAGS+=$$system(geant4-config --cflags)
LIBS+=$$system(geant4-config --libs)

#for ubuntu 16
#message("Using xerces-c-3.1 for Ubuntu 16. Please change this in the project file, if you are running another OS")
#LIBS+= -L/usr/lib/x86_64-linux-gnu/ -lxerces-c-3.1

#for ubuntu 19
message("Using xerces-c-3.2 for Ubuntu 19. Please change this in the project file, if you are running another OS")
LIBS+= -L/usr/lib/x86_64-linux-gnu/ -lxerces-c-3.2

#ROOT settings
LIBS+=$$system(root-config --glibs)
QMAKE_CXXFLAGS+=$$system(root-config --cflags)



DISTFILES += \
    bragg-peak.sim \
    cobalt-decay.sim \
    visinit.mac \
    visupdate.mac \
    cerenkov.sim \
    README \
    ../linuxdeploy/linuxdeploy-centos6-x86_64.AppImage \
    ../linuxdeploy/linuxdeploy-plugin-appimage-x86_64.AppImage \
    ../linuxdeploy/linuxdeploy-plugin-qt-x86_64.AppImage \
    musr.sim \
    ICAL.sim \
    mICAL.sim \
    ICAL2.sim \
    models/bragg-peak.sim \
    models/cerenkov.sim \
    models/cobalt-decay.sim \
    models/ICAL2.sim \
    models/mICAL.sim \
    models/musr.sim \
    models/musr.sim \
    models/mICAL.sim \
    models/cobalt-decay.sim \
    models/cerenkov.sim \
    models/bragg-peak.sim \
    models/ICAL2.sim \
    database/db.sqlite \
    deploy-tools/linuxdeploy-plugin-appimage-x86_64.AppImage \
    deploy-tools/linuxdeploy-plugin-qt-x86_64.AppImage \
    deploy-tools/linuxdeploy-x86_64.AppImage \
    deploy-tools/create-app \
    deploy-tools/db.sqlite \
    deploy-tools/app-files/simple.desktop \
    deploy-tools/app-files/simple.png \
    macros/visupdate.mac \
    macros/visinit.mac \
    models/gold-foil.sim \
    deploy-tools/models/gold-foil.sim \
    deploy-tools/models/musr.sim \
    deploy-tools/models/mICAL.sim \
    deploy-tools/models/cobalt-decay.sim \
    deploy-tools/models/cerenkov.sim \
    deploy-tools/models/bragg-peak.sim \
    deploy-tools/models/ICAL2.sim \
    deploy-tools/install-g4-dependencies

RESOURCES += \
    resources.qrc













#Enable this section for Ubuntu 19 version of our code
##generated from Geant4-config
#QMAKE_CXXFLAGS=-DG4INTY_USE_XT -DG4VIS_USE_OPENGL -DG4UI_USE_TCSH -DG4INTY_USE_QT -DG4UI_USE_QT -DG4VIS_USE_OPENGLQT -I/usr/include/x86_64-linux-gnu/qt5/ -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/lib/x86_64-linux-gnu/qt5//mkspecs/linux-g++ -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -DG4VIS_USE_OPENGLX -W -Wall -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-variadic-macros -Wshadow -pipe -pthread -ftls-model=initial-exec -std=c++11 -I/home/samuel/g4/g4-install/bin/../include/Geant4
#LIBS+=-L/home/samuel/g4/g4-install/bin/../lib -lG4OpenGL -lG4gl2ps -lG4Tree -lG4FR -lG4GMocren -lG4visHepRep -lG4RayTracer -lG4VRML -lG4vis_management -lG4modeling -lG4interfaces -lG4persistency -lG4analysis -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -lG4clhep -lG4expat -lG4zlib

## the following line is required for Geant4 to work
#LIBS+= -L/usr/lib/x86_64-linux-gnu/ -lxerces-c-3.2

##generated from root-config
#LIBS+=-L/home/samuel/root_v6.18.04.source/root-install/lib -lGui -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -pthread -lm -ldl -rdynamic
#QMAKE_CXXFLAGS+=-pthread -std=c++11 -m64 -I/home/samuel/root_v6.18.04.source/root-install/include


#Enable this section for Ubuntu 18 version of our code
#generated from geant4-config
#QMAKE_CXXFLAGS=-DG4INTY_USE_XT -DG4VIS_USE_OPENGL -DG4UI_USE_TCSH -DG4INTY_USE_QT -DG4UI_USE_QT -DG4VIS_USE_OPENGLQT -I/usr/include/x86_64-linux-gnu/qt5/ -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/lib/x86_64-linux-gnu/qt5//mkspecs/linux-g++ -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -DG4VIS_USE_OPENGLX -W -Wall -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-variadic-macros -Wshadow -pipe -pthread -ftls-model=initial-exec -std=c++11 -I/home/samuel/geant4/source/analysis/g4tools/include -I/home/samuel/geant4/source/analysis/accumulables/include -I/home/samuel/geant4/source/analysis/csv/include -I/home/samuel/geant4/source/analysis/g4tools/include -I/home/samuel/geant4/source/analysis/hntools/include -I/home/samuel/geant4/source/analysis/management/include -I/home/samuel/geant4/source/analysis/root/include -I/home/samuel/geant4/source/analysis/xml/include -I/home/samuel/geant4/source/digits_hits/detector/include -I/home/samuel/geant4/source/digits_hits/digits/include -I/home/samuel/geant4/source/digits_hits/hits/include -I/home/samuel/geant4/source/digits_hits/scorer/include -I/home/samuel/geant4/source/digits_hits/utils/include -I/home/samuel/geant4/source/error_propagation/include -I/home/samuel/geant4/source/event/include -I/home/samuel/geant4/source/externals/clhep/include -I/home/samuel/geant4/source/externals/expat/include -I/home/samuel/geant4/source/externals/zlib/include -I/home/samuel/g4/g4-build/source/geometry/include -I/home/samuel/geant4/source/geometry/biasing/include -I/home/samuel/geant4/source/geometry/divisions/include -I/home/samuel/geant4/source/geometry/magneticfield/include -I/home/samuel/geant4/source/geometry/management/include -I/home/samuel/geant4/source/geometry/navigation/include -I/home/samuel/geant4/source/geometry/solids/Boolean/include -I/home/samuel/geant4/source/geometry/solids/CSG/include -I/home/samuel/geant4/source/geometry/solids/specific/include -I/home/samuel/geant4/source/geometry/volumes/include -I/home/samuel/g4/g4-build/source/global/include -I/home/samuel/geant4/source/global/HEPGeometry/include -I/home/samuel/geant4/source/global/HEPNumerics/include -I/home/samuel/geant4/source/global/HEPRandom/include -I/home/samuel/geant4/source/global/management/include -I/home/samuel/geant4/source/graphics_reps/include -I/home/samuel/geant4/source/intercoms/include -I/home/samuel/geant4/source/interfaces/GAG/include -I/home/samuel/geant4/source/interfaces/basic/include -I/home/samuel/geant4/source/interfaces/common/include -I/home/samuel/geant4/source/materials/include -I/home/samuel/geant4/source/parameterisations/gflash/include -I/home/samuel/geant4/source/particles/adjoint/include -I/home/samuel/geant4/source/particles/bosons/include -I/home/samuel/geant4/source/particles/hadrons/barions/include -I/home/samuel/geant4/source/particles/hadrons/ions/include -I/home/samuel/geant4/source/particles/hadrons/mesons/include -I/home/samuel/geant4/source/particles/leptons/include -I/home/samuel/geant4/source/particles/management/include -I/home/samuel/geant4/source/particles/shortlived/include -I/home/samuel/geant4/source/particles/utils/include -I/home/samuel/geant4/source/persistency/ascii/include -I/home/samuel/geant4/source/persistency/mctruth/include -I/home/samuel/geant4/source/physics_lists/builders/include -I/home/samuel/geant4/source/physics_lists/constructors/decay/include -I/home/samuel/geant4/source/physics_lists/constructors/electromagnetic/include -I/home/samuel/geant4/source/physics_lists/constructors/factory/include -I/home/samuel/geant4/source/physics_lists/constructors/gamma_lepto_nuclear/include -I/home/samuel/geant4/source/physics_lists/constructors/hadron_elastic/include -I/home/samuel/geant4/source/physics_lists/constructors/hadron_inelastic/include -I/home/samuel/geant4/source/physics_lists/constructors/ions/include -I/home/samuel/geant4/source/physics_lists/constructors/limiters/include -I/home/samuel/geant4/source/physics_lists/constructors/stopping/include -I/home/samuel/geant4/source/physics_lists/lists/include -I/home/samuel/geant4/source/physics_lists/util/include -I/home/samuel/geant4/source/processes/biasing/management/include -I/home/samuel/geant4/source/processes/biasing/generic/include -I/home/samuel/geant4/source/processes/biasing/importance/include -I/home/samuel/geant4/source/processes/cuts/include -I/home/samuel/geant4/source/processes/decay/include -I/home/samuel/geant4/source/processes/electromagnetic/adjoint/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/processes/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/models/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/utils/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/management/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/molecules/management/include -I/home/samuel/geant4/source/processes/electromagnetic/dna/molecules/types/include -I/home/samuel/geant4/source/processes/electromagnetic/highenergy/include -I/home/samuel/geant4/source/processes/electromagnetic/lowenergy/include -I/home/samuel/geant4/source/processes/electromagnetic/muons/include -I/home/samuel/geant4/source/processes/electromagnetic/pii/include -I/home/samuel/geant4/source/processes/electromagnetic/polarisation/include -I/home/samuel/geant4/source/processes/electromagnetic/standard/include -I/home/samuel/geant4/source/processes/electromagnetic/utils/include -I/home/samuel/geant4/source/processes/electromagnetic/xrays/include -I/home/samuel/geant4/source/processes/hadronic/cross_sections/include -I/home/samuel/geant4/source/processes/hadronic/management/include -I/home/samuel/geant4/source/processes/hadronic/models/abla/include -I/home/samuel/geant4/source/processes/hadronic/models/abrasion/include -I/home/samuel/geant4/source/processes/hadronic/models/binary_cascade/include -I/home/samuel/geant4/source/processes/hadronic/models/cascade/cascade/include -I/home/samuel/geant4/source/processes/hadronic/models/coherent_elastic/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/ablation/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/evaporation/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/fermi_breakup/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/fission/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/gem_evaporation/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/handler/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/management/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/multifragmentation/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/photon_evaporation/include -I/home/samuel/geant4/source/processes/hadronic/models/de_excitation/util/include -I/home/samuel/geant4/source/processes/hadronic/models/em_dissociation/include -I/home/samuel/geant4/source/processes/hadronic/models/fission/include -I/home/samuel/geant4/source/processes/hadronic/models/im_r_matrix/include -I/home/samuel/geant4/source/processes/hadronic/models/inclxx/utils/include -I/home/samuel/geant4/source/processes/hadronic/models/inclxx/incl_physics/include -I/home/samuel/geant4/source/processes/hadronic/models/inclxx/interface/include -I/home/samuel/geant4/source/processes/hadronic/models/gamma_nuclear/include -I/home/samuel/geant4/source/processes/hadronic/models/lend/include -I/home/samuel/geant4/source/processes/hadronic/models/lepto_nuclear/include -I/home/samuel/geant4/source/processes/hadronic/models/management/include -I/home/samuel/geant4/source/processes/hadronic/models/particle_hp/include -I/home/samuel/geant4/source/processes/hadronic/models/parton_string/diffraction/include -I/home/samuel/geant4/source/processes/hadronic/models/parton_string/hadronization/include -I/home/samuel/geant4/source/processes/hadronic/models/parton_string/management/include -I/home/samuel/geant4/source/processes/hadronic/models/parton_string/qgsm/include -I/home/samuel/geant4/source/processes/hadronic/models/pre_equilibrium/exciton_model/include -I/home/samuel/geant4/source/processes/hadronic/models/qmd/include -I/home/samuel/geant4/source/processes/hadronic/models/quasi_elastic/include -I/home/samuel/geant4/source/processes/hadronic/models/radioactive_decay/include -I/home/samuel/geant4/source/processes/hadronic/models/rpg/include -I/home/samuel/geant4/source/processes/hadronic/models/theo_high_energy/include -I/home/samuel/geant4/source/processes/hadronic/models/util/include -I/home/samuel/geant4/source/processes/hadronic/processes/include -I/home/samuel/geant4/source/processes/hadronic/stopping/include -I/home/samuel/geant4/source/processes/hadronic/util/include -I/home/samuel/geant4/source/processes/management/include -I/home/samuel/geant4/source/processes/optical/include -I/home/samuel/geant4/source/processes/solidstate/phonon/include -I/home/samuel/geant4/source/processes/solidstate/channeling/include -I/home/samuel/geant4/source/processes/parameterisation/include -I/home/samuel/geant4/source/processes/scoring/include -I/home/samuel/geant4/source/processes/transportation/include -I/home/samuel/geant4/source/readout/include -I/home/samuel/geant4/source/run/include -I/home/samuel/geant4/source/track/include -I/home/samuel/geant4/source/tracking/include -I/home/samuel/geant4/source/visualization/FukuiRenderer/include -I/home/samuel/geant4/source/visualization/HepRep/include -I/home/samuel/geant4/source/visualization/RayTracer/include -I/home/samuel/geant4/source/visualization/Tree/include -I/home/samuel/geant4/source/visualization/VRML/include -I/home/samuel/geant4/source/visualization/XXX/include -I/home/samuel/geant4/source/visualization/externals/gl2ps/include -I/home/samuel/geant4/source/visualization/gMocren/include -I/home/samuel/geant4/source/visualization/management/include -I/home/samuel/geant4/source/visualization/modeling/include -I/home/samuel/geant4/source/visualization/OpenGL/include
#LIBS+=-L/home/samuel/g4/g4-build/BuildProducts/lib -lG4OpenGL -lG4gl2ps -lG4Tree -lG4FR -lG4GMocren -lG4visHepRep -lG4RayTracer -lG4VRML -lG4vis_management -lG4modeling -lG4interfaces -lG4persistency -lG4analysis -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -lG4clhep -lG4expat -lG4zlib

#QMAKE_CXXFLAGS+=-DG4INTY_USE_XT -DG4VIS_USE_OPENGL -DG4UI_USE_TCSH -DG4INTY_USE_QT -DG4UI_USE_QT -DG4VIS_USE_OPENGLQT -I/usr/include/x86_64-linux-gnu/qt5/ -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/lib/x86_64-linux-gnu/qt5//mkspecs/linux-g++ -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -DG4VIS_USE_OPENGLX -W -Wall -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-variadic-macros -Wshadow -pipe -pthread -ftls-model=initial-exec -std=c++11 -I/home/samuel/g4/g4-install/bin/./../include/Geant4
#QMAKE_CXXFLAGS+=$$system(geant4-config --cflags)
#LIBS+=-L/home/samuel/g4/g4-install/bin/./../lib -lG4OpenGL -lG4gl2ps -lG4Tree -lG4FR -lG4GMocren -lG4visHepRep -lG4RayTracer -lG4VRML -lG4vis_management -lG4modeling -lG4interfaces -lG4persistency -lG4analysis -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -lG4clhep -lG4expat -lG4zlib
#LIBS+=$$system(geant4-config --libs)

# the following line is required for Geant4 to work
#LIBS+= -L/usr/lib/x86_64-linux-gnu/ -lxerces-c-3.2
#generated from root-config
#LIBS+=-L/home/samuel/root_v6.18.04.Linux-ubuntu18-x86_64-gcc7.4/root/lib -lGui -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -pthread -lm -ldl -rdynamic
#QMAKE_CXXFLAGS+=-pthread -std=c++11 -m64 -I/home/samuel/root_v6.18.04.Linux-ubuntu18-x86_64-gcc7.4/root/include

#LIBS+=$$system(root-config --glibs)
#QMAKE_CXXFLAGS+=-pthread -std=c++11 -m64
#QMAKE_CXXFLAGS+=$$system(root-config --cflags)
#message($$system(root-config --cflags))

