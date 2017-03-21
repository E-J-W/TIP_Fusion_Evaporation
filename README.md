# TIP Fusion-Evaporation DSAM GEANT4 Code

Maintainer: Jonathan Williams

## Description

GEANT4 code implementing TIP/TIGRESS DSAM using a fusion-evaporation reaction process.

At present, the TIP CsI wall and GRIFFIN/TIGRESS array geometries are implemented.  Step-wise decay of the residual nucleus (gamma ray cascade) is also availaible.

## Installation

This code has been tested with GEANT4.9.4 on Scientific Linux 6, Ubuntu 14.04, Ubuntu 15.10, and Ubuntu 16.04.  On Ubuntu 16.04 this program throws memory allocation errors when compiled using the default version of the `g++` compiler, it is recommended to install and use `g++-4.8` instead.

This program depends on GEANT4.9.4, CLHEP 2.1.0.1, and ROOT v5.x (results are saved to ROOT tree files), and requires proper setup of environment variables for each, for example by adding the follwing lines to ~/.bashrc (substituting the appropriate paths):

```
#ROOT configuration
export ROOTSYS=/path/to/root
export ROOTINC=/path/to/root/include
export ROOTLIB=/path/to/root/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTLIB
export PATH=$PATH:$ROOTSYS/bin
```

```
#CLHEP configuration
export CLHEPSYS=/path/to/clhep
export PATH=$PATH:$CLHEPSYS/bin:$CLHEPSYS/lib
```

```
#for GEANT4
#the next line should point to the env.sh file generated upon installation/configuration of GEANT4
source /path/to/env.sh
export G4INSTALL=/path/to/geant4_install_directory
```

Modified GEANT4 classes included in this code:

G4IonParametrisedLossModel (from GEANT4.9.4) - modified to allow for user defined stopping powers.

G4IonStoppingData (from GEANT4.9.4) - modified to allow for user defined stopping powers.

G4NuclearDecayChannel (from GEANT4.9.4) - reimplemented as GammaDecayChannel, a decay channel with only internal conversion.  This is done to properly compute properties of daughter nuclei and emitted gammas when considering a cascade of internal conversion decays.

## Acknowledgements

A. Chester - CsI wall code.

T. Ma - CsI ball code.

C. Morse - Finding and fixing memory leaks.

K. Starosta - Initial TIP codebase.

GRIFFIN/TIGRESS code based on code provided by the GRIFFIN collaboration, available at: https://github.com/GRIFFINCollaboration/detectorSimulations
