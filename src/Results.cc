#include "Results.hh"

Results::Results(Projectile *proj, DetectorConstruction *det,
                 PhysicsList *physl)
    : theProjectile(proj), theDetector(det), thePhysicsList(physl) {

  soh = sizeof(partHit);
  sogh = sizeof(GHit);
  soi = sizeof(gun);
  sos = sizeof(stat);
  soes = sizeof(eStat);
  memset(CP, 0, sizeof(CP));
}

Results::~Results() { tree->Delete(); }
//---------------------------------------------------------
void Results::SetupRun(Int_t nP, Int_t nN, Int_t nA) {
  // Get A, Z of the projectile and recoiling residual (needed for particle
  // identification when saving data to tree)
  numP = nP;
  numN = nN;
  numA = nA;
  Ap = theProjectile->getA();
  Zp = theProjectile->getZ();
  switch(theDetector->GetTargetType())
  {
    case 1:
      //plunger
      Ar = theProjectile->getA() + theDetector->GetPlunger()->getTargetMass() - numP - numN - numA * 4;
      Zr = theProjectile->getZ() + theDetector->GetPlunger()->getTargetCharge() - numP - numA * 2;
      //   G4cout << "Results: target A: " << theDetector->GetPlunger()->getTargetMass() << ", target Z: " <<theDetector->GetPlunger()->getTargetCharge() << G4endl;
      break;
    default:
      //dsam target
      Ar = theProjectile->getA() + theDetector->GetTarget()->getTargetMass() - numP - numN - numA * 4;
      Zr = theProjectile->getZ() + theDetector->GetTarget()->getTargetCharge() - numP - numA * 2;
      // G4cout << "Results: target A: " << theDetector->GetTarget()->getTargetMass() << ", target Z: " << theDetector->GetTarget()->getTargetCharge() << G4endl;
      break;
  } 
  //  G4cout << "Results: projectile A: " << Ap << ", projectile Z: " << Zp << ",recoil A: " << Ar << ", recoil Z: " << Zr << G4endl;
  

  // get HPGe and CsI crystal positions
  if (theDetector->usingCsIBall())
    GetCsIBallPositions();
  else
    GetCsIWallPositions();
  /*for(int i=0; i<GN; i++)
    for(int j=0; j<GS; j++)
      {
              CP[i][j]=theDetector->GetDetectorCrystalPosition(i,j);
              printf("HPGe position %d crystal %d x %f y %f z
    %f\n",i+1,j,CP[i][j].getX(),CP[i][j].getY(),CP[i][j].getZ());
      }*/

  numDec = thePhysicsList->getReaction()->GetNumDecays();
  G4cout << "Results - number of decays: " << numDec << G4endl;

  G4cout << "Results - creating tree... ";
  TreeCreate();
  G4cout << "created!" << G4endl;

  // getc(stdin);
}
//---------------------------------------------------------
void Results::TreeCreate() {

  char branchName[256];

  if (tree == NULL) {
    tree = new TTree("tree", "tree");
    tree->Branch("Gfold", &GHit.Gfold, "Gfold/I");
    tree->Branch("GId", GHit.GId, "GId[Gfold]/I");
    tree->Branch("GSeg", GHit.GSeg, "GSeg[Gfold]/I");
    tree->Branch("GRing", GHit.GRing, "GRing[Gfold]/I");
    tree->Branch("Gx", GHit.Gx, "Gx[Gfold]/D");
    tree->Branch("Gy", GHit.Gy, "Gy[Gfold]/D");
    tree->Branch("Gz", GHit.Gz, "Gz[Gfold]/D");
    tree->Branch("GE", GHit.GE, "GE[Gfold]/D");
    tree->Branch("GW", GHit.GW, "GW[Gfold]/D");
    tree->Branch("GT", GHit.GT, "GT[Gfold]/D");
    tree->Branch("GfoldAddBack", &GHit.GfoldAB, "GfoldAB/I");
    tree->Branch("GIdAddBack", GHit.GIdAB, "GIdAB[GfoldAB]/I");
    tree->Branch("GSegAddBack", GHit.GSegAB, "GSegAB[GfoldAB]/I");
    tree->Branch("GRingAddBack", GHit.GRingAB, "GRingAB[GfoldAB]/I");
    tree->Branch("GxAddBack", GHit.GxAB, "GxAB[GfoldAB]/D");
    tree->Branch("GyAddBack", GHit.GyAB, "GyAB[GfoldAB]/D");
    tree->Branch("GzAddBack", GHit.GzAB, "GzAB[GfoldAB]/D");
    tree->Branch("GEAddBack", GHit.GEAB, "GEAB[GfoldAB]/D");
    tree->Branch("dsfold", &eStat.dsfold, "dsfold/I");
    tree->Branch("ds", &eStat.ds, "ds[dsfold]/D");
    tree->Branch("calcERes", &eStat.calcERes, "calcERes[dsfold]/D");
    tree->Branch("CsIfold", &partHit.CsIfold, "CsIfold/I");
    tree->Branch("CsIx", partHit.x, "x[CsIfold]/D");
    tree->Branch("CsIy", partHit.y, "y[CsIfold]/D");
    tree->Branch("CsIz", partHit.z, "z[CsIfold]/D");
    tree->Branch("CsIpx", partHit.px, "px[CsIfold]/D");
    tree->Branch("CsIpy", partHit.py, "py[CsIfold]/D");
    tree->Branch("CsIpz", partHit.pz, "pz[CsIfold]/D");
    tree->Branch("CsIE", partHit.E, "E[CsIfold]/D");
    tree->Branch("CsIb", partHit.b, "b[CsIfold]/D");
    tree->Branch("CsIw", partHit.w, "w[CsIfold]/D");
    tree->Branch("CsIA", partHit.A, "A[CsIfold]/I");
    tree->Branch("CsIZ", partHit.Z, "Z[CsIfold]/I");
    tree->Branch("CsIId", partHit.Id, "Id[CsIfold]/I");
    tree->Branch("CsIr", partHit.r, "r[CsIfold]/I");
    tree->Branch("CsIpath", partHit.path, "path[CsIfold]/D");
    tree->Branch("CsIdE", partHit.Id, "dE[CsIfold]/D");
    tree->Branch("CsIdEdx", partHit.dEdx, "dEdx[CsIfold]/D");
    tree->Branch("CsIdLdx", partHit.dLdx, "dLdx[CsIfold]/D");
    tree->Branch("CsILY", partHit.LY, "LY[CsIfold]/D");
    tree->Branch("projGun", &gun,
                 "x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/"
                 "D:phi/D:w/D"); // projectile when shot from the particle gun
    tree->Branch("projReactionIn", &pRIn,
                 "x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/"
                 "D:phi/D:w/D"); // projectile at the reaction point
    tree->Branch("resReactionOut", &rROut,
                 "x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/"
                 "D:phi/D:w/D"); // residual at the reaction point
    //setup branches that are relevant to the chosen target
    switch(theDetector->GetTargetType())
    {
      case 1:
        //plunger
        tree->Branch("projBackingIn", &pBIn_plunger,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/""D:phi/D:w/D"); // projectile upon entering the target  
        tree->Branch("projTargetIn", &pTIn_plunger,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/""D:phi/D:w/D"); // projectile upon entering the target 
        tree->Branch("resTargetOut", &rTOut_plunger,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/""D"); // residual upon leaving the target
        tree->Branch("resDegraderIn", &rDIn_plunger,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/""D"); // residual upon entering the degrader  
        tree->Branch("resDegraderOut", &rDOut_plunger,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/""D"); // residual upon leaving the degrader
        break;
      default:
        //dsam target
        tree->Branch("projTargetIn", &pTIn_dsam,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/""D:phi/D:w/D"); // projectile upon entering the target
        tree->Branch("resBackingIn", &rBIn_dsam,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/""D"); // residual upon leaving the target/entering the backing
        tree->Branch("resBackingOut", &rBOut_dsam,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/""D"); // residual upon leaving the backing (if it makes it that far)
        break;
    } 
      

    for (int i = 0; i < numDec; i++) // make branch for each decay
      if (i < MAXNUMDECAYS) {
        sprintf(branchName, "resDec%i", i + 1);
        tree->Branch(
            branchName, &rDec[i],
            "x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:tROffset/D:theta/D:phi/D:w/"
            "D"); // residual upon decaying via gamma emission
      }
    tree->Branch("partReactionOut", &partROut,
                 "x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:t/D:theta/D:phi/D:w/"
                 "D"); // evaporated particle at the reaction point
 } else {
    printf("Tree not deleted, could not create new tree!\n");
    getc(stdin);
  }
}
//---------------------------------------------------------
void Results::TreeClear() {
  tree->Delete("all");
  tree = NULL;
  TreeCreate();
}
//---------------------------------------------------------
void Results::TreeReport() { tree->Print(); }
//---------------------------------------------------------
void Results::TreeSave(G4String name) {
  TDirectory *dir = new TDirectory();
  dir = gDirectory;
  TFile f(name, "recreate");
  f.cd();
  // tree->AutoSave();
  // tree->SetDirectory(dir);
  tree->Write();
  f.Close();
  dir->cd();
  G4cout << "Trees of simulated parameters saved in file " << name << G4endl;
}
//---------------------------------------------------------
void Results::FillTree(G4int evtNb, TrackerIonHitsCollection *IonCollection,
                       TrackerCsIHitsCollection *CsICollection,
                       G4double gw[GN][GS], G4double ge[GN][GS],
                       G4ThreeVector gp[GN][GS], G4double gt[GN][GS]) {

  G4int Nt = IonCollection->entries();

  memset(&eStat, 0, soes);
  memset(&stat, 0, sos);
  memset(&gun, 0, soi);
  memset(&pBIn_plunger, 0, soi);
  memset(&pTIn_plunger, 0, soi);
  memset(&rTOut_plunger,0,soi);
  memset(&rDIn_plunger,0,soi);
  memset(&rDOut_plunger,0,soi);
  memset(&pTIn_dsam,0,soi);
  memset(&rBIn_dsam,0,soi);
  memset(&rBOut_dsam,0,soi);
  memset(&pRIn, 0, soi);
  memset(&rROut, 0, soi);
 
 
  for (int i = 0; i < numDec; i++)
    if (i < MAXNUMDECAYS)
      memset(&rDec[i], 0, soi);

  memset(&partROut, 0, soi);
  memset(&partHit, 0, soh);
  // memset(&GHit,0,sogh);

  // evaporated particle tracking
  G4int Np = CsICollection->entries();
  if (Np > 0) {
    if (((numA > 0) && ((*CsICollection)[0]->GetA() == 4) &&
          ((*CsICollection)[0]->GetZ() == 2)) ||
        ((numP > 0) && ((*CsICollection)[0]->GetA() == 1) &&
          ((*CsICollection)[0]->GetZ() == 1)) ||
        ((numN > 0) && ((*CsICollection)[0]->GetA() == 1) &&
          ((*CsICollection)[0]->GetZ() == 0))) {
            
      partROut.x = (*CsICollection)[0]->GetPos().getX() / mm;
      partROut.y = (*CsICollection)[0]->GetPos().getY() / mm;
      partROut.z = (*CsICollection)[0]->GetPos().getZ() / mm;
      partROut.px = (*CsICollection)[0]->GetMom().getX() / MeV;
      partROut.py = (*CsICollection)[0]->GetMom().getY() / MeV;
      partROut.pz = (*CsICollection)[0]->GetMom().getZ() / MeV;
      partROut.b = (*CsICollection)[0]->GetBeta();
      partROut.E = (*CsICollection)[0]->GetKE() / MeV;
      partROut.t = (*CsICollection)[0]->GetTime() / ns;
      partROut.tROffset = 0.;
      partROut.theta = acos(((*CsICollection)[0]->GetMom().getZ()) /
                            ((*CsICollection)[0]->GetMom().mag())) /
                        degree; // angle between (0,0,1) and momentum vector
      partROut.phi =
          acos((*CsICollection)[0]->GetMom().getX() /
                sqrt((*CsICollection)[0]->GetMom().getX() *
                        (*CsICollection)[0]->GetMom().getX() +
                    (*CsICollection)[0]->GetMom().getY() *
                        (*CsICollection)[0]->GetMom().getY())) /
          degree; // angle between (1,0,0) and momentum vector in x and y
      partROut.w = (*CsICollection)[0]->GetWeight();
    }
  }


  if (Nt > 0) {
    //G4cout << "Saving ion collection data..." << G4endl;
    stat.evNb = evtNb;
    stat.Ap = Ap;
    stat.Zp = Zp;

    for (Int_t i = 0; i < Nt; i++) {
      
      //      if((*IonCollection)[i]->GetFlag()!=0)
      //	printf("Flag %d\n",(*IonCollection)[i]->GetFlag());

      if ((*IonCollection)[i]->GetFlag() == GUN_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ap)
          if ((*IonCollection)[i]->GetZ() == Zp) {
            gun.x = (*IonCollection)[i]->GetPos().getX() / mm;
            gun.y = (*IonCollection)[i]->GetPos().getY() / mm;
            gun.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            gun.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            gun.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            gun.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            gun.b = (*IonCollection)[i]->GetBeta();
            gun.E = (*IonCollection)[i]->GetKE() / MeV;
            gun.t = (*IonCollection)[i]->GetTime() / ns;
            gun.tROffset = 0.;
            gun.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                             ((*IonCollection)[i]->GetMom().mag())) /
                        degree; // angle between (0,0,1) and momentum vector
            gun.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            gun.w = (*IonCollection)[i]->GetWeight();
          }
      } else if ((*IonCollection)[i]->GetFlag() == PLUNGER_BACKING_IN_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ap)
          if ((*IonCollection)[i]->GetZ() == Zp) {
            pBIn_plunger.x = (*IonCollection)[i]->GetPos().getX() / mm;
            pBIn_plunger.y = (*IonCollection)[i]->GetPos().getY() / mm;
            pBIn_plunger.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            pBIn_plunger.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            pBIn_plunger.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            pBIn_plunger.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            pBIn_plunger.b = (*IonCollection)[i]->GetBeta();
            pBIn_plunger.E = (*IonCollection)[i]->GetKE() / MeV;
            pBIn_plunger.t = (*IonCollection)[i]->GetTime() / ns;
            pBIn_plunger.tROffset = 0.;
            pBIn_plunger.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                              ((*IonCollection)[i]->GetMom().mag())) /
                         degree; // angle between (0,0,1) and momentum vector                                                                                                       
            pBIn_plunger.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y                                                                                                     
            pBIn_plunger.w = (*IonCollection)[i]->GetWeight();
          }
      } else if ((*IonCollection)[i]->GetFlag() == PLUNGER_TARGET_IN_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ap)
          if ((*IonCollection)[i]->GetZ() == Zp) {
            pTIn_plunger.x = (*IonCollection)[i]->GetPos().getX() / mm;
            pTIn_plunger.y = (*IonCollection)[i]->GetPos().getY() / mm;
            pTIn_plunger.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            pTIn_plunger.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            pTIn_plunger.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            pTIn_plunger.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            pTIn_plunger.b = (*IonCollection)[i]->GetBeta();
            pTIn_plunger.E = (*IonCollection)[i]->GetKE() / MeV;
            pTIn_plunger.t = (*IonCollection)[i]->GetTime() / ns;
            pTIn_plunger.tROffset = 0.;
            pTIn_plunger.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                              ((*IonCollection)[i]->GetMom().mag())) /
                         degree; // angle between (0,0,1) and momentum vector
            pTIn_plunger.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            pTIn_plunger.w = (*IonCollection)[i]->GetWeight();
          }
      }
      else if ((*IonCollection)[i]->GetFlag() == PLUNGER_TARGET_OUT_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ar)
          if ((*IonCollection)[i]->GetZ() == Zr) {
            rTOut_plunger.x = (*IonCollection)[i]->GetPos().getX() / mm;
            rTOut_plunger.y = (*IonCollection)[i]->GetPos().getY() / mm;
            rTOut_plunger.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            rTOut_plunger.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            rTOut_plunger.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            rTOut_plunger.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            rTOut_plunger.b = (*IonCollection)[i]->GetBeta();
            rTOut_plunger.E = (*IonCollection)[i]->GetKE() / MeV;
            rTOut_plunger.t = (*IonCollection)[i]->GetTime() / ns;
            rTOut_plunger.tROffset = 0.;
            rTOut_plunger.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                              ((*IonCollection)[i]->GetMom().mag())) /
                         degree; // angle between (0,0,1) and momentum vector
            rTOut_plunger.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            rTOut_plunger.w = (*IonCollection)[i]->GetWeight();
          }
      }
      else if ((*IonCollection)[i]->GetFlag() == PLUNGER_DEGRADER_IN_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ar)
          if ((*IonCollection)[i]->GetZ() == Zr) {
            rDIn_plunger.x = (*IonCollection)[i]->GetPos().getX() / mm;
            rDIn_plunger.y = (*IonCollection)[i]->GetPos().getY() / mm;
            rDIn_plunger.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            rDIn_plunger.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            rDIn_plunger.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            rDIn_plunger.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            rDIn_plunger.b = (*IonCollection)[i]->GetBeta();
            rDIn_plunger.E = (*IonCollection)[i]->GetKE() / MeV;
            rDIn_plunger.t = (*IonCollection)[i]->GetTime() / ns;
            rDIn_plunger.tROffset = 0.;
            rDIn_plunger.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                               ((*IonCollection)[i]->GetMom().mag())) /
                          degree; // angle between (0,0,1) and momentum vector
            rDIn_plunger.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            rDIn_plunger.w = (*IonCollection)[i]->GetWeight();
	    }
        }
      else if ((*IonCollection)[i]->GetFlag() == PLUNGER_DEGRADER_OUT_FLAG) {
	if ((*IonCollection)[i]->GetA() == Ar)
          if ((*IonCollection)[i]->GetZ() == Zr) {
            rDOut_plunger.x = (*IonCollection)[i]->GetPos().getX() / mm;
            rDOut_plunger.y = (*IonCollection)[i]->GetPos().getY() / mm;
            rDOut_plunger.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            rDOut_plunger.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            rDOut_plunger.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            rDOut_plunger.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            rDOut_plunger.b = (*IonCollection)[i]->GetBeta();
            rDOut_plunger.E = (*IonCollection)[i]->GetKE() / MeV;
            rDOut_plunger.t = (*IonCollection)[i]->GetTime() / ns;
            rDOut_plunger.tROffset = 0.;
            rDOut_plunger.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                               ((*IonCollection)[i]->GetMom().mag())) /
                          degree; // angle between (0,0,1) and momentum vector                                                                                                             
            rDOut_plunger.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y                                                                                                            
            rDOut_plunger.w = (*IonCollection)[i]->GetWeight();
            }
        } else if ((*IonCollection)[i]->GetFlag() == DSAM_TARGET_IN_FLAG) {
          if ((*IonCollection)[i]->GetA() == Ap)
            if ((*IonCollection)[i]->GetZ() == Zp) {
              pTIn_dsam.x = (*IonCollection)[i]->GetPos().getX() / mm;
              pTIn_dsam.y = (*IonCollection)[i]->GetPos().getY() / mm;
              pTIn_dsam.z = (*IonCollection)[i]->GetPos().getZ() / mm;
              pTIn_dsam.px = (*IonCollection)[i]->GetMom().getX() / MeV;
              pTIn_dsam.py = (*IonCollection)[i]->GetMom().getY() / MeV;
              pTIn_dsam.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
              pTIn_dsam.b = (*IonCollection)[i]->GetBeta();
              pTIn_dsam.E = (*IonCollection)[i]->GetKE() / MeV;
              pTIn_dsam.t = (*IonCollection)[i]->GetTime() / ns;
              pTIn_dsam.tROffset = 0.;
              pTIn_dsam.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                                ((*IonCollection)[i]->GetMom().mag())) /
                          degree; // angle between (0,0,1) and momentum vector
              pTIn_dsam.phi =
                  acos((*IonCollection)[i]->GetMom().getX() /
                      sqrt((*IonCollection)[i]->GetMom().getX() *
                                (*IonCollection)[i]->GetMom().getX() +
                            (*IonCollection)[i]->GetMom().getY() *
                                (*IonCollection)[i]->GetMom().getY())) /
                  degree; // angle between (1,0,0) and momentum vector in x and y
              pTIn_dsam.w = (*IonCollection)[i]->GetWeight();
            }
        } else if ((*IonCollection)[i]->GetFlag() == DSAM_BACKING_IN_FLAG) {
          if ((*IonCollection)[i]->GetA() == Ar)
            if ((*IonCollection)[i]->GetZ() == Zr) {
              rBIn_dsam.x = (*IonCollection)[i]->GetPos().getX() / mm;
              rBIn_dsam.y = (*IonCollection)[i]->GetPos().getY() / mm;
              rBIn_dsam.z = (*IonCollection)[i]->GetPos().getZ() / mm;
              rBIn_dsam.px = (*IonCollection)[i]->GetMom().getX() / MeV;
              rBIn_dsam.py = (*IonCollection)[i]->GetMom().getY() / MeV;
              rBIn_dsam.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
              rBIn_dsam.b = (*IonCollection)[i]->GetBeta();
              rBIn_dsam.E = (*IonCollection)[i]->GetKE() / MeV;
              rBIn_dsam.t = (*IonCollection)[i]->GetTime() / ns;
              rBIn_dsam.tROffset = 0.;
              rBIn_dsam.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                                ((*IonCollection)[i]->GetMom().mag())) /
                          degree; // angle between (0,0,1) and momentum vector
              rBIn_dsam.phi =
                  acos((*IonCollection)[i]->GetMom().getX() /
                      sqrt((*IonCollection)[i]->GetMom().getX() *
                                (*IonCollection)[i]->GetMom().getX() +
                            (*IonCollection)[i]->GetMom().getY() *
                                (*IonCollection)[i]->GetMom().getY())) /
                  degree; // angle between (1,0,0) and momentum vector in x and y
              rBIn_dsam.w = (*IonCollection)[i]->GetWeight();
            }
        } else if ((*IonCollection)[i]->GetFlag() == DSAM_BACKING_OUT_FLAG) {
          if ((*IonCollection)[i]->GetA() == Ar)
            if ((*IonCollection)[i]->GetZ() == Zr) {
              rBOut_dsam.x = (*IonCollection)[i]->GetPos().getX() / mm;
              rBOut_dsam.y = (*IonCollection)[i]->GetPos().getY() / mm;
              rBOut_dsam.z = (*IonCollection)[i]->GetPos().getZ() / mm;
              rBOut_dsam.px = (*IonCollection)[i]->GetMom().getX() / MeV;
              rBOut_dsam.py = (*IonCollection)[i]->GetMom().getY() / MeV;
              rBOut_dsam.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
              rBOut_dsam.b = (*IonCollection)[i]->GetBeta();
              rBOut_dsam.E = (*IonCollection)[i]->GetKE() / MeV;
              rBOut_dsam.t = (*IonCollection)[i]->GetTime() / ns;
              rBOut_dsam.tROffset = 0.;
              rBOut_dsam.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                                ((*IonCollection)[i]->GetMom().mag())) /
                            degree; // angle between (0,0,1) and momentum vector
              rBOut_dsam.phi =
                  acos((*IonCollection)[i]->GetMom().getX() /
                      sqrt((*IonCollection)[i]->GetMom().getX() *
                                (*IonCollection)[i]->GetMom().getX() +
                            (*IonCollection)[i]->GetMom().getY() *
                                (*IonCollection)[i]->GetMom().getY())) /
                  degree; // angle between (1,0,0) and momentum vector in x and y
              rBOut_dsam.w = (*IonCollection)[i]->GetWeight();
            }
        }      
    }
      
    for (Int_t i = 0; i < Nt; i++) {
      if ((*IonCollection)[i]->GetPFlag() == REACTION_IN_FLAG) {
        if ((*IonCollection)[i]->GetA() == Ap)
          if ((*IonCollection)[i]->GetZ() == Zp) {
            pRIn.x = (*IonCollection)[i]->GetPos().getX() / mm;
            pRIn.y = (*IonCollection)[i]->GetPos().getY() / mm;
            pRIn.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            pRIn.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            pRIn.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            pRIn.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            pRIn.b = (*IonCollection)[i]->GetBeta();
            pRIn.E = (*IonCollection)[i]->GetKE() / MeV;
            pRIn.t = (*IonCollection)[i]->GetTime() / ns;
            pRIn.tROffset = 0.;
            pRIn.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                              ((*IonCollection)[i]->GetMom().mag())) /
                         degree; // angle between (0,0,1) and momentum vector
            pRIn.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            pRIn.w = (*IonCollection)[i]->GetWeight();
          }
      } else if ((*IonCollection)[i]->GetPFlag() == REACTION_OUT_FLAG) {
        // residual nucleus tracking
        if ((*IonCollection)[i]->GetA() == Ar)
          if ((*IonCollection)[i]->GetZ() == Zr) {
            rROut.x = (*IonCollection)[i]->GetPos().getX() / mm;
            rROut.y = (*IonCollection)[i]->GetPos().getY() / mm;
            rROut.z = (*IonCollection)[i]->GetPos().getZ() / mm;
            rROut.px = (*IonCollection)[i]->GetMom().getX() / MeV;
            rROut.py = (*IonCollection)[i]->GetMom().getY() / MeV;
            rROut.pz = (*IonCollection)[i]->GetMom().getZ() / MeV;
            rROut.b = (*IonCollection)[i]->GetBeta();
            rROut.E = (*IonCollection)[i]->GetKE() / MeV;
            rROut.t = (*IonCollection)[i]->GetTime() / ns;
            rROut.tROffset = 0.;
            rROut.theta = acos(((*IonCollection)[i]->GetMom().getZ()) /
                               ((*IonCollection)[i]->GetMom().mag())) /
                          degree; // angle between (0,0,1) and momentum vector
            rROut.phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            rROut.w = (*IonCollection)[i]->GetWeight();
          }
      } else if ((*IonCollection)[i]->GetPFlag() >= DECAY_FLAG) {
        G4int flag = (*IonCollection)[i]->GetPFlag();
        if ((*IonCollection)[i]->GetA() == Ar)
          if ((*IonCollection)[i]->GetZ() == Zr) {
            rDec[flag - DECAY_FLAG].x =
                (*IonCollection)[i]->GetPos().getX() / mm;
            rDec[flag - DECAY_FLAG].y =
                (*IonCollection)[i]->GetPos().getY() / mm;
            rDec[flag - DECAY_FLAG].z =
                (*IonCollection)[i]->GetPos().getZ() / mm;
            rDec[flag - DECAY_FLAG].px =
                (*IonCollection)[i]->GetMom().getX() / MeV;
            rDec[flag - DECAY_FLAG].py =
                (*IonCollection)[i]->GetMom().getY() / MeV;
            rDec[flag - DECAY_FLAG].pz =
                (*IonCollection)[i]->GetMom().getZ() / MeV;
            rDec[flag - DECAY_FLAG].b = (*IonCollection)[i]->GetBeta();
            rDec[flag - DECAY_FLAG].E = (*IonCollection)[i]->GetKE() / MeV;
            rDec[flag - DECAY_FLAG].t = (*IonCollection)[i]->GetTime() / ns;
            rDec[flag - DECAY_FLAG].tROffset =
                rDec[flag - DECAY_FLAG].t - pRIn.t;
            rDec[flag - DECAY_FLAG].theta =
                acos(((*IonCollection)[i]->GetMom().getZ()) /
                     ((*IonCollection)[i]->GetMom().mag())) /
                degree; // angle between (0,0,1) and momentum vector
            rDec[flag - DECAY_FLAG].phi =
                acos((*IonCollection)[i]->GetMom().getX() /
                     sqrt((*IonCollection)[i]->GetMom().getX() *
                              (*IonCollection)[i]->GetMom().getX() +
                          (*IonCollection)[i]->GetMom().getY() *
                              (*IonCollection)[i]->GetMom().getY())) /
                degree; // angle between (1,0,0) and momentum vector in x and y
            rDec[flag - DECAY_FLAG].w = (*IonCollection)[i]->GetWeight();
          }
      }
    }
  } // end of ion collection entry saving

  // record time offsets with respect to the reaction
  gun.tROffset = gun.t - pRIn.t;
  pBIn_plunger.tROffset = pBIn_plunger.t - pRIn.t;
  pTIn_plunger.tROffset = pTIn_plunger.t - pRIn.t;
  rTOut_plunger.tROffset = rTOut_plunger.t - pRIn.t;
  rDIn_plunger.tROffset = rDIn_plunger.t-pRIn.t;
  rDOut_plunger.tROffset = rDOut_plunger.t-pRIn.t;
  pTIn_dsam.tROffset = pTIn_dsam.t-pRIn.t;
  rBIn_dsam.tROffset = rBIn_dsam.t-pRIn.t;
  rBOut_dsam.tROffset = rBOut_dsam.t-pRIn.t;
  rROut.tROffset = rROut.t - pRIn.t;
  partROut.tROffset = partROut.t - pRIn.t;
  if (gun.tROffset < 0.)
    gun.tROffset = 0.;
  if (pBIn_plunger.tROffset < 0.)
    pBIn_plunger.tROffset = 0.;
   if (pTIn_plunger.tROffset < 0.)
    pTIn_plunger.tROffset = 0.;
  if (rTOut_plunger.tROffset < 0.)
    rTOut_plunger.tROffset = 0.;
  if (rDIn_plunger.tROffset < 0.)
    rDIn_plunger.tROffset = 0.;
  if (rDOut_plunger.tROffset < 0.)
    rDOut_plunger.tROffset = 0.;
  if (pTIn_dsam.tROffset < 0.)
    pTIn_dsam.tROffset = 0.;
  if (rBIn_dsam.tROffset < 0.)
    rBIn_dsam.tROffset = 0.;
  if (rBOut_dsam.tROffset < 0.)
    rBOut_dsam.tROffset = 0.;
  if (partROut.tROffset < 0.)
    partROut.tROffset = 0.;

  partHit.CsIfold = 0;
  memset(&partHit.x, 0, sizeof(partHit.x));
  memset(&partHit.y, 0, sizeof(partHit.y));
  memset(&partHit.z, 0, sizeof(partHit.z));
  memset(&partHit.px, 0, sizeof(partHit.px));
  memset(&partHit.pz, 0, sizeof(partHit.pz));
  memset(&partHit.b, 0, sizeof(partHit.b));
  memset(&partHit.E, 0, sizeof(partHit.E));
  memset(&partHit.dE, 0, sizeof(partHit.dE));
  memset(&partHit.w, 0, sizeof(partHit.w));
  memset(&partHit.A, 0, sizeof(partHit.A));
  memset(&partHit.Z, 0, sizeof(partHit.Z));
  memset(&partHit.Id, 0, sizeof(partHit.Id));
  memset(&partHit.r, 0, sizeof(partHit.r));
  memset(&partHit.path, 0, sizeof(partHit.path));
  memset(&partHit.dEdx, 0, sizeof(partHit.dEdx));
  memset(&partHit.dLdx, 0, sizeof(partHit.dLdx));
  memset(&partHit.LY, 0, sizeof(partHit.LY));

  G4int i, j;
  if (Np > 0) {
    for (j = 1; j < NCsISph + 1; j++) // loop through CsI detector IDs (1 indexed)
    {
      if ((j < NCsI + 1) ||
          (theDetector
               ->usingCsIBall())) // check whether index is valid for wall/ball
        if ((CsITrigId == 0) ||
            (CsITrigId == j)) // save only data for the detectors we want to see
          for (i = 0; i < Np; i++) // loop through all entries in the collection
          {
            if ((*CsICollection)[i]->GetDisabled() == 0) // don't save any
                                                         // entries which have
                                                         // been disabled due to
                                                         // trigger conditions
              if ((*CsICollection)[i]->GetId() == j)     // check whether entry
              // belongs to the detector
              // we are looking at
              {
                if ((partHit.E[partHit.CsIfold] == 0.) &&
                    ((*CsICollection)[i]->GetKE() > 0.)) // first entry for a
                                                         // given detector with
                                                         // nonzero energy
                {
                  partHit.x[partHit.CsIfold] =
                      (*CsICollection)[i]->GetPos().getX() / mm;
                  partHit.y[partHit.CsIfold] =
                      (*CsICollection)[i]->GetPos().getY() / mm;
                  partHit.z[partHit.CsIfold] =
                      (*CsICollection)[i]->GetPos().getZ() / mm;
                  partHit.px[partHit.CsIfold] =
                      (*CsICollection)[i]->GetMom().getX() / MeV;
                  partHit.py[partHit.CsIfold] =
                      (*CsICollection)[i]->GetMom().getY() / MeV;
                  partHit.pz[partHit.CsIfold] =
                      (*CsICollection)[i]->GetMom().getZ() / MeV;
                  partHit.b[partHit.CsIfold] = (*CsICollection)[i]->GetBeta();
                  partHit.E[partHit.CsIfold] =
                      (*CsICollection)[i]->GetKE() / MeV;
                  partHit.dE[partHit.CsIfold] =
                      (*CsICollection)[i]->GetKE() / MeV;
                  partHit.w[partHit.CsIfold] = (*CsICollection)[i]->GetWeight();
                  partHit.A[partHit.CsIfold] = (*CsICollection)[i]->GetA();
                  partHit.Z[partHit.CsIfold] = (*CsICollection)[i]->GetZ();
                  partHit.Id[partHit.CsIfold] = (*CsICollection)[i]->GetId();
                  partHit.r[partHit.CsIfold] = (*CsICollection)[i]->GetRingId();
                  // partHit.path=(*CsICollection)[i]->GetPathLength()/um; // in
                  // microns
                  // partHit.dEdx=((*CsICollection)[i]->GetKE()/MeV)/((*CsICollection)[i]->GetPathLength()/um);
                  partHit.path[partHit.CsIfold] =
                      ((*CsICollection)[i]->GetPathLength() / cm) * CsIDensity *
                      1000; // in mg/cm^2
                  partHit.dEdx[partHit.CsIfold] =
                      ((*CsICollection)[i]->GetKE() / MeV) /
                      (((*CsICollection)[i]->GetPathLength() / cm) *
                       CsIDensity * 1000);
                  partHit.dLdx[partHit.CsIfold] = CalculateBirksLawStep(
                      partHit.Id[partHit.CsIfold], partHit.dE[partHit.CsIfold],
                      partHit.dEdx[partHit.CsIfold]); // make sure to check
                                                      // units of kB!
                  partHit.LY[partHit.CsIfold] = partHit.dLdx[partHit.CsIfold];
                  // G4cout<<"CsI ID in tree: "<<partHit.Id<<G4endl;
                  // printf("particle: dE %9.3f path %9.3f  dE/dx %9.3f  dL
                  // %9.3f  LY
                  // %9.3f\n",partHit.E,partHit.path,partHit.dEdx,partHit.dLdx,partHit.LY);
                } else if ((*CsICollection)[i]->GetKE() > 0.) // later entries
                                                              // for a given
                                                              // detector with
                                                              // nonzero energy
                {
                  partHit.E[partHit.CsIfold] +=
                      (*CsICollection)[i]->GetKE() / MeV;
                  partHit.dE[partHit.CsIfold] =
                      (*CsICollection)[i]->GetKE() / MeV;
                  // partHit.path+=(*CsICollection)[i]->GetPathLength()/um; //
                  // in microns
                  // partHit.dEdx=((*CsICollection)[i]->GetKE()/MeV)/((*CsICollection)[i]->GetPathLength()/um);
                  partHit.path[partHit.CsIfold] +=
                      ((*CsICollection)[i]->GetPathLength() / cm) * CsIDensity *
                      1000; // in mg/cm^2
                  partHit.dEdx[partHit.CsIfold] =
                      ((*CsICollection)[i]->GetKE() / MeV) /
                      (((*CsICollection)[i]->GetPathLength() / cm) *
                       CsIDensity * 1000);
                  partHit.dLdx[partHit.CsIfold] = CalculateBirksLawStep(
                      partHit.Id[partHit.CsIfold], partHit.dE[partHit.CsIfold],
                      partHit.dEdx[partHit.CsIfold]); // make sure to check
                                                      // units of kB!
                  partHit.LY[partHit.CsIfold] += partHit.dLdx[partHit.CsIfold];
                  // printf("particle: dE %9.3f path %9.3f  dE/dx %9.3f  dL
                  // %9.3f  LY
                  // %9.3f\n",partHit.dE,partHit.path,partHit.dEdx,partHit.dLdx,partHit.LY);
                }
              }
            // getc(stdin);
          }
      if (partHit.E[partHit.CsIfold] > 0.)
        partHit.CsIfold++; // increase fold if energy in a given detector is
                           // greater than 0
    }
  } // end of CsI collection entry saving

  GHit.Gfold = 0;
  GHit.GfoldAB = 0;
  memset(maxGe, 0, sizeof(maxGe));
  memset(&GHit.GId, 0, sizeof(GHit.GId));
  memset(&GHit.GSeg, 0, sizeof(GHit.GSeg));
  memset(&GHit.GE, 0, sizeof(GHit.GE));
  memset(&GHit.GT, 0, sizeof(GHit.GT));
  memset(&GHit.GIdAB, 0, sizeof(GHit.GIdAB));
  memset(&GHit.GSegAB, 0, sizeof(GHit.GSegAB));
  memset(&GHit.GRingAB, 0, sizeof(GHit.GRingAB));
  memset(&GHit.GEAB, 0, sizeof(GHit.GEAB));
  memset(&GHit.GxAB, 0, sizeof(GHit.GxAB));
  memset(&GHit.GyAB, 0, sizeof(GHit.GyAB));
  memset(&GHit.GzAB, 0, sizeof(GHit.GzAB));
  // non-addback
  for (i = 0; i < GN; i++)   // number of positions
    for (j = 0; j < GS; j++) // number of crystals
      if (gw[i][j] > 0) {
        // G4cout<<" i "<<i<<" j "<<j<<" w "<<gw[i][j]<<G4endl;
        GHit.GId[GHit.Gfold] = i + 1;
        GHit.GSeg[GHit.Gfold] = j;
        GHit.GRing[GHit.Gfold] = RingMap(
            GHit.GId[GHit.Gfold],
            GHit.GSeg[GHit.Gfold]); // get the ring in which the hit occured
        GHit.Gx[GHit.Gfold] = gp[i][j].getX();
        GHit.Gy[GHit.Gfold] = gp[i][j].getY();
        GHit.Gz[GHit.Gfold] = gp[i][j].getZ();
        GHit.GE[GHit.Gfold] = ge[i][j];
        GHit.GW[GHit.Gfold] = gw[i][j];
        GHit.GT[GHit.Gfold] = gt[i][j];
        GHit.Gfold++;
      }

  // addback
  for (i = 0; i < GN; i++)   // number of positions
    for (j = 0; j < GS; j++) // number of crystals
      if (ge[i][j] > maxGe[i]) {
        // assign position, ring, etc. of the addback hit
        GHit.GIdAB[GHit.GfoldAB] = i + 1;
        GHit.GSegAB[GHit.GfoldAB] = j;
        GHit.GRingAB[GHit.GfoldAB] = RingMap(
            GHit.GIdAB[GHit.GfoldAB],
            GHit.GSegAB[GHit.GfoldAB]); // get the ring in which the hit occured
        GHit.GxAB[GHit.GfoldAB] = gp[i][j].getX();
        GHit.GyAB[GHit.GfoldAB] = gp[i][j].getY();
        GHit.GzAB[GHit.GfoldAB] = gp[i][j].getZ();
        if (maxGe[i] ==
            0.) // no energy deposit in this detector prior to this hit
          GHit.GfoldAB++;
        maxGe[i] = ge[i][j];
      }

  // addback energy: sum deposits over all crystals in the detector with the
  // highest energy deposit
  for (i = 0; i < GHit.GfoldAB; i++) // number of addback events
    for (j = 0; j < GS; j++)         // number of crystals
      if (ge[(GHit.GIdAB[i]) - 1][j] > 0)
        GHit.GEAB[i] += ge[(GHit.GIdAB[i]) - 1][j];

  // compute an approxiate Doppler shift based on the specific detectors where
  // gamma and evaporated particles were seen
  Double_t beta; // speed of residual (calculated)
  eStat.dsfold = 0;
  for (i = 0; i < GHit.GfoldAB; i++) // number of addback hits in the event
  {
    G4ThreeVector gammaVec = theDetector->GetDetectorCrystalPosition(
        GHit.GIdAB[eStat.dsfold] - 1, GHit.GSegAB[eStat.dsfold]);
    if (gammaVec.mag() != 0)
      gammaVec.setMag(1.0);
    // G4cout << "gammaVec: " << gammaVec << G4endl;
    G4ThreeVector resMom, partMom, partDir;
    resMom.setX(gun.px);
    resMom.setY(gun.py);
    resMom.setZ(gun.pz);
    // get residual momentum by subtracting evaporated particle momenta from
    // beam momentum
    for (j = 0; j < partHit.CsIfold; j++) {
      partMom.setX(partHit.px[j]);
      partMom.setY(partHit.py[j]);
      partMom.setZ(partHit.pz[j]);
      partDir = PP[partHit.Id[j] - 1]; // get direction to CsI detector
      partDir.setMag(
          partMom.mag()); // turn direction vector into momentum vector
      resMom -= partDir;
    }
    eStat.calcERes[eStat.dsfold] = resMom * resMom / (2. * 931.45 * Ar);
    // Print debug
    /*G4cout<<"Residual momentum (calc), hit #"<<i+1<<": "<<resMom;
    G4cout<<", magnitude:"<<resMom.mag()<<G4endl;
    G4cout<<"Energy (calc): "<<resE/MeV<<", (sim): "<<rROut.E/MeV<<G4endl;*/
    beta = resMom.mag() / (931.45 * Ar);
    // G4cout<<"Residual beta (calc): "<<beta<<", (sim): "<<rROut.b<<G4endl;
    if (resMom.mag() != 0)
      resMom.setMag(1.0);

    eStat.ds[eStat.dsfold] =
        sqrt(1 - beta * beta) / (1 - beta * resMom * gammaVec);
    eStat.dsfold++;
  }
  // printf("Gun beta: %10.10f\nReaction out beta: %10.10f\n",gun.b,rROut.b);
  // printf("Gun momentum: [%10.10f %10.10f %10.10f]\n",gun.px,gun.py,gun.pz);
  // getc(stdin);
  tree->Fill();
}
//=====================================================================================
G4int Results::RingMap(G4int id, G4int seg) {
  G4int r, s;
  s = -1;
  if (seg == 0)
    s = 0;
  if (seg == 1)
    s = 1;
  if (seg == 2)
    s = 1;
  if (seg == 3)
    s = 0;
  if (s < 0)
    return -1;

  r = -1;
  switch (id) {
  case 1:
    r = 1;
    break;
  case 2:
    r = 1;
    break;
  case 3:
    r = 1;
    break;
  case 4:
    r = 1;
    break;
  case 5:
    r = 3;
    break;
  case 6:
    r = 3;
    break;
  case 7:
    r = 3;
    break;
  case 8:
    r = 3;
    break;
  case 9:
    r = 3;
    break;
  case 10:
    r = 3;
    break;
  case 11:
    r = 3;
    break;
  case 12:
    r = 3;
    break;
  case 13:
    r = 5;
    break;
  case 14:
    r = 5;
    break;
  case 15:
    r = 5;
    break;
  case 16:
    r = 5;
    break;
  default:
    break;
  }
  if (r < 0)
    return -1;

  return r + s;
}
//=====================================================================================
void Results::ReportCrystalPositions() {
  Double_t xx, yy, zz, rr, cc;
  printf("det cry ring     x        y        z        r       cos\n");
  for (Int_t det = 0; det < GN; det++)
    for (Int_t cry = 0; cry < GS; cry++) {
      xx = CP[det][cry].getX();
      yy = CP[det][cry].getY();
      zz = CP[det][cry].getZ();
      rr = sqrt(xx * xx + yy * yy + zz * zz);
      cc = zz / rr;
      printf(" %2d  %2d  %3d %8.3f %8.3f %8.3f %8.3f %8.3f\n", det + 1, cry,
             RingMap(det + 1, cry), xx, yy, zz, rr, cc);
    }
}
//=====================================================================================
void Results::GetCsIWallPositions() {
  Double_t xx, yy, zz;
  for (Int_t det = 0; det < NCsI; det++) {
    xx = theDetector->GetCsIWall()->GetXPos(det + 1);
    yy = theDetector->GetCsIWall()->GetYPos(det + 1);
    zz = theDetector->GetCsIWall()->GetZPos();
    PP[det].setX(xx);
    PP[det].setY(yy);
    PP[det].setZ(zz);
  }
}
//=====================================================================================
void Results::GetCsIBallPositions() {
  Double_t xx, yy, zz;
  for (Int_t det = 0; det < NCsISph; det++) {
    xx = theDetector->GetCsIBall()->GetXPos(det + 1);
    yy = theDetector->GetCsIBall()->GetYPos(det + 1);
    zz = theDetector->GetCsIBall()->GetZPos();
    PP[det].setX(xx);
    PP[det].setY(yy);
    PP[det].setZ(zz);
  }
}
//=====================================================================================
void Results::ReportCsIWallPositions() {
  Double_t xx, yy, zz, rr, cc;
  printf("pin      x        y        z        r       cos \n");
  for (Int_t det = 0; det < NCsI; det++) {
    xx = PP[det].getX();
    yy = PP[det].getY();
    zz = PP[det].getZ();
    rr = sqrt(xx * xx + yy * yy + zz * zz);
    cc = zz / rr;
    printf(" %2d  %8.3f %8.3f %8.3f %8.3f %8.3f\n", det + 1, xx, yy, zz, rr,
           cc);
  }
}
//=====================================================================================
void Results::GroupCosDist() {

  Int_t pin, det, cry;
  Double_t cos;

  c = (TCanvas *)gROOT->FindObject("StdCanv");
  if (c != NULL)
    c->Close();

  c = new TCanvas("StdCanv", "StdCanv", 700, 500);
  c->Clear();
  c->cd();

  h = (TH1D *)gROOT->FindObject("GroupCos");
  if (h == NULL)
    h = new TH1D("GroupCos", "GroupCos", 1000, 0.0, 1.0);
  h->Reset();

  for (pin = 0; pin < NCsI; pin++)
    for (det = 0; det < GN; det++)
      for (cry = 0; cry < GS; cry++) {
        cos = PP[pin].dot(CP[det][cry]) / PP[pin].mag() / CP[det][cry].mag();
        h->Fill(cos);
      }

  h->GetXaxis()->SetTitle("cos(theta)");
  h->GetYaxis()->SetTitle("Counts");
  h->SetLineColor(kRed);
  h->Draw();
}

//=====================================================================================
G4double Results::CalculatePath(G4ThreeVector iPos, G4ThreeVector Pos) {
  G4double xd, yd, zd;
  G4double dist;

  xd = Pos.getX() - iPos.getX();
  yd = Pos.getY() - iPos.getY();
  zd = Pos.getZ() - iPos.getZ();

  dist = sqrt(xd * xd + yd * yd + zd * zd);

  return dist;
}
//=====================================================================================
G4double Results::CalculateBirksLawStep(G4int id, G4double dE, G4double dEdx) {
  // G4double S=1.0; // absolute scinitallation yeild for CsI (can take from
  // arXiv 1502:03800v1 for low E heavy ions).
  G4double dL; // differential light yield

  //   printf("Birks constant for position %2d is %6.3f um/MeV %6.3f
  //   (mg/cm^2)/MeV\n",id,kB[id-1],kBm[id-1]);
  // getc(stdin);

  // dL = S*dE/(1+kB[id-1]*dEdx); // for dE in MeV/um
  dL = S[id - 1] * dE / (1 + kBm[id - 1] * dEdx); // for dE in MeV/(mg/cm^2)

  return dL;
}
