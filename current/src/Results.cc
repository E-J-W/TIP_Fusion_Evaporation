#include "Results.hh"

Results::Results(Projectile* proj,DetectorConstruction* det):theProjectile(proj),theDetector(det)
//Results::Results(Projectile* proj,Recoil* rec,DetectorConstruction* det):theProjectile(proj),theRecoil(rec),theDetector(det)
{
  h=NULL;
  g=NULL;  
  c=NULL;
  TreeCreate();  
  IonFill=0;
  soh=sizeof(partHit);
  sogh=sizeof(GHit);
  soi=sizeof(gun);
  sos=sizeof(stat);
  memset(CP,0,sizeof(CP));

  // HPGe FWHM response parameters
  F=1.25;
  G=2.00;
  H=0.00;

  // Energy calibration parameters
  A=0.0;
  B=1.0;
  C=0;

}

Results::~Results()
{
Gtree->Delete();
Iontree->Delete();
CsItree->Delete();
}
//---------------------------------------------------------
void Results::SetupRun(Int_t numP, Int_t numN, Int_t numA)
{
  //Get A, Z of the projectile and recoiling residual (needed for particle identification when saving data to tree)
  Ap=theProjectile->getA();
  Zp=theProjectile->getZ();
  Ar=theProjectile->getA() + theDetector->GetTarget()->getTargetMass() - numP - numN - numA*4;
  Zr=theProjectile->getZ() + theDetector->GetTarget()->getTargetCharge() - numP - numA*2;
  //G4cout << "Results: target A: " << theDetector->GetTarget()->getTargetMass() << ", target Z: " << theDetector->GetTarget()->getTargetCharge() << G4endl; 
  //G4cout << "Results: projectile A: " << Ap << ", projectile Z: " << Zp << ", recoil A: " << Ar << ", recoil Z: " << Zr << G4endl; 
  
  CsIDensity=theDetector->GetCsIArray()->GetCsIDensity();
  // printf("CsI density is     %9.3f g/cm^3\n",CsIDensity);
  // getc(stdin);

  // setup Birks constant for LY calculation
  for(int i=0 ; i<NCsI ; i++)
    {
      kB[i]=theDetector->GetCsIArray()->GetBirksConstant(i); // Birks constant in um/MeV
      kBm[i]=kB[i]*CsIDensity/10;                            // Birks constant in (mg/cm^2)/MeV
      S[i]=theDetector->GetCsIArray()->GetLYScaling(i);
      //printf("Birks constant for position %2d set to %6.3f um/MeV %6.3f (mg/cm^2)/MeV\n",i+1,kB[i],kBm[i]);
    }
    
  //get HPGe and CsI crystal positions
  GetCsIPositions();
  for(int i=0; i<GN; i++)
    for(int j=0; j<GS; j++)
      {
	      CP[i][j]=theDetector->GetDetectorCrystalPosition(i,j);
	      //printf("HPGe position %d crystal %d x %f y %f z %f\n",i+1,j,CP[i][j].getX(),CP[i][j].getY(),CP[i][j].getZ());
      }
    
  //getc(stdin);
}
//---------------------------------------------------------
void Results::TreeCreate()
{
  if((Gtree==NULL)&&(Iontree==NULL)&&(CsItree==NULL))
    {
      Gtree= new TTree("Gtree","Gtree");
      Gtree->Branch("Gfold",&GHit.Gfold,"Gfold/I");
      Gtree->Branch("GId",GHit.GId,"GId[Gfold]/I");
      Gtree->Branch("GSeg",GHit.GSeg,"GSeg[Gfold]/I");
      Gtree->Branch("GRing",GHit.GRing,"GRing[Gfold]/I");
      Gtree->Branch("Gx",GHit.Gx,"Gx[Gfold]/D");
      Gtree->Branch("Gy",GHit.Gy,"Gy[Gfold]/D");
      Gtree->Branch("Gz",GHit.Gz,"Gz[Gfold]/D");
      Gtree->Branch("GE",GHit.GE,"GE[Gfold]/D");
      Gtree->Branch("GW",GHit.GW,"GW[Gfold]/D");
      Gtree->Branch("GfoldAddBack",&GHit.GfoldAB,"GfoldAB/I");
      Gtree->Branch("GIdAddBack",GHit.GIdAB,"GIdAB[GfoldAB]/I");
      Gtree->Branch("GSegAddBack",GHit.GSegAB,"GSegAB[GfoldAB]/I");
      Gtree->Branch("GRingAddBack",GHit.GRingAB,"GRingAB[GfoldAB]/I");
      Gtree->Branch("GxAddBack",GHit.GxAB,"GxAB[GfoldAB]/D");
      Gtree->Branch("GyAddBack",GHit.GyAB,"GyAB[GfoldAB]/D");
      Gtree->Branch("GzAddBack",GHit.GzAB,"GzAB[GfoldAB]/D");
      Gtree->Branch("GEAddBack",GHit.GEAB,"GEAB[GfoldAB]/D");
      
      //tree->Branch("stat",&stat,"evNb/I:Ap/I:Zp/I:Ar/I:Zr/I");

      Iontree= new TTree("Iontree","Iontree");
      Iontree->Branch("projGun",&gun,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //projectile when shot from the particle gun
      Iontree->Branch("projTargetIn",&pTIn,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //projectile upon entering the target
      Iontree->Branch("projReactionIn",&pRIn,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //projectile at the reaction point
      Iontree->Branch("resReactionOut",&rROut,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //residual at the reaction point
      Iontree->Branch("resBackingIn",&rBIn,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //residual upon leaving the target/entering the backing
      Iontree->Branch("resBackingOut",&rBOut,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //residual upon leaving the backing (if it makes it that far)
      Iontree->Branch("resDec",&rDec,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:theta/D:phi/D:w/D"); //residual upon decaying via gamma emission

      //tree->Branch("partCsIHit",&partHit,"x/D:y/D:z/D:px/D:py/D:pz/D:E/D:b/D:w/D:Id/I:r/I:path/D:dE/D:dEdx/D:dLdx/D:LY/D"); //particle hit in CsI
      
      CsItree= new TTree("CsItree","CsItree");
      CsItree->Branch("CsIfold",&partHit.CsIfold,"CsIfold/I");
      CsItree->Branch("x",partHit.x,"x[CsIfold]/D");
      CsItree->Branch("y",partHit.y,"y[CsIfold]/D");
      CsItree->Branch("z",partHit.z,"z[CsIfold]/D");
      CsItree->Branch("px",partHit.px,"px[CsIfold]/D");
      CsItree->Branch("py",partHit.py,"py[CsIfold]/D");
      CsItree->Branch("pz",partHit.pz,"pz[CsIfold]/D");
      CsItree->Branch("E",partHit.E,"E[CsIfold]/D");
      CsItree->Branch("b",partHit.b,"b[CsIfold]/D");
      CsItree->Branch("w",partHit.w,"w[CsIfold]/D");
      CsItree->Branch("Id",partHit.Id,"Id[CsIfold]/I");
      CsItree->Branch("r",partHit.r,"r[CsIfold]/I");
      CsItree->Branch("path",partHit.path,"path[CsIfold]/D");
      CsItree->Branch("dE",partHit.Id,"dE[CsIfold]/D");
      CsItree->Branch("dEdx",partHit.dEdx,"dEdx[CsIfold]/D");
      CsItree->Branch("dLdx",partHit.dLdx,"dLdx[CsIfold]/D");
      CsItree->Branch("LY",partHit.LY,"LY[CsIfold]/D");
    }
  else
    {
      printf("Trees not deleted, could not create new trees!\n");
      getc(stdin);
    }

}
//---------------------------------------------------------
void Results::TreeClear()
{
  Gtree->Delete("all");
  Iontree->Delete("all");
  CsItree->Delete("all");
  Gtree=NULL;
  Iontree=NULL;
  CsItree=NULL;
  TreeCreate();
}
//---------------------------------------------------------
void Results::TreeReport()
{
  Gtree->Print();
  Iontree->Print();
  CsItree->Print();
}
//---------------------------------------------------------
void Results::TreeSave(G4String name)
{
  TDirectory *dir=new TDirectory();
  dir=gDirectory;
  TFile f(name,"recreate");
  f.cd();
  //tree->AutoSave();
  //tree->SetDirectory(dir);
  Gtree->Write();
  Iontree->Write();
  CsItree->Write();
  f.Close();
  dir->cd();
  G4cout<<"Trees of simulated parameters saved in file "<<name<<G4endl;
  
}
//---------------------------------------------------------
void Results::TreeRead(G4String)
{
 
}

//---------------------------------------------------------
void Results::TreeAdd(G4String )
{

 

}
//---------------------------------------------------------
void Results::FillTree(G4int evtNb, TrackerIonHitsCollection* IonCollection,TrackerCsIHitsCollection* CsICollection,G4double gw[GN][GS],G4double ge[GN][GS],G4ThreeVector gp[GN][GS])
{

 G4int Nt=IonCollection->entries();

 memset(&stat,0,sos);
 memset(&gun,0,soi);
 memset(&pTIn,0,soi);
 memset(&pRIn,0,soi);
 memset(&rROut,0,soi);
 memset(&rBIn,0,soi);
 memset(&rBOut,0,soi);
 memset(&partHit,0,soh);
 //memset(&GHit,0,sogh);

 if(Nt>0) 
   {
     //G4cout << "Saving ion collection data..." << G4endl; 
     stat.evNb=evtNb;
     stat.Ap=Ap;
     stat.Zp=Zp;
 
     for(Int_t i=0;i<Nt;i++)
       switch((*IonCollection)[i]->GetFlag())
	 {
	 case GUN_FLAG:
	   if((*IonCollection)[i]->GetA()==Ap)
	     if((*IonCollection)[i]->GetZ()==Zp)
	       {
		 gun.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 gun.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 gun.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 gun.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 gun.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 gun.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 gun.b=(*IonCollection)[i]->GetBeta();
		 gun.E=(*IonCollection)[i]->GetKE()/MeV;
		 gun.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 gun.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y
		 gun.w=(*IonCollection)[i]->GetWeight();

	       }
	   break;

	 case TARGET_IN_FLAG:
	   if((*IonCollection)[i]->GetA()==Ap)
	     if((*IonCollection)[i]->GetZ()==Zp)
	       {
		 pTIn.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 pTIn.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 pTIn.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 pTIn.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 pTIn.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 pTIn.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 pTIn.b=(*IonCollection)[i]->GetBeta();
		 pTIn.E=(*IonCollection)[i]->GetKE()/MeV;
		 pTIn.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 pTIn.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y	 
		 pTIn.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;
	 case BACKING_IN_FLAG:
	   if((*IonCollection)[i]->GetA()==Ar)
	     if((*IonCollection)[i]->GetZ()==Zr)
	       {
		 rBIn.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 rBIn.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 rBIn.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 rBIn.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 rBIn.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 rBIn.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 rBIn.b=(*IonCollection)[i]->GetBeta();
		 rBIn.E=(*IonCollection)[i]->GetKE()/MeV;
		 rBIn.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 rBIn.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y 
		 rBIn.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;
	 case BACKING_OUT_FLAG:
	   if((*IonCollection)[i]->GetA()==Ar)
	     if((*IonCollection)[i]->GetZ()==Zr)
	       {
		 rBOut.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 rBOut.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 rBOut.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 rBOut.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 rBOut.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 rBOut.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 rBOut.b=(*IonCollection)[i]->GetBeta();
		 rBOut.E=(*IonCollection)[i]->GetKE()/MeV;
		 rBOut.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 rBOut.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y
		 rBOut.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;

	 default:
	   break;
	 }

    for(Int_t i=0;i<Nt;i++)
       switch((*IonCollection)[i]->GetPFlag())
	 {
	 case REACTION_IN_FLAG:
	   if((*IonCollection)[i]->GetA()==Ap)
	     if((*IonCollection)[i]->GetZ()==Zp)
	       {
		 pRIn.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 pRIn.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 pRIn.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 pRIn.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 pRIn.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 pRIn.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 pRIn.b=(*IonCollection)[i]->GetBeta();
		 pRIn.E=(*IonCollection)[i]->GetKE()/MeV;
		 pRIn.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 pRIn.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y
		 pRIn.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;
	 case REACTION_OUT_FLAG:
	   if((*IonCollection)[i]->GetA()==Ar)
	     if((*IonCollection)[i]->GetZ()==Zr)
	       {
		 rROut.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 rROut.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 rROut.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 rROut.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 rROut.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 rROut.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 rROut.b=(*IonCollection)[i]->GetBeta();
		 rROut.E=(*IonCollection)[i]->GetKE()/MeV;
		 rROut.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 rROut.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y
		 rROut.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;
	 case DECAY_FLAG:	   
	   if((*IonCollection)[i]->GetA()==Ar)
	     if((*IonCollection)[i]->GetZ()==Zr)
	       {
		 rDec.x=(*IonCollection)[i]->GetPos().getX()/mm;
		 rDec.y=(*IonCollection)[i]->GetPos().getY()/mm;
		 rDec.z=(*IonCollection)[i]->GetPos().getZ()/mm;
		 rDec.px=(*IonCollection)[i]->GetMom().getX()/MeV;
		 rDec.py=(*IonCollection)[i]->GetMom().getY()/MeV;
		 rDec.pz=(*IonCollection)[i]->GetMom().getZ()/MeV;
		 rDec.b=(*IonCollection)[i]->GetBeta();
		 rDec.E=(*IonCollection)[i]->GetKE()/MeV;
		 rDec.theta=acos(((*IonCollection)[i]->GetMom().getZ())/((*IonCollection)[i]->GetMom().mag()))/degree;//angle between (0,0,1) and momentum vector
		 rDec.phi=acos((*IonCollection)[i]->GetMom().getX()/sqrt((*IonCollection)[i]->GetMom().getX()*(*IonCollection)[i]->GetMom().getX() + (*IonCollection)[i]->GetMom().getY()*(*IonCollection)[i]->GetMom().getY()))/degree;//angle between (1,0,0) and momentum vector in x and y
		 rDec.w=(*IonCollection)[i]->GetWeight();
	       }
	   break;

	 default:
	   break;
	 }
   }//end of ion collection entry saving

  partHit.CsIfold=1;
  memset(&partHit.x,0,sizeof(partHit.x));
  memset(&partHit.y,0,sizeof(partHit.y));
  memset(&partHit.z,0,sizeof(partHit.z));
  memset(&partHit.px,0,sizeof(partHit.px));
  memset(&partHit.pz,0,sizeof(partHit.pz));
  memset(&partHit.b,0,sizeof(partHit.b));
  memset(&partHit.E,0,sizeof(partHit.E));
  memset(&partHit.dE,0,sizeof(partHit.dE));
  memset(&partHit.w,0,sizeof(partHit.w));
  memset(&partHit.Id,0,sizeof(partHit.Id));
  memset(&partHit.r,0,sizeof(partHit.r));
  memset(&partHit.path,0,sizeof(partHit.path));
  memset(&partHit.dEdx,0,sizeof(partHit.dEdx));
  memset(&partHit.dLdx,0,sizeof(partHit.dLdx));
  memset(&partHit.LY,0,sizeof(partHit.LY));
 
 G4int Np=CsICollection->entries();
 if(Np>0)
   {
     for(Int_t i=0;i<Np;i++)
       {
         if((CsITrigId==0)||(CsITrigId==(*CsICollection)[i]->GetId()))//save only data for the detectors we want to see
           {
             if(partHit.E[0]==0.)
               {
                 partHit.x[0]=(*CsICollection)[i]->GetPos().getX()/mm;
                 partHit.y[0]=(*CsICollection)[i]->GetPos().getY()/mm;
                 partHit.z[0]=(*CsICollection)[i]->GetPos().getZ()/mm;
                 partHit.px[0]=(*CsICollection)[i]->GetMom().getX()/MeV;
                 partHit.py[0]=(*CsICollection)[i]->GetMom().getY()/MeV;
                 partHit.pz[0]=(*CsICollection)[i]->GetMom().getZ()/MeV;
                 partHit.b[0]=(*CsICollection)[i]->GetBeta();
                 partHit.E[0]=(*CsICollection)[i]->GetKE()/MeV;
                 partHit.dE[0]=(*CsICollection)[i]->GetKE()/MeV;		 
                 partHit.w[0]=(*CsICollection)[i]->GetWeight();
                 partHit.Id[0]=(*CsICollection)[i]->GetId();
                 partHit.r[0]=(*CsICollection)[i]->GetRingId();
                 // partHit.path=(*CsICollection)[i]->GetPathLength()/um; // in microns
                 // partHit.dEdx=((*CsICollection)[i]->GetKE()/MeV)/((*CsICollection)[i]->GetPathLength()/um); 
                 partHit.path[0]=((*CsICollection)[i]->GetPathLength()/cm)*CsIDensity*1000; // in mg/cm^2
                 partHit.dEdx[0]=((*CsICollection)[i]->GetKE()/MeV)/(((*CsICollection)[i]->GetPathLength()/cm)*CsIDensity*1000);
                 partHit.dLdx[0]=CalculateBirksLawStep(partHit.Id[0],partHit.dE[0],partHit.dEdx[0]); // make sure to check units of kB!
                 partHit.LY[0]=partHit.dLdx[0];
                 //G4cout<<"CsI ID in tree: "<<partHit.Id<<G4endl;
                 //printf("particle: dE %9.3f path %9.3f  dE/dx %9.3f  dL %9.3f  LY %9.3f\n",partHit.E,partHit.path,partHit.dEdx,partHit.dLdx,partHit.LY);
               }
             else
               {
                 if((*CsICollection)[i]->GetId()==partHit.Id[0])
                   {
                     partHit.E[0]+=(*CsICollection)[i]->GetKE()/MeV;	
                     partHit.dE[0]=(*CsICollection)[i]->GetKE()/MeV;		 
                     // partHit.path+=(*CsICollection)[i]->GetPathLength()/um; // in microns
                     // partHit.dEdx=((*CsICollection)[i]->GetKE()/MeV)/((*CsICollection)[i]->GetPathLength()/um);
                     partHit.path[0]+=((*CsICollection)[i]->GetPathLength()/cm)*CsIDensity*1000; // in mg/cm^2
                     partHit.dEdx[0]=((*CsICollection)[i]->GetKE()/MeV)/(((*CsICollection)[i]->GetPathLength()/cm)*CsIDensity*1000);
                     partHit.dLdx[0]=CalculateBirksLawStep(partHit.Id[0],partHit.dE[0],partHit.dEdx[0]); // make sure to check units of kB!
                     partHit.LY[0]+=partHit.dLdx[0];
                     //printf("particle: dE %9.3f path %9.3f  dE/dx %9.3f  dL %9.3f  LY %9.3f\n",partHit.dE,partHit.path,partHit.dEdx,partHit.dLdx,partHit.LY);
                   }
               }
           }
         //getc(stdin);
       }
   }//end of CsI collection entry saving


  G4int i,j;
  GHit.Gfold=0;
  GHit.GfoldAB=0;
  memset(maxGe,0,sizeof(maxGe));
  memset(&GHit.GId,0,sizeof(GHit.GId));
  memset(&GHit.GSeg,0,sizeof(GHit.GSeg));
  memset(&GHit.GE,0,sizeof(GHit.GE));
  memset(&GHit.GIdAB,0,sizeof(GHit.GIdAB));
  memset(&GHit.GSegAB,0,sizeof(GHit.GSegAB));
  memset(&GHit.GRingAB,0,sizeof(GHit.GRingAB));
  memset(&GHit.GEAB,0,sizeof(GHit.GEAB));
  memset(&GHit.GxAB,0,sizeof(GHit.GxAB));
  memset(&GHit.GyAB,0,sizeof(GHit.GyAB));
  memset(&GHit.GzAB,0,sizeof(GHit.GzAB));
  //non-addback
  for(i=0;i<GN;i++)//number of positions
    for(j=0;j<GS;j++)//number of crystals
	    if(gw[i][j]>0)
  	    {
          //G4cout<<" i "<<i<<" j "<<j<<" w "<<gw[i][j]<<G4endl;
	        GHit.GId[GHit.Gfold]=i+1;
	        GHit.GSeg[GHit.Gfold]=j;
	        GHit.GRing[GHit.Gfold]=RingMap(GHit.GId[GHit.Gfold],GHit.GSeg[GHit.Gfold]);//get the ring in which the hit occured
	        GHit.Gx[GHit.Gfold]=gp[i][j].getX();
  	      GHit.Gy[GHit.Gfold]=gp[i][j].getY();
  	      GHit.Gz[GHit.Gfold]=gp[i][j].getZ();
  	      GHit.GE[GHit.Gfold]=ge[i][j];
  	      GHit.GW[GHit.Gfold]=gw[i][j];
          GHit.Gfold++;
  	    } 

  //addback
  for(i=0;i<GN;i++)//number of positions
    for(j=0;j<GS;j++)//number of crystals
  	  if(ge[i][j]>maxGe[i])
  	    {
  	      //assign position, ring, etc. of the addback hit
  	      GHit.GIdAB[GHit.GfoldAB]=i+1;
          GHit.GSegAB[GHit.GfoldAB]=j;
          GHit.GRingAB[GHit.GfoldAB]=RingMap(GHit.GIdAB[GHit.GfoldAB],GHit.GSegAB[GHit.GfoldAB]);//get the ring in which the hit occured
          GHit.GxAB[GHit.GfoldAB]=gp[i][j].getX();
  	      GHit.GyAB[GHit.GfoldAB]=gp[i][j].getY();
  	      GHit.GzAB[GHit.GfoldAB]=gp[i][j].getZ();
          if(maxGe[i]==0.)//no energy deposit in this detector prior to this hit
  	        GHit.GfoldAB++;
  	      maxGe[i]=ge[i][j];
  	    }
 
  //addback energy: sum deposits over all crystals in the detector with the highest energy deposit
  for(i=0;i<GHit.GfoldAB;i++)//number of addback events
    for(j=0;j<GS;j++)//number of crystals
      if(ge[(GHit.GIdAB[i])-1][j]>0)
        GHit.GEAB[i]+=ge[(GHit.GIdAB[i])-1][j];
        
  //compute an approxiate Doppler shift based on the specific detectors where gamma and evaporated particles were seen
  //...
    
  Gtree->Fill();
  Iontree->Fill();
  CsItree->Fill();
  IonFill++;
}
//=====================================================================================
G4int Results::RingMap(G4int id,G4int seg)
 {
   G4int r,s;
   s=-1;
   if(seg==0) s=0;
   if(seg==1) s=1;
   if(seg==2) s=1;
   if(seg==3) s=0;
   if(s<0) return -1;

   r=-1;
   switch (id)
     {
     case  1:r=1;break;
     case  2:r=1;break;
     case  3:r=1;break;
     case  4:r=1;break;
     case  5:r=3;break;
     case  6:r=3;break;
     case  7:r=3;break;
     case  8:r=3;break;
     case  9:r=3;break;
     case 10:r=3;break;
     case 11:r=3;break;
     case 12:r=3;break;
     case 13:r=5;break;
     case 14:r=5;break;
     case 15:r=5;break;
     case 16:r=5;break;
     default:break;
     }
   if(r<0) return -1;

   return r+s;
 }
//=====================================================================================
void Results::ReportCrystalPositions()
 {
   Double_t xx,yy,zz,rr,cc;
   printf("det cry ring     x        y        z        r       cos\n");
   for(Int_t det=0;det<GN;det++)
     for(Int_t cry=0;cry<GS;cry++)
       {
	 xx=CP[det][cry].getX();
	 yy=CP[det][cry].getY();
	 zz=CP[det][cry].getZ();
	 rr=sqrt(xx*xx+yy*yy+zz*zz);
	 cc=zz/rr;
	 printf(" %2d  %2d  %3d %8.3f %8.3f %8.3f %8.3f %8.3f\n",det+1,cry,RingMap(det+1,cry),xx,yy,zz,rr,cc);
       }
       
 } 
//=====================================================================================
void Results::GetCsIPositions()
{
  Double_t xx,yy,zz;
  for(Int_t det=0;det<NCsI;det++)
    {
	    xx=theDetector->GetCsIArray()->GetXPos(det+1);
	    yy=theDetector->GetCsIArray()->GetYPos(det+1);
	    zz=theDetector->GetCsIArray()->GetZPos();
	    PP[det].setX(xx);
	    PP[det].setY(yy);
	    PP[det].setZ(zz);
    }
}
//=====================================================================================
void Results::ReportCsIPositions()
 {
   Double_t xx,yy,zz,rr,cc;
   printf("pin      x        y        z        r       cos \n");
   for(Int_t det=0;det<NCsI;det++)
       {
	 xx=PP[det].getX();
	 yy=PP[det].getY();
	 zz=PP[det].getZ();
	 rr=sqrt(xx*xx+yy*yy+zz*zz);
	 cc=zz/rr;
	 printf(" %2d  %8.3f %8.3f %8.3f %8.3f %8.3f\n",det+1,xx,yy,zz,rr,cc);
       }
       
 }
//=====================================================================================
void Results::GroupCosDist()
 {
 
   Int_t pin,det,cry;
   Double_t cos;

   c=(TCanvas *)gROOT->FindObject("StdCanv");
   if(c!=NULL)
     c->Close();

   c=new TCanvas("StdCanv","StdCanv",700,500);
   c->Clear();
   c->cd();
 
   h=(TH1D *)gROOT->FindObject("GroupCos");
   if(h==NULL)
     h=new TH1D("GroupCos","GroupCos",1000,0.0,1.0);
   h->Reset();
 
   for(pin=0;pin<NCsI;pin++)
     for(det=0;det<GN;det++)
       for(cry=0;cry<GS;cry++)
	 {
	   cos=PP[pin].dot(CP[det][cry])/PP[pin].mag()/CP[det][cry].mag();
	   h->Fill(cos);
	 }
  
   h->GetXaxis()->SetTitle("cos(theta)");
   h->GetYaxis()->SetTitle("Counts");
   h->SetLineColor(kRed);
   h->Draw();

 }
//=====================================================================================
G4double Results::FWHM_response(G4double e_in)
{
  G4double e_out,fwhm,sigma,e,channel;
  
  e=e_in/1000.;
  // printf("e %f ein %f\n",e,e_in);
  fwhm=sqrt(F+G*e+H*e*e);
  channel=A+B*e_in+C*e_in*e_in;
  sigma=fwhm/2.35482;
  //  printf("channel %f sigma %f\n",channel,sigma);
  if(sigma>0)
    e_out=CLHEP::RandGauss::shoot(channel,sigma);
  else
    e_out=channel;

  //  printf("e_out  %f\n",e_out);
  //  getc(stdin);
  return e_out;
}

//=====================================================================================
G4double Results::CalculatePath(G4ThreeVector iPos, G4ThreeVector Pos)
{
  G4double xd,yd,zd;
  G4double dist;

  xd=Pos.getX()-iPos.getX();
  yd=Pos.getY()-iPos.getY();
  zd=Pos.getZ()-iPos.getZ();

  dist=sqrt(xd*xd+yd*yd+zd*zd);

  return dist;
}
//=====================================================================================
G4double Results::CalculateBirksLawStep(G4int id,G4double dE,G4double dEdx)
{
  //G4double S=1.0; // absolute scinitallation yeild for CsI (can take from arXiv 1502:03800v1 for low E heavy ions).
  G4double dL;    // differential light yield

  //   printf("Birks constant for position %2d is %6.3f um/MeV %6.3f (mg/cm^2)/MeV\n",id,kB[id-1],kBm[id-1]);
  // getc(stdin);
  
  // dL = S*dE/(1+kB[id-1]*dEdx); // for dE in MeV/um
  dL = S[id-1]*dE/(1+kBm[id-1]*dEdx); // for dE in MeV/(mg/cm^2)

  return dL;
}
