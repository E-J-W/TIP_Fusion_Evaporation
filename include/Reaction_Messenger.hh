#ifndef Reaction_Messenger_h
#define Reaction_Messenger_h 1

#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "ReactionFusEvap.hh"
#include "globals.hh"

class Reaction_Messenger : public G4UImessenger {
public:
  Reaction_Messenger(ReactionFusEvap *);
  ~Reaction_Messenger();

  void SetNewValue(G4UIcommand *, G4String);

private:
  ReactionFusEvap *theReaction;
  G4UIdirectory *theReactionDir;

  G4UIcmdWithADoubleAndUnit *TauCmd;
  G4UIcmdWithADoubleAndUnit *EneCmd;
  G4UIcmdWithAString *ADecCmd;
  G4UIcmdWithAnInteger *NNCmd;
  G4UIcmdWithAnInteger *NPCmd;
  G4UIcmdWithAnInteger *NACmd;

  G4UIcmdWithAnInteger *MACmd;

  G4UIcmdWithoutParameter *SetupCmd;

  G4UIdirectory *theCompoundDir;
  G4UIcmdWithADoubleAndUnit *QCmd;

  G4UIdirectory *theEvaporationDir;
  G4UIcmdWithADoubleAndUnit *Q1Cmd;
  G4UIcmdWithADoubleAndUnit *Q2Cmd;
  G4UIcmdWithADoubleAndUnit *Q3Cmd;
  G4UIcmdWithADoubleAndUnit *Q4Cmd;
  G4UIcmdWithADouble *dExix0Cmd;
  G4UIcmdWithADouble *dExiwCmd;
  G4UIcmdWithADouble *dExitauCmd;

  G4UIcmdWithoutParameter *CARCmd;
  G4UIcmdWithADoubleAndUnit *CARMaxCmd;
  G4UIcmdWithADoubleAndUnit *CARMinCmd;
};

#endif
