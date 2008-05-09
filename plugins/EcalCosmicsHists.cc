// -*- C++ -*-
//
// Package:   EcalCosmicsHists 
// Class:     EcalCosmicsHists  
// 
/**\class EcalCosmicsHists EcalCosmicsHists.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/

 
#include "CaloOnlineTools/EcalTools/plugins/EcalCosmicsHists.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include <vector>
 
using namespace cms;
using namespace edm;
using namespace std;

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalCosmicsHists::EcalCosmicsHists(const edm::ParameterSet& iConfig) :
  ecalRecHitCollection_ (iConfig.getParameter<edm::InputTag>("ecalRecHitCollection")),
  barrelClusterCollection_ (iConfig.getParameter<edm::InputTag>("barrelClusterCollection")),
  endcapClusterCollection_ (iConfig.getParameter<edm::InputTag>("endcapClusterCollection")),
  l1GTReadoutRecTag_ (iConfig.getUntrackedParameter<std::string>("L1GlobalReadoutRecord","gtDigis")),
  runNum_(-1),
  histRangeMax_ (iConfig.getUntrackedParameter<double>("histogramMaxRange",1.8)),
  histRangeMin_ (iConfig.getUntrackedParameter<double>("histogramMinRange",0.0)),
  minSeedAmp_ (iConfig.getUntrackedParameter<double>("MinSeedAmp",5.0)),
  minTimingAmp_ (iConfig.getUntrackedParameter<double>("MinTimingAmp",.100)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("ecalCosmicHists"))),
  minCosmicE1_ (iConfig.getUntrackedParameter<double>("MinCosmicE1", 15.0)),
  minCosmicE2_ (iConfig.getUntrackedParameter<double>("MinCosmicE2", 5.0))
{
  naiveEvtNum_ = 0;
  cosmicCounter_ = 0;

  string title1 = "Seed Energy for All Feds; Seed Energy (GeV)";
  string name1 = "SeedEnergyAllFEDs";
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;
  allFedsHist_ = new TH1F(name1.c_str(),title1.c_str(),numBins,histRangeMin_,histRangeMax_);
  
  fedMap_ = new EcalFedMap();
 
}


EcalCosmicsHists::~EcalCosmicsHists()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalCosmicsHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  int ievt = iEvent.id().event();
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);
  
  edm::Handle<reco::BasicClusterCollection> bccHandle;  
  edm::Handle<reco::BasicClusterCollection> eccHandle;  
  
  naiveEvtNum_++;

  //  cout << "Timestamp: " << iEvent.id().run() << " " << iEvent.id().event() << " " << iEvent.time().value() << endl;

  iEvent.getByLabel(barrelClusterCollection_, bccHandle);
  if (!(bccHandle.isValid())) 
    {
	  LogWarning("EcalCosmicsHists") << barrelClusterCollection_ << " not available";
      return;
    }
  LogDebug("EcalCosmicsHists") << "event " << ievt;
  
  iEvent.getByLabel(endcapClusterCollection_, eccHandle);
  if (!(eccHandle.isValid())) 
    {
	  LogWarning("EcalCosmicsHists") << endcapClusterCollection_ << " not available";
      //return;
    }
  
  Handle<EcalRecHitCollection> hits;
  iEvent.getByLabel(ecalRecHitCollection_, hits);
  if (!(eccHandle.isValid())) 
    {
	  LogWarning("EcalCosmicsHists") << ecalRecHitCollection_ << " not available";
      //return;
    }

  // get the GMTReadoutCollection
  Handle<L1MuGMTReadoutCollection> gmtrc_handle; 
  iEvent.getByLabel(l1GTReadoutRecTag_,gmtrc_handle);
  L1MuGMTReadoutCollection const* gmtrc = gmtrc_handle.product();
  if (!(gmtrc_handle.isValid())) 
    {
	  LogWarning("EcalCosmicsHists") << "l1MuGMTReadoutCollection" << " not available";
      //return;
    }
  
  // get hold of L1GlobalReadoutRecord
  Handle<L1GlobalTriggerReadoutRecord> L1GTRR;
  iEvent.getByLabel(l1GTReadoutRecTag_,L1GTRR);
  bool isEcalL1 = false;
  if (!(L1GTRR.isValid())) 
    {
	  LogWarning("EcalCosmicsHists") << l1GTReadoutRecTag_ << " not available";
      //return;
    }
  else
    {
      const unsigned int n(L1GTRR->decisionWord().size());
      l1Names_.resize(n);
      l1Accepts_.resize(n);
      for (unsigned int i=0; i!=n; ++i) {
        l1Accepts_[i]=0;
        l1Names_[i]="NameNotAvailable";
      }
      for (unsigned int i=0; i!=n; ++i) {
        if (L1GTRR->decisionWord()[i])
          {
            l1Accepts_[i]++;
            //cout << "L1A bit: " << i << endl;
          }
      }
    }
  if(l1Accepts_[14] || l1Accepts_[15] || l1Accepts_[16] || l1Accepts_[17]
      || l1Accepts_[18] || l1Accepts_[19] || l1Accepts_[20])
    isEcalL1 = true;
  if(l1Accepts_[73] || l1Accepts_[74] || l1Accepts_[75] || l1Accepts_[76]
      || l1Accepts_[77] || l1Accepts_[78])
    isEcalL1 = true;
      
  bool isRPCL1 = false;
  bool isDTL1 = false;
  bool isCSCL1 = false;
  bool isHCALL1 = false;
      
  std::vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  std::vector<L1MuGMTReadoutRecord>::const_iterator igmtrr;
      
  for(igmtrr=gmt_records.begin(); igmtrr!=gmt_records.end(); igmtrr++) {

    std::vector<L1MuRegionalCand>::const_iterator iter1;
    std::vector<L1MuRegionalCand> rmc;

    //DT triggers
    int idt = 0;
    rmc = igmtrr->getDTBXCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        idt++;
      }
    }

    //if(idt>0) std::cout << "Found " << idt << " valid DT candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && idt>0) isDTL1 = true;

    //RPC triggers
    int irpcb = 0;
    rmc = igmtrr->getBrlRPCCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        irpcb++;
      }
    }

    //if(irpcb>0) std::cout << "Found " << irpcb << " valid RPC candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && irpcb>0) isRPCL1 = true;

    //CSC Triggers
    int icsc = 0;
    rmc = igmtrr->getCSCCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        icsc++;
      }
    }

    //if(icsc>0) std::cout << "Found " << icsc << " valid CSC candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && icsc>0) isCSCL1 = true;
  }
  

  L1GlobalTriggerReadoutRecord const* gtrr = L1GTRR.product();

  for(int ibx=-1; ibx<=1; ibx++) {
    bool hcal_top = false;
    bool hcal_bot = false;
    const L1GtPsbWord psb = gtrr->gtPsbWord(0xbb0d,ibx);
    std::vector<int> valid_phi;
    if((psb.aData(4)&0x3f) >= 1) {valid_phi.push_back( (psb.aData(4)>>10)&0x1f ); }
    if((psb.bData(4)&0x3f) >= 1) {valid_phi.push_back( (psb.bData(4)>>10)&0x1f ); }
    if((psb.aData(5)&0x3f) >= 1) {valid_phi.push_back( (psb.aData(5)>>10)&0x1f ); }
    if((psb.bData(5)&0x3f) >= 1) {valid_phi.push_back( (psb.bData(5)>>10)&0x1f ); }
    std::vector<int>::const_iterator iphi;
    for(iphi=valid_phi.begin(); iphi!=valid_phi.end(); iphi++) {
      //std::cout << "Found HCAL mip with phi=" << *iphi << " in bx wrt. L1A = " << ibx << std::endl;
      if(*iphi<9) hcal_top=true;
      if(*iphi>8) hcal_bot=true;
    }
    if(ibx==0 && hcal_top && hcal_bot) isHCALL1=true;
  }

  std::cout << "**** Trigger Source ****" << std::endl;
  if(isDTL1) std::cout << "DT" << std::endl;
  if(isRPCL1) std::cout << "RPC" << std::endl;
  if(isCSCL1) cout << "CSC" << endl;
  if(isHCALL1) std::cout << "HCAL" << std::endl;
  if(isEcalL1) std::cout << "ECAL" << std::endl;
  std::cout << "************************" << std::endl;
  
  if(runNum_==-1)
    {
      runNum_ = iEvent.id().run();
    }      

  int  numberOfCosmics = 0;
  //int eventnum = iEvent.id().event();
  
     const reco::BasicClusterCollection *clusterCollection_p = bccHandle.product();
  for (reco::BasicClusterCollection::const_iterator clus = clusterCollection_p->begin(); clus != clusterCollection_p->end(); ++clus)
   {
    double energy = clus->energy();
	double phi    = clus->phi();
	double eta    = clus->eta();
    
    double time = -1000.0;

	double ampli = 0.;
    double secondMin = 0.;
    double secondTime = -1000.;
    int numXtalsinE9 = 0;
	
	EBDetId maxDet;
	EBDetId secDet;

	std::vector<DetId> clusterDetIds = clus->getHitsByDetId();//get these from the cluster
    for(std::vector<DetId>::const_iterator detitr = clusterDetIds.begin(); detitr != clusterDetIds.end(); ++detitr)
      {
	  //Here I use the "find" on a digi collection... I have been warned...
	  if ((*detitr).det() != DetId::Ecal) { std::cout << " det is " <<(*detitr).det() << std::endl;continue;}
	  if ((*detitr).subdetId() != EcalBarrel) {std::cout << " subdet is " <<(*detitr).subdetId() << std::endl; continue; }
	  EcalRecHitCollection::const_iterator thishit = hits->find((*detitr));
	  if (thishit == hits->end()) 
	    {
	      continue;
	    }
		//The checking above should no longer be needed...... as only those in the cluster would already have rechits..
		
	  EcalRecHit myhit = (*thishit);
	    
	  double thisamp = myhit.energy();
      if (thisamp > 0.027) {numXtalsinE9++; }
	  if (thisamp > secondMin) {secondMin = thisamp; secondTime = myhit.time(); secDet = (EBDetId)(*detitr);}
	  if (secondMin > ampli) {std::swap(ampli,secondMin); std::swap(time,secondTime); std::swap(maxDet,secDet);}
     }

    float E2 = ampli + secondMin;
	EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId((EBDetId) maxDet);
    int FEDid = 600+elecId.dccId();
     
    //if ((ampli < minCosmicE1_) && (secondMin < minCosmicE2_)) continue; //Posibly add a third && (numXtalsinenergy<3) TEST IT FIRST
    numberOfCosmics++;

    
    //Set some more values
 
    int ieta = maxDet.ieta();
    int iphi = maxDet.iphi();

    int ietaSM = maxDet.ietaSM();
    int iphiSM = maxDet.iphiSM();

    // print out some info
    LogWarning("EcalCosmicsHists") << "hit! " << " amp " << ampli  << " : " 
					//<< fedMap_->getSliceFromFed(FEDid) 
					//<< " : ic " <<  ic << " : hashedIndex " << hashedIndex 
					<< " : ieta " << ieta << " iphi " << iphi 
					<< " : nCosmics " << " " << cosmicCounter_ << " / " << naiveEvtNum_ << endl;      
    
    // fill the proper hist
    TH1F* uRecHist = FEDsAndHists_[FEDid];
    TH1F* E2uRecHist = FEDsAndE2Hists_[FEDid];
    TH1F* energyuRecHist = FEDsAndenergyHists_[FEDid];
    TH1F* timingHist = FEDsAndTimingHists_[FEDid];
    TH1F* freqHist = FEDsAndFrequencyHists_[FEDid];
    TH1F* iphiProfileHist = FEDsAndiPhiProfileHists_[FEDid];
    TH1F* ietaProfileHist = FEDsAndiEtaProfileHists_[FEDid];
    TH2F* timingHistVsFreq = FEDsAndTimingVsFreqHists_[FEDid];
    TH2F* timingHistVsAmp = FEDsAndTimingVsAmpHists_[FEDid];
    TH2F* E2vsE1uRecHist = FEDsAndE2vsE1Hists_[FEDid];
    TH2F* energyvsE1uRecHist = FEDsAndenergyvsE1Hists_[FEDid];
    TH1F* numXtalInE9Hist = FEDsAndNumXtalsInE9Hists_[FEDid];    
    TH2F* occupHist = FEDsAndOccupancyHists_[FEDid];
    TH2F* timingHistVsPhi = FEDsAndTimingVsPhiHists_[FEDid];
    TH2F* timingHistVsModule = FEDsAndTimingVsModuleHists_[FEDid];

    if(uRecHist==0)
      {
	initHists(FEDid);
	uRecHist = FEDsAndHists_[FEDid];
	E2uRecHist = FEDsAndE2Hists_[FEDid];
	energyuRecHist = FEDsAndenergyHists_[FEDid];
	timingHist = FEDsAndTimingHists_[FEDid];
	freqHist = FEDsAndFrequencyHists_[FEDid];
	timingHistVsFreq = FEDsAndTimingVsFreqHists_[FEDid];
	timingHistVsAmp = FEDsAndTimingVsAmpHists_[FEDid];
	iphiProfileHist = FEDsAndiPhiProfileHists_[FEDid];
	ietaProfileHist = FEDsAndiEtaProfileHists_[FEDid];
	E2vsE1uRecHist = FEDsAndE2vsE1Hists_[FEDid];
	energyvsE1uRecHist = FEDsAndenergyvsE1Hists_[FEDid];
	numXtalInE9Hist = FEDsAndNumXtalsInE9Hists_[FEDid];
	occupHist = FEDsAndOccupancyHists_[FEDid];
	timingHistVsPhi = FEDsAndTimingVsPhiHists_[FEDid];
	timingHistVsModule = FEDsAndTimingVsModuleHists_[FEDid];
      }
    
    uRecHist->Fill(ampli);
    E2uRecHist->Fill(E2);
    E2vsE1uRecHist->Fill(ampli,E2);
    energyuRecHist->Fill(energy);
    energyvsE1uRecHist->Fill(ampli,energy);
    allFedsHist_->Fill(ampli);
    allFedsE2Hist_->Fill(E2); 
    allFedsenergyHist_->Fill(energy);
    allFedsenergyHighHist_->Fill(energy);
    allFedsE2vsE1Hist_->Fill(ampli,E2);
    allFedsenergyvsE1Hist_->Fill(ampli,energy);
    freqHist->Fill(naiveEvtNum_);
    iphiProfileHist->Fill(iphi);
    ietaProfileHist->Fill(ieta);

    allFedsFrequencyHist_->Fill(naiveEvtNum_);
    allFedsiPhiProfileHist_->Fill(iphi);
    allFedsiEtaProfileHist_->Fill(ieta);
    allOccupancy_->Fill(iphi, ieta);
    TrueOccupancy_->Fill(phi, eta);
    allOccupancyCoarse_->Fill(iphi, ieta);
    TrueOccupancyCoarse_->Fill(phi, eta);
    allFedsNumXtalsInE9Hist_->Fill(numXtalsinE9);
    numXtalInE9Hist->Fill(numXtalsinE9);
    occupHist->Fill(ietaSM,iphiSM);

    
    if(isEcalL1&&!isDTL1&&!isRPCL1&&!isCSCL1&&!isHCALL1) {
      allOccupancyECAL_->Fill(iphi, ieta);
      allOccupancyCoarseECAL_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
	allFedsTimingHistECAL_->Fill(time);
	allFedsTimingPhiEtaHistECAL_->Fill(iphi,ieta,time);
      }      
      triggerExclusiveHist_->Fill(0);
    } 
    
    if(!isEcalL1&&!isDTL1&&!isRPCL1&&!isCSCL1&&isHCALL1) {   
      allOccupancyHCAL_->Fill(iphi, ieta);
      allOccupancyCoarseHCAL_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
	allFedsTimingHistHCAL_->Fill(time);
	allFedsTimingPhiEtaHistHCAL_->Fill(iphi,ieta,time);
      }      
      triggerExclusiveHist_->Fill(1);
    }

    if(!isEcalL1&&isDTL1&&!isRPCL1&&!isCSCL1&&!isHCALL1) {
      allOccupancyDT_->Fill(iphi, ieta);
      allOccupancyCoarseDT_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
	allFedsTimingHistDT_->Fill(time);
	allFedsTimingPhiEtaHistDT_->Fill(iphi,ieta,time);
      }      
      triggerExclusiveHist_->Fill(2);
    }
    
    if(!isEcalL1&&!isDTL1&&isRPCL1&&!isCSCL1&&!isHCALL1) {   
      allOccupancyRPC_->Fill(iphi, ieta);
      allOccupancyCoarseRPC_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
	allFedsTimingHistRPC_->Fill(time);
	allFedsTimingPhiEtaHistRPC_->Fill(iphi,ieta,time);
      }      
      triggerExclusiveHist_->Fill(3);
    }

    if(!isEcalL1&&!isDTL1&&!isRPCL1&&isCSCL1&&!isHCALL1) {   
      allOccupancyCSC_->Fill(iphi, ieta);
      allOccupancyCoarseCSC_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
	allFedsTimingHistCSC_->Fill(time);
	allFedsTimingPhiEtaHistCSC_->Fill(iphi,ieta,time);
      }      
      triggerExclusiveHist_->Fill(4);
    }

    if (isEcalL1) triggerHist_->Fill(0);
    if (isHCALL1) triggerHist_->Fill(1);
    if (isDTL1)   triggerHist_->Fill(2);
    if (isRPCL1)  triggerHist_->Fill(3);
    if (isCSCL1)  triggerHist_->Fill(4);

    if (ampli > minTimingAmp_) {
      timingHist->Fill(time);
      timingHistVsFreq->Fill(time, naiveEvtNum_);
      timingHistVsAmp->Fill(time, ampli);
      allFedsTimingHist_->Fill(time);
      allFedsTimingVsAmpHist_->Fill(time, ampli);
      allFedsTimingVsFreqHist_->Fill(time, naiveEvtNum_);
      timingHistVsPhi->Fill(time, iphiSM);
      timingHistVsModule->Fill(time, ietaSM);
      allFedsTimingPhiHist_->Fill(iphi,time);
      allFedsTimingPhiEtaHist_->Fill(iphi,ieta,time);
      if (FEDid>=610&&FEDid<=627)  allFedsTimingPhiEbmHist_->Fill(iphi,time);
      if (FEDid>=628&&FEDid<=645)  allFedsTimingPhiEbpHist_->Fill(iphi,time);

      if (FEDid>=610&&FEDid<=627)  allFedsTimingEbmHist_->Fill(time);
      if (FEDid>=628&&FEDid<=645)  allFedsTimingEbpHist_->Fill(time);
      if (FEDid>=613&&FEDid<=616)  allFedsTimingEbmTopHist_->Fill(time);
      if (FEDid>=631&&FEDid<=634)  allFedsTimingEbpTopHist_->Fill(time);
      if (FEDid>=622&&FEDid<=625)  allFedsTimingEbmBottomHist_->Fill(time);
      if (FEDid>=640&&FEDid<=643)  allFedsTimingEbpBottomHist_->Fill(time);
    }

  }
  
  numberofCosmicsHist_->Fill(numberOfCosmics);
  if ( numberOfCosmics > 0 ) numberofGoodEvtFreq_->Fill(naiveEvtNum_);
  if ( numberOfCosmics > 0 ) cosmicCounter_++;

}


// insert the hist map into the map keyed by FED number
void EcalCosmicsHists::initHists(int FED)
{
  using namespace std;
  
  string FEDid = intToString(FED);
  string title1 = "Energy of Seed Crystal ";
  title1.append(fedMap_->getSliceFromFed(FED));
  title1.append(";Seed Energy (GeV);Number of Cosmics");
  string name1 = "SeedEnergyFED";
  name1.append(intToString(FED));
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;
  TH1F* hist = new TH1F(name1.c_str(),title1.c_str(), numBins, histRangeMin_, histRangeMax_);
  FEDsAndHists_[FED] = hist;
  FEDsAndHists_[FED]->SetDirectory(0);
  
  TH1F* E2hist = new TH1F(Form("E2_FED_%d",FED),Form("E2_FED_%d",FED), numBins, histRangeMin_, histRangeMax_);
  FEDsAndE2Hists_[FED] = E2hist;
  FEDsAndE2Hists_[FED]->SetDirectory(0);
  
  TH1F* energyhist = new TH1F(Form("Energy_FED_%d",FED),Form("Energy_FED_%d",FED), numBins, histRangeMin_, histRangeMax_);
  FEDsAndenergyHists_[FED] = energyhist;
  FEDsAndenergyHists_[FED]->SetDirectory(0);
  
  TH2F* E2vsE1hist = new TH2F(Form("E2vsE1_FED_%d",FED),Form("E2vsE1_FED_%d",FED), numBins, histRangeMin_, histRangeMax_, numBins, histRangeMin_, histRangeMax_);
  FEDsAndE2vsE1Hists_[FED] = E2vsE1hist;
  FEDsAndE2vsE1Hists_[FED]->SetDirectory(0);
 
  TH2F* energyvsE1hist = new TH2F(Form("EnergyvsE1_FED_%d",FED),Form("EnergyvsE1_FED_%d",FED), numBins, histRangeMin_, histRangeMax_, numBins, histRangeMin_, histRangeMax_);
  FEDsAndenergyvsE1Hists_[FED] = energyvsE1hist;
  FEDsAndenergyvsE1Hists_[FED]->SetDirectory(0);
  
  title1 = "Time for ";
  title1.append(fedMap_->getSliceFromFed(FED));
  title1.append(";Relative Time (1 clock = 25ns);Events");
  name1 = "TimeFED";
  name1.append(intToString(FED));
  TH1F* timingHist = new TH1F(name1.c_str(),title1.c_str(),78,-7,7);
  FEDsAndTimingHists_[FED] = timingHist;
  FEDsAndTimingHists_[FED]->SetDirectory(0);

  TH1F* freqHist = new TH1F(Form("Frequency_FED_%d",FED),Form("Frequency for FED %d;Event Number",FED),100,0.,100000);
  FEDsAndFrequencyHists_[FED] = freqHist;
  FEDsAndFrequencyHists_[FED]->SetDirectory(0);
  
  TH1F* iphiProfileHist = new TH1F(Form("iPhi_Profile_FED_%d",FED),Form("iPhi Profile for FED %d",FED),360,1.,361);
  FEDsAndiPhiProfileHists_[FED] = iphiProfileHist;
  FEDsAndiPhiProfileHists_[FED]->SetDirectory(0);
  
  TH1F* ietaProfileHist = new TH1F(Form("iEta_Profile_FED_%d",FED),Form("iEta Profile for FED %d",FED),172,-86,86);
  FEDsAndiEtaProfileHists_[FED] = ietaProfileHist;
  FEDsAndiEtaProfileHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsFreq = new TH2F(Form("timeVsFreqFED_%d",FED),Form("time Vs Freq FED %d",FED),78,-7,7,100,0.,100000);
  FEDsAndTimingVsFreqHists_[FED] = timingHistVsFreq;
  FEDsAndTimingVsFreqHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsAmp = new TH2F(Form("timeVsAmpFED_%d",FED),Form("time Vs Amp FED %d",FED),78,-7,7,numBins,histRangeMin_,histRangeMax_);
  FEDsAndTimingVsAmpHists_[FED] = timingHistVsAmp;
  FEDsAndTimingVsAmpHists_[FED]->SetDirectory(0);
  
  TH1F* numXtalInE9Hist = new TH1F(Form("NumXtalsInE9_FED_%d",FED),Form("Num active Xtals In E9 for FED %d;Num Active Xtals",FED),10,0,10);
  FEDsAndNumXtalsInE9Hists_[FED] = numXtalInE9Hist;
  FEDsAndNumXtalsInE9Hists_[FED]->SetDirectory(0);

  TH2F* OccupHist = new TH2F(Form("occupFED_%d",FED),Form("Occupancy FED %d;i#eta;i#phi",FED),85,1,86,20,1,21);
  FEDsAndOccupancyHists_[FED] = OccupHist;
  FEDsAndOccupancyHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsPhi = new TH2F(Form("timeVsPhiFED_%d",FED),Form("time Vs Phi FED %d;Relative Time (1 clock = 25ns);i#phi",FED),78,-7,7,20,1,21);
  FEDsAndTimingVsPhiHists_[FED] = timingHistVsPhi;
  FEDsAndTimingVsPhiHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsModule = new TH2F(Form("timeVsModuleFED_%d",FED),Form("time Vs Module FED %d;Relative Time (1 clock = 25ns);i#eta",FED),78,-7,7,4,1,86);
  FEDsAndTimingVsModuleHists_[FED] = timingHistVsModule;
  FEDsAndTimingVsModuleHists_[FED]->SetDirectory(0);

}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalCosmicsHists::beginJob(const edm::EventSetup&)
{
  //Here I will init some of the specific histograms
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;
  allFedsenergyHist_           = new TH1F("energy_AllClusters","energy_AllClusters;Cluster Energy (GeV)",numBins,histRangeMin_,histRangeMax_);
  allFedsenergyHighHist_       = new TH1F("energyHigh_AllClusters","energyHigh_AllClusters;Cluster Energy (GeV)",numBins,histRangeMin_,200.0);
  allFedsE2Hist_           = new TH1F("E2_AllClusters","E2_AllClusters;Seed+highest neighbor energy (GeV)",numBins,histRangeMin_,histRangeMax_);
  allFedsE2vsE1Hist_       = new TH2F("E2vsE1_AllClusters","E2vsE1_AllClusters;Seed Energy (GeV);Seed+highest neighbor energy (GeV)",numBins,histRangeMin_,histRangeMax_,numBins,histRangeMin_,histRangeMax_);
  allFedsenergyvsE1Hist_       = new TH2F("energyvsE1_AllClusters","energyvsE1_AllClusters;Seed Energy (GeV);Energy(GeV)",numBins,histRangeMin_,histRangeMax_,numBins,histRangeMin_,histRangeMax_);
  allFedsTimingHist_       = new TH1F("timeForAllFeds","timeForAllFeds;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingVsFreqHist_ = new TH2F("timeVsFreqAllEvent","time Vs Freq All events;Relative Time (1 clock = 25ns);Event Number",78,-7,7,2000,0.,200000);
  allFedsTimingVsAmpHist_  = new TH2F("timeVsAmpAllEvents","time Vs Amp All Events;Relative Time (1 clock = 25ns);Amplitude (GeV)",78,-7,7,numBins,histRangeMin_,histRangeMax_);
  allFedsFrequencyHist_    = new TH1F("FrequencyAllEvent","Frequency for All events;Event Number",2000,0.,200000);
  allFedsiPhiProfileHist_  = new TH1F("iPhiProfileAllEvents","iPhi Profile all events;i#phi",360,1.,361.);
  allFedsiEtaProfileHist_  = new TH1F("iEtaProfileAllEvents","iEta Profile all events;i#eta",172,-86,86);
  allOccupancy_            = new TH2F("OccupancyAllEvents","Occupancy all events;i#phi;i#eta",360,1.,361.,172,-86,86);
  TrueOccupancy_            = new TH2F("TrueOccupancyAllEvents","True Occupancy all events;#phi;#eta",360,-3.14159,3.14159,172,-1.5,1.5);
  allOccupancyCoarse_      = new TH2F("OccupancyAllEventsCoarse","Occupancy all events Coarse;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  TrueOccupancyCoarse_      = new TH2F("TrueOccupancyAllEventsCoarse","True Occupancy all events Coarse;#phi;#eta",360/5,-3.14159,3.14159,172/5,-1.5,1.5);
  allFedsNumXtalsInE9Hist_ = new TH1F("NumXtalsInE9AllHist","Number of active Xtals in E9;NumXtals",10,0,10);

  allFedsTimingPhiHist_          = new TH2F("timePhiAllFEDs","time vs Phi for all FEDs (TT binning);i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEbpHist_       = new TH2F("timePhiEBP","time vs Phi for FEDs in EB+ (TT binning) ;i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEbmHist_       = new TH2F("timePhiEBM","time vs Phi for FEDs in EB- (TT binning);i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEtaHist_       = new TH3F("timePhiEtaAllFEDs","(Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  allFedsTimingEbpHist_       = new TH1F("timeEBP","time for FEDs in EB+;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmHist_       = new TH1F("timeEBM","time for FEDs in EB-;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbpTopHist_    = new TH1F("timeEBPTop","time for FEDs in EB+ Top;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmTopHist_    = new TH1F("timeEBMTop","time for FEDs in EB- Top;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbpBottomHist_ = new TH1F("timeEBPBottom","time for FEDs in EB+ Bottom;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmBottomHist_ = new TH1F("timeEBMBottom","time for FEDs in EB- Bottom;Relative Time (1 clock = 25ns)",78,-7,7);

  numberofCosmicsHist_ = new TH1F("numberofCosmicsPerEvent","Number of cosmics per event;Number of Cosmics",10,0,10);
  numberofGoodEvtFreq_  = new TH1F("frequencyOfGoodEvents","Number of events with cosmic vs Event;Event Number;Number of Good Events/100 Events",2000,0,200000);

  allOccupancyECAL_            = new TH2F("OccupancyAllEvents_ECAL","Occupancy all events ECAL;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseECAL_      = new TH2F("OccupancyAllEventsCoarse_ECAL","Occupancy all events Coarse ECAL;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsTimingHistECAL_       = new TH1F("timeForAllFeds_ECAL","timeForAllFeds ECAL;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistECAL_       = new TH3F("timePhiEtaAllFEDs_ECAL","ECAL (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  allOccupancyDT_            = new TH2F("OccupancyAllEvents_DT","Occupancy all events DT;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseDT_      = new TH2F("OccupancyAllEventsCoarse_DT","Occupancy all events Coarse DT;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsTimingHistDT_       = new TH1F("timeForAllFeds_DT","timeForAllFeds DT;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistDT_       = new TH3F("timePhiEtaAllFEDs_DT","DT (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  allOccupancyRPC_            = new TH2F("OccupancyAllEvents_RPC","Occupancy all events RPC;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseRPC_      = new TH2F("OccupancyAllEventsCoarse_RPC","Occupancy all events Coarse RPC;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsTimingHistRPC_       = new TH1F("timeForAllFeds_RPC","timeForAllFeds RPC;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistRPC_       = new TH3F("timePhiEtaAllFEDs_RPC","RPC (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  allOccupancyCSC_            = new TH2F("OccupancyAllEvents_CSC","Occupancy all events CSC;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseCSC_      = new TH2F("OccupancyAllEventsCoarse_CSC","Occupancy all events Coarse CSC;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsTimingHistCSC_       = new TH1F("timeForAllFeds_CSC","timeForAllFeds CSC;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistCSC_       = new TH3F("timePhiEtaAllFEDs_CSC","CSC (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  allOccupancyHCAL_            = new TH2F("OccupancyAllEvents_HCAL","Occupancy all events HCAL;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseHCAL_      = new TH2F("OccupancyAllEventsCoarse_HCAL","Occupancy all events Coarse HCAL;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsTimingHistHCAL_       = new TH1F("timeForAllFeds_HCAL","timeForAllFeds HCAL;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistHCAL_       = new TH3F("timePhiEtaAllFEDs_HCAL","HCAL (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,1,361,8,-86,86,78,-7,7);  

  triggerHist_ = new TH1F("triggerHist","Trigger Number",5,0,5);
  triggerExclusiveHist_ = new TH1F("triggerExclusiveHist","Trigger Number (Mutually Exclusive)",5,0,5);

  runNumberHist_ = new TH1F("runNumberHist","Run Number",1,0,1);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalCosmicsHists::endJob()
{
  using namespace std;
  fileName_ += "-"+intToString(runNum_)+".graph.root";

  TFile root_file_(fileName_.c_str() , "RECREATE");

  for(map<int,TH1F*>::const_iterator itr = FEDsAndHists_.begin();
      itr != FEDsAndHists_.end(); ++itr)
  {
    string dir = fedMap_->getSliceFromFed(itr->first);
    TDirectory* FEDdir = gDirectory->mkdir(dir.c_str());
    FEDdir->cd();

    TH1F* hist = itr->second;
    if(hist!=0)
      hist->Write();
    else
    {
      cerr << "EcalCosmicsHists: Error: This shouldn't happen!" << endl;
    }
    // Write out timing hist
    hist = FEDsAndTimingHists_[itr->first];
    if(hist!=0)
      hist->Write();
    else
    {
      cerr << "EcalCosmicsHists: Error: This shouldn't happen!" << endl;
    }
	
	hist = FEDsAndFrequencyHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndiPhiProfileHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndiEtaProfileHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndE2Hists_[itr->first];
	hist->Write();
	
	hist = FEDsAndenergyHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndNumXtalsInE9Hists_[itr->first];
	hist->Write();
	
	TH2F* hist2 = FEDsAndTimingVsAmpHists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndTimingVsFreqHists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndE2vsE1Hists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndenergyvsE1Hists_[itr->first];
	hist2->Write();

	hist2 = FEDsAndOccupancyHists_[itr->first];
	hist2->Write();

	hist2 = FEDsAndTimingVsPhiHists_[itr->first];
	hist2->Write();

	hist2 = FEDsAndTimingVsModuleHists_[itr->first];
	hist2->Write();

    root_file_.cd();
  }
  allFedsHist_->Write();
  allFedsE2Hist_->Write();
  allFedsenergyHist_->Write();
  allFedsenergyHighHist_->Write();
  allFedsE2vsE1Hist_->Write();
  allFedsenergyvsE1Hist_->Write();
  allFedsTimingHist_->Write();
  allFedsTimingVsAmpHist_->Write();
  allFedsFrequencyHist_->Write();
  allFedsTimingVsFreqHist_->Write();
  allFedsiEtaProfileHist_->Write();
  allFedsiPhiProfileHist_->Write();
  allOccupancy_->Write();
  TrueOccupancy_->Write();
  allOccupancyCoarse_->Write();
  TrueOccupancyCoarse_->Write();
  allFedsNumXtalsInE9Hist_->Write();
  allFedsTimingPhiHist_->Write();
  allFedsTimingPhiEbpHist_->Write();
  allFedsTimingPhiEbmHist_->Write();
  allFedsTimingEbpHist_->Write();
  allFedsTimingEbmHist_->Write();
  allFedsTimingEbpTopHist_->Write();
  allFedsTimingEbmTopHist_->Write();
  allFedsTimingEbpBottomHist_->Write();
  allFedsTimingEbmBottomHist_->Write();
  allFedsTimingPhiEtaHist_->Write();

  
  allOccupancyECAL_->Write();
  allOccupancyCoarseECAL_->Write();
  allFedsTimingPhiEtaHistECAL_->Write();
  allFedsTimingHistECAL_->Write();

  allOccupancyHCAL_->Write();
  allOccupancyCoarseHCAL_->Write();
  allFedsTimingPhiEtaHistHCAL_->Write();
  allFedsTimingHistHCAL_->Write();

  allOccupancyDT_->Write();
  allOccupancyCoarseDT_->Write();
  allFedsTimingPhiEtaHistDT_->Write();
  allFedsTimingHistDT_->Write();

  allOccupancyRPC_->Write();
  allOccupancyCoarseRPC_->Write();
  allFedsTimingPhiEtaHistRPC_->Write();
  allFedsTimingHistRPC_->Write();

  allOccupancyCSC_->Write();
  allOccupancyCoarseCSC_->Write();
  allFedsTimingPhiEtaHistCSC_->Write();
  allFedsTimingHistCSC_->Write();

  triggerHist_->Write();
  triggerExclusiveHist_->Write();

  numberofCosmicsHist_->Write();
  numberofGoodEvtFreq_->Write();

  runNumberHist_->SetBinContent(1,runNum_);
  runNumberHist_->Write();

  root_file_.Close();

 

  LogWarning("EcalCosmicsHists") << "---> Number of cosmic events: " << cosmicCounter_ << " in " << naiveEvtNum_ << " events.";

}


std::string EcalCosmicsHists::intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}

