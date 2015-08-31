#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "DetectorConstruction.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "PhysicsList.hh"
#include <sys/time.h>
#include "Results.hh"
#include "DetectorConstruction.hh"
#include "G4EmCalculator.hh"
#include "EvaporationChannel.hh"

class RunAction : public G4UserRunAction
{
  public:
  RunAction(PhysicsList*,Results*,DetectorConstruction*,EvaporationChannel*);
   ~RunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
  timeval GetStartTime(){return ts;};
  timeval ts;

  private:
  
  PhysicsList* thePhysicsList;
  Results* theResults;
  DetectorConstruction* theDetector;
  EvaporationChannel* theEvaporationChannel;
 
};




#endif

    
