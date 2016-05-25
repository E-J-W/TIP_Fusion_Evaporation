#include "Run_Messenger.hh"


Run_Messenger::Run_Messenger(G4RunManager* rm, PhysicsList* physl)
:theRunManager(rm),thePhysicsList(physl)
{ 
 
  thePLDir = new G4UIdirectory("/Physics/");
  thePLDir->SetGuidance("Run physics control.");
  
  theRDir = new G4UIdirectory("/Run/");
  theRDir->SetGuidance("Run control.");
  
  PLSCmd = new G4UIcmdWithADoubleAndUnit("/Physics/Step",this);
  PLSCmd->SetGuidance("Set the step size for ion physics in the simulation.");
  PLSCmd->SetParameterName("Step size",false);
  PLSCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  ICmd = new G4UIcmdWithoutParameter("/Run/Initialize",this);
  ICmd->SetGuidance("Initialize the run.");

}



Run_Messenger::~Run_Messenger()
{
  delete PLSCmd;
}


void Run_Messenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 

  if( command == PLSCmd )
    {
      thePhysicsList->setStepSize(PLSCmd->GetNewDoubleValue(newValue));
      //theRunManager->PhysicsHasBeenModified();
      //theRunManager->SetUserInitialization(thePhysicsList);
    }
  if( command == ICmd )
    {
      G4cout << "---------- INITIALIZING RUN ----------"<< G4endl;
      theRunManager->Initialize();
      G4cout << "---------- DONE RUN INITIALIZATION ----------"<< G4endl;
    }

}

