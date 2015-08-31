#include "RunAction.hh"

 
RunAction::RunAction(PhysicsList* PL,Results* Res,DetectorConstruction* Det): thePhysicsList(PL), theResults(Res), theDetector(Det)
{
  
}


RunAction::~RunAction()
{

}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4double At,Zt;
  G4int nP,nN,nA;
  G4int DA,DZ;
  G4cout<<" Beginin of run "<<G4endl;
  gettimeofday(&ts,NULL);
 
  At=theDetector->GetTarget()->getTargetMass();
  Zt=theDetector->GetTarget()->getTargetCharge();
  thePhysicsList->getReaction()->SetTargetMassAndCharge(At,Zt);
  thePhysicsList->getReaction()->TargetFaceCrossSection();
  nN= thePhysicsList->getReaction()->GetNumberOfNeutrons();
  nP= thePhysicsList->getReaction()->GetNumberOfProtons();
  nA= thePhysicsList->getReaction()->GetNumberOfAlphas();
  DA=At-nN-nP-4*nA;
  DZ=Zt-nP-2*nA;
  theResults->SetupRun(DA,DZ);
}


 
void RunAction::EndOfRunAction(const G4Run*)
{
 
}

