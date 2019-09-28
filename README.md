<h2>Simple: Geant4 simplified</h2>

![Screenshot](https://github.com/deepaksamuel/simple/blob/master/simple-bragg.png)


Every particle/nuclear/medical/space physicist would have heard about Geant4 or probably even used it for their analysis. A tool designed to simulate particle interactions with matter, it is one of the tools developed at CERN used equally by researchers in academia and industry. Written in C++, it can simulate right from complex particle interactions taking place at the Large Hadron Collider (mother of Higgs Boson - the God particle) to the simple radioactive decay. One can construct detectors, shoot various particles and measure the response using Geant4.

However, it is any beginners nightmare. The learning curve is steep and most of the users tend to copy one of its examples and tweak it to get their results.

In many circumstances, one needs just "ball-park" numbers or just a proof-of-principle of some crazy ideas. To quickly code things in Geant4 and then analyze the data is not easy. The installation and initial settings are not straightforward for the first time users though there exist an excellent user forum and documentation. The code needs to be download and additional libraries have to be pre-installed in addition to the data files which are mandatory for Geant4 to run successfully. In addition, one needs to understand the physics lists and structure of Geant4 to proceed further.

Therefore, having used Geant4 for about 10 years, I thought that a tool with a design similar to RStudio, which helps one to insert detectors, create materials, shoot particles and also to analyze data in a single interface without any coding would be useful to many. Additionally, if the process of downloading and compiling Geant4 could be reduced, it would ease up things. Towards this goal, I built a tool (codename "simple") which will let you build your simulations without compiling Geant4 or ROOT and without writing any code!

Qt for user interface building, Geant4 as the simulation engine and ROOT for data analysis makes simple a really useful tool for fast simulation and analysis of particle interactions with matter. One can insert new geometries (only Box, Sphere, Cylinder and Wedge available now) and create new material composites from NIST materials. One can choose to record parameters like position, timing, energy and momentum when a particle enters a geometry and the same is stored in a ROOT file for analysis and plotting within the interface. Furthermore, one can create scoring meshes with various parameters and filters. The screenshot shown above is of a simple water cube for measuring the Bragg peak of protons. The design took a few seconds and the run took another few seconds for 10,000 particles.

This tool can also be used as a demonstration tool in nuclear and particle physics/ medical physics classes and can aid in teaching students about data analysis techniques, in general, using external libraries like python.

The application is opensource and the alpha release for Linux is available as of now as an AppImage (tested in Ubuntu 19.04) at 
https://github.com/deepaksamuel/simple/releases/download/v0.1-alpha/Simple-x86_64.AppImage

No need to compile or download Geant4 or ROOT. Just double click on the app image. If you are a first time user, you will have to set the path to the Geant4 datasets (you will have to check the "First time user" checkbox). The Geant4 datasets are available from 
https://geant4.web.cern.ch/support/download
Also, do not forget to place the db.sqlite (available in this repo) file next to the app. This is very important. In case you don't do it, the application creates its own db.sqlite file which is empty and useless. 

As a first example:
#Open bragg-peak.sim file in the application and click on "Shoot beam". Go to Scoring mesh tab on the left side and right click on the mesh to plot projection. Wait for a few seconds for the simulation to run.

#In the dataframe tab, open the data folder which is created automatically in the same folder as your application. This folder contains all the data files created in your simulation. 

#Double click on temp-energyDeposit-Box-Mesh.root

#Go to the plots tab and type "totalvalueMeV:binz" and press enter to see the Bragg peak.

#TODO:
--AppImages for other Linux distributions like CentOS
--Windows version

