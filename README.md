<h2>Simple: Geant4 simplified</h2>

![Screenshot](https://github.com/deepaksamuel/simple/blob/master/simple-bragg.png)

Simple is a front-end to perform Geant4 simulations of particle interactions with matter, without writing any code. It integrates user-friendly tools to create simple geometries, visualize tracks and to perform data analysis with graphing. All you need to do is to download the datasets from Geant4 download page (https://geant4.web.cern.ch/support/download.) and run the Appimage appropriate to your system:

Ubuntu 16 and above: https://github.com/deepaksamuel/simple/releases/download/v0.8-alpha/simple-ubuntu-16.04-x64-allfiles.zip

Ubuntu 19 onwards: https://github.com/deepaksamuel/simple/releases/download/v0.7-alpha/Simple-Ubuntu19.10-x86_64.AppImage


Simple can also be used as a demonstration tool in nuclear and particle physics/ medical physics classes and can aid in teaching students about data analysis techniques, in general, using external libraries like python.


<b>AppImages are what executables are for Windows. No need to compile or download Geant4 or ROOT. Just download and double click the app image and you are ready to go!</b>


When the application is run for the first time, you mush check the "First time use" checkbox in the first dialog box that appears. Next, you must set the Geant4 dataset locations in the user interface that comes up and click update. If there are any errors in the folder paths, the output log will display them. This has to be done only once on a system as long as the datasets are not moved to a different location.

<b>Also, do not forget to place the db.sqlite (available in this repo) file next to the app. This is very important. In case you don't do it, the application creates its own db.sqlite file which is empty and useless. </b>

As a first example:

#Open bragg-peak.sim file in the application and click on "Shoot beam". Go to Scoring mesh tab on the left side and right click on the mesh to plot projection. Wait for a few seconds for the simulation to run.

#In the dataframe tab, open the data folder which is created automatically in the same folder as your application. This folder contains all the data files created in your simulation. 


#Double click on temp-energyDeposit-Box-Mesh.root

#Go to the plots tab and type "totalvalueMeV:binz" and press enter to see the Bragg peak.

<b> Branches in the output file </b>

The output files has the following branches:
  
Event ID: eid
  
Particle ID: pid
  
Detector ID: detecorId
  
X/Y/Z position: posx/posy/posz [mm]

Global time: globTime [ns]

Proper time: propTime [ns]

Local time: localTime [ns]

Energy deposited: tot_edep [MeV]

Energy: tot_E [MeV]

Kinetic Energy: tot_KE [MeV]

Momentum (magnitude) x/y/z: momx / momy / momz [MeV/c]

Momentum (direction vectors) x/y/z: px / py / pz
