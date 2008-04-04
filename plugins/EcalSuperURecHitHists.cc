// -*- C++ -*-
//
// Package:   EcalSuperURecHitHists 
// Class:     EcalSuperURecHitHists  
// 
/**\class EcalSuperURecHitHists EcalSuperURecHitHists.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/

 
#include "CaloOnlineTools/EcalTools/plugins/EcalSuperURecHitHists.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
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
EcalSuperURecHitHists::EcalSuperURecHitHists(const edm::ParameterSet& iConfig) :
  EcalUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection")),
  runNum_(-1),
  histRangeMax_ (iConfig.getUntrackedParameter<double>("histogramMaxRange",200.0)),
  histRangeMin_ (iConfig.getUntrackedParameter<double>("histogramMinRange",-10.0)),
  minSeedAmp_ (iConfig.getUntrackedParameter<double>("MinSeedAmp",8.0)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("ecalURechHitHists"))),
  minCosmicE1_ (iConfig.getUntrackedParameter<double>("MinCosmicE1", 10.0)),
  minCosmicE2_ (iConfig.getUntrackedParameter<double>("MinCosmicE2", 12.0)),
  minCosmicE9_ (iConfig.getUntrackedParameter<double>("MinCosmicE9", 12.0))
{
  naiveEvtNum_ = 0;
  cosmicCounter_ = 0;

  vector<int> listDefaults;
  listDefaults.push_back(-1);
  
  maskedChannels_ = iConfig.getUntrackedParameter<vector<int> >("maskedChannels", listDefaults);
  maskedFEDs_ = iConfig.getUntrackedParameter<vector<int> >("maskedFEDs", listDefaults);

  vector<string> defaultMaskedEBs;
  defaultMaskedEBs.push_back("none");
  maskedEBs_ =  iConfig.getUntrackedParameter<vector<string> >("maskedEBs",defaultMaskedEBs);
  
  fedMap_ = new EcalFedMap();
  string title1 = "Uncalib Rec Hits (ADC counts)";
  string name1 = "URecHitsAllFEDs";
  int numBins = (int)round(histRangeMax_-histRangeMin_)+1;
  allFedsHist_ = new TH1F(name1.c_str(),title1.c_str(),numBins,histRangeMin_,histRangeMax_);
  //title1 = "Jitter for all FEDs";
  //name1 = "JitterAllFEDs";
  //allFedsTimingHist_ = new TH1F(name1.c_str(),title1.c_str(),14,-7,7);

  // load up the maskedFED list with the proper FEDids
  if(maskedFEDs_[0]==-1)
  {
    //if "actual" EB id given, then convert to FEDid and put in listFEDs_
    if(maskedEBs_[0] != "none")
    {
      maskedFEDs_.clear();
      for(vector<string>::const_iterator ebItr = maskedEBs_.begin(); ebItr != maskedEBs_.end(); ++ebItr)
      {
        maskedFEDs_.push_back(fedMap_->getFedFromSlice(*ebItr));
      }
    }
  }
}


EcalSuperURecHitHists::~EcalSuperURecHitHists()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalSuperURecHitHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  int ievt = iEvent.id().event();
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);
  Handle<EcalUncalibratedRecHitCollection> hits;
  
  naiveEvtNum_++;
    
  if(runNum_==-1)
    {
      runNum_ = iEvent.id().run();
    }      

 //TODO: improve try/catch behavior
  try
  {
    iEvent.getByLabel(EcalUncalibratedRecHitCollection_, hits);
    int neh = hits->size();
    LogDebug("EcalSuperURecHitHists") << "event " << ievt << " hits collection size " << neh;
  }
  catch ( exception& ex)
  {
    LogWarning("EcalSuperURecHitHists") << EcalUncalibratedRecHitCollection_ << " not available";
  }
  
  ESHandle<CaloTopology> caloTopo;
  iSetup.get<CaloTopologyRecord>().get(caloTopo);
  
  //This allows me to make sure I don't use a channel more than once
  std::vector<int> usedChannels_;
  int  numberOfCosmics = 0;
  int eventnum = iEvent.id().event();
  for (EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr)
  {
    EcalUncalibratedRecHit hit = (*hitItr);
    EBDetId ebDet = hit.id();
    //TODO: make it work for endcap FEDs also
    int ic = ebDet.ic();
    int hashedIndex = ebDet.hashedIndex();
    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(ebDet);
    int FEDid = 600+elecId.dccId();
    float ampli = hit.amplitude();

    if (ampli < minSeedAmp_ ) continue;

    vector<int>::iterator result;
    result = find(maskedFEDs_.begin(), maskedFEDs_.end(), FEDid);
    if(result != maskedFEDs_.end())
    {
      LogWarning("EcalSuperURecHitHists") << "skipping uncalRecHit for masked FED " << FEDid << " ; amplitude " << ampli;
      continue;
    }      

    result = find(maskedChannels_.begin(), maskedChannels_.end(), hashedIndex);
    if  (result != maskedChannels_.end())
    {
      LogWarning("EcalSuperURecHitHists") << "skipping masked uncalRecHit for channel: " << ic << " with amplitude " << ampli ;
      continue;
    }
	
    result = find(usedChannels_.begin(), usedChannels_.end(), hashedIndex);
    if  (result != usedChannels_.end())
    {
      LogWarning("EcalSuperURecHitHists") << "skipping used uncalRecHit for channel: " << ic << " with amplitude " << ampli ;
      continue;
    }
    std::cout << " this one is good " << hashedIndex << " SM " <<ebDet.ism() << " ic " << ic << std::endl;
    //This is cause we use the original channel
    usedChannels_.push_back(hashedIndex);
    
    //Get some neighbor information
    std::vector<DetId> neighbors = caloTopo->getWindow(ebDet,3,3);
    float secondMin = 0.;
    float E9 = ampli;
    int numXtalsinE9 = 1;
    for(std::vector<DetId>::const_iterator detitr = neighbors.begin(); detitr != neighbors.end(); ++detitr)
      {
	//Here I use the "find" on a digi collection... I have been warned...
	if ((*detitr).det() != DetId::Ecal) { std::cout << " det is " <<(*detitr).det() << std::endl;continue;}
	if ((*detitr).subdetId() != EcalBarrel) {std::cout << " subdet is " <<(*detitr).subdetId() << std::endl; continue; }
	EcalUncalibratedRecHitCollection::const_iterator thishit = hits->find((*detitr));
	if (thishit == hits->end()) 
	  {
	    //std::cout << "Great Holly, it ain't in there "<< EBDetId(*detitr) << std::endl;
	    continue;
	  }
	EcalUncalibratedRecHit myhit = (*thishit);
	EBDetId neighborDetId = myhit.id();
	int neighborHashedIndex = neighborDetId.hashedIndex();
	if (find(maskedChannels_.begin(), maskedChannels_.end(), neighborHashedIndex)==maskedChannels_.end())
	  {
	    if (find(usedChannels_.begin(), usedChannels_.end(), neighborHashedIndex)==usedChannels_.end())
	      {
		usedChannels_.push_back(neighborHashedIndex);
		float thisamp = (*thishit).amplitude();
		//if (thisamp > 3.0) E9+=thisamp; //To reduce noise maybe only include those above a certain amplitude
		E9+=thisamp; //To reduce noise maybe only include those above a certain amplitude
		if (thisamp > secondMin) secondMin = thisamp;
		if (thisamp > 3.0) numXtalsinE9++;
	      }
	  }
     }
    if (secondMin > ampli) std::swap(ampli,secondMin);

    float E2 = ampli + secondMin;
    //    float E8 = E9-ampli;
    //    float E2mE1 = secondMin;
     
    if ((ampli < minCosmicE1_) && (E2 < minCosmicE2_) && (E9 < minCosmicE9_)) continue;
    numberOfCosmics++;

    
    //Set some more values
 
    float jitter = hit.jitter()+1.;
    int ieta = ebDet.ieta();
    int iphi = ebDet.iphi();

    int ietaSM = ebDet.ietaSM();
    int iphiSM = ebDet.iphiSM();

    // print out some info
    LogWarning("EcalSuperURecHitHists") << "hit! " << " amp " << ampli  << " : " 
					<< fedMap_->getSliceFromFed(FEDid) 
					<< " : ic " <<  ic << " : hashedIndex " << hashedIndex 
					<< " : ieta " << ieta << " iphi " << iphi 
					<< " : nCosmics " << " " << cosmicCounter_ << " / " << naiveEvtNum_ << endl;      
    
    // fill the proper hist
    TH1F* uRecHist = FEDsAndHists_[FEDid];
    TH1F* E2uRecHist = FEDsAndE2Hists_[FEDid];
    TH1F* E9uRecHist = FEDsAndE9Hists_[FEDid];
    TH1F* timingHist = FEDsAndTimingHists_[FEDid];
    TH1F* freqHist = FEDsAndFrequencyHists_[FEDid];
    TH1F* iphiProfileHist = FEDsAndiPhiProfileHists_[FEDid];
    TH1F* ietaProfileHist = FEDsAndiEtaProfileHists_[FEDid];
    TH2F* timingHistVsFreq = FEDsAndTimingVsFreqHists_[FEDid];
    TH2F* timingHistVsAmp = FEDsAndTimingVsAmpHists_[FEDid];
    TH2F* E2vsE1uRecHist = FEDsAndE2vsE1Hists_[FEDid];
    TH2F* E9vsE1uRecHist = FEDsAndE9vsE1Hists_[FEDid];
    TH1F* numXtalInE9Hist = FEDsAndNumXtalsInE9Hists_[FEDid];    
    TH2F* occupHist = FEDsAndOccupancyHists_[FEDid];
    TH2F* timingHistVsPhi = FEDsAndTimingVsPhiHists_[FEDid];

    if(uRecHist==0)
      {
	initHists(FEDid);
	uRecHist = FEDsAndHists_[FEDid];
	E2uRecHist = FEDsAndE2Hists_[FEDid];
	E9uRecHist = FEDsAndE9Hists_[FEDid];
	timingHist = FEDsAndTimingHists_[FEDid];
	freqHist = FEDsAndFrequencyHists_[FEDid];
	timingHistVsFreq = FEDsAndTimingVsFreqHists_[FEDid];
	timingHistVsAmp = FEDsAndTimingVsAmpHists_[FEDid];
	iphiProfileHist = FEDsAndiPhiProfileHists_[FEDid];
	ietaProfileHist = FEDsAndiEtaProfileHists_[FEDid];
	E2vsE1uRecHist = FEDsAndE2vsE1Hists_[FEDid];
	E9vsE1uRecHist = FEDsAndE9vsE1Hists_[FEDid];
	numXtalInE9Hist = FEDsAndNumXtalsInE9Hists_[FEDid];
	occupHist = FEDsAndOccupancyHists_[FEDid];
	timingHistVsPhi = FEDsAndTimingVsPhiHists_[FEDid];
      }
    
    uRecHist->Fill(ampli);
    E2uRecHist->Fill(E2);
    E2vsE1uRecHist->Fill(ampli,E2);
    E9uRecHist->Fill(E9);
    E9vsE1uRecHist->Fill(ampli,E9);
    allFedsHist_->Fill(ampli);
    allFedsE2Hist_->Fill(E2);
    allFedsE9Hist_->Fill(E9);
    allFedsE2vsE1Hist_->Fill(ampli,E2);
    allFedsE9vsE1Hist_->Fill(ampli,E9);
    timingHist->Fill(jitter);
    freqHist->Fill(eventnum);
    timingHistVsFreq->Fill(jitter, eventnum);
    timingHistVsAmp->Fill(jitter, ampli);
    iphiProfileHist->Fill(iphi);
    ietaProfileHist->Fill(ieta);
    allFedsTimingHist_->Fill(jitter);
    allFedsFrequencyHist_->Fill(eventnum);
    allFedsTimingVsAmpHist_->Fill(jitter, ampli);
    allFedsTimingVsFreqHist_->Fill(jitter, eventnum);
    allFedsiPhiProfileHist_->Fill(iphi);
    allFedsiEtaProfileHist_->Fill(ieta);
    allOccupancy_->Fill(iphi, ieta);
    allOccupancyCoarse_->Fill(iphi, ieta);
    allFedsNumXtalsInE9Hist_->Fill(numXtalsinE9);
    numXtalInE9Hist->Fill(numXtalsinE9);
    timingHistVsPhi->Fill(jitter, iphiSM);

    allFedsTimingPhiHist_->Fill(iphi,jitter);
    if (FEDid>=610&&FEDid<=627)  allFedsTimingPhiEbmHist_->Fill(iphi,jitter);
    if (FEDid>=628&&FEDid<=645)  allFedsTimingPhiEbpHist_->Fill(iphi,jitter);
    occupHist->Fill(ietaSM,iphiSM);

  }
  
  numberofCosmicsHist_->Fill(numberOfCosmics);
  if (numberOfCosmics > 0 ) numberofGoodEvtFreq_->Fill(eventnum);
  if ( numberOfCosmics > 0 ) cosmicCounter_++;
  
}


// insert the hist map into the map keyed by FED number
void EcalSuperURecHitHists::initHists(int FED)
{
  using namespace std;
  
  string FEDid = intToString(FED);
  string title1 = "Uncalib Rec Hits (ADC counts) for ";
  title1.append(fedMap_->getSliceFromFed(FED));
  string name1 = "URecHitsFED";
  name1.append(intToString(FED));
  int numBins = (int)round(histRangeMax_-histRangeMin_)+1;
  TH1F* hist = new TH1F(name1.c_str(),title1.c_str(), numBins, histRangeMin_, histRangeMax_);
  FEDsAndHists_[FED] = hist;
  FEDsAndHists_[FED]->SetDirectory(0);
  
  TH1F* E2hist = new TH1F(Form("E2_FED_%d",FED),Form("E2_FED_%d",FED), numBins, histRangeMin_, histRangeMax_);
  FEDsAndE2Hists_[FED] = E2hist;
  FEDsAndE2Hists_[FED]->SetDirectory(0);
  
  TH1F* E9hist = new TH1F(Form("E9_FED_%d",FED),Form("E9_FED_%d",FED), numBins, histRangeMin_, histRangeMax_);
  FEDsAndE9Hists_[FED] = E9hist;
  FEDsAndE9Hists_[FED]->SetDirectory(0);
  
  TH2F* E2vsE1hist = new TH2F(Form("E2vsE1_FED_%d",FED),Form("E2vsE1_FED_%d",FED), numBins, histRangeMin_, histRangeMax_, numBins, histRangeMin_, histRangeMax_);
  FEDsAndE2vsE1Hists_[FED] = E2vsE1hist;
  FEDsAndE2vsE1Hists_[FED]->SetDirectory(0);
 
  TH2F* E9vsE1hist = new TH2F(Form("E9vsE1_FED_%d",FED),Form("E9vsE1_FED_%d",FED), numBins, histRangeMin_, histRangeMax_, numBins, histRangeMin_, histRangeMax_);
  FEDsAndE9vsE1Hists_[FED] = E9vsE1hist;
  FEDsAndE9vsE1Hists_[FED]->SetDirectory(0);
  
  title1 = "Jitter for ";
  title1.append(fedMap_->getSliceFromFed(FED));
  name1 = "JitterFED";
  name1.append(intToString(FED));
  TH1F* timingHist = new TH1F(name1.c_str(),title1.c_str(),78,-7,7);
  FEDsAndTimingHists_[FED] = timingHist;
  FEDsAndTimingHists_[FED]->SetDirectory(0);

  TH1F* freqHist = new TH1F(Form("Frequency_FED_%d",FED),Form("Frequency for FED %d",FED),100,0.,100000);
  FEDsAndFrequencyHists_[FED] = freqHist;
  FEDsAndFrequencyHists_[FED]->SetDirectory(0);
  
  TH1F* iphiProfileHist = new TH1F(Form("iPhi_Profile_FED_%d",FED),Form("iPhi Profile for FED %d",FED),360,1.,361);
  FEDsAndiPhiProfileHists_[FED] = iphiProfileHist;
  FEDsAndiPhiProfileHists_[FED]->SetDirectory(0);
  
  TH1F* ietaProfileHist = new TH1F(Form("iEta_Profile_FED_%d",FED),Form("iEta Profile for FED %d",FED),172,-86,86);
  FEDsAndiEtaProfileHists_[FED] = ietaProfileHist;
  FEDsAndiEtaProfileHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsFreq = new TH2F(Form("JitterVsFreqFED_%d",FED),Form("Jitter Vs Freq FED %d",FED),78,-7,7,100,0.,100000);
  FEDsAndTimingVsFreqHists_[FED] = timingHistVsFreq;
  FEDsAndTimingVsFreqHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsAmp = new TH2F(Form("JitterVsAmpFED_%d",FED),Form("Jitter Vs Amp FED %d",FED),78,-7,7,numBins,histRangeMin_,histRangeMax_);
  FEDsAndTimingVsAmpHists_[FED] = timingHistVsAmp;
  FEDsAndTimingVsAmpHists_[FED]->SetDirectory(0);
  
  TH1F* numXtalInE9Hist = new TH1F(Form("NumXtalsInE9_FED_%d",FED),Form("Num active Xtals In E9 for FED %d;Num Active Xtals",FED),10,0,10);
  FEDsAndNumXtalsInE9Hists_[FED] = numXtalInE9Hist;
  FEDsAndNumXtalsInE9Hists_[FED]->SetDirectory(0);

  TH2F* OccupHist = new TH2F(Form("occupFED_%d",FED),Form("Occupancy FED %d",FED),85,1,86,20,1,21);
  FEDsAndOccupancyHists_[FED] = OccupHist;
  FEDsAndOccupancyHists_[FED]->SetDirectory(0);

  TH2F* timingHistVsPhi = new TH2F(Form("JitterVsPhiFED_%d",FED),Form("Jitter Vs Phi FED %d",FED),78,-7,7,20,1,21);

  FEDsAndTimingVsPhiHists_[FED] = timingHistVsPhi;
  FEDsAndTimingVsPhiHists_[FED]->SetDirectory(0);

}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalSuperURecHitHists::beginJob(const edm::EventSetup&)
{
  //Here I will init some of the specific histograms
  int numBins = (int)round(histRangeMax_-histRangeMin_)+1;
  allFedsE9Hist_           = new TH1F("E9_AllURecHits","E9_AllURecHits;E_3x3 (ADC)",numBins,histRangeMin_,histRangeMax_);
  allFedsE2Hist_           = new TH1F("E2_AllURecHits","E2_AllURecHits;E_1+E_2 (ADC)",numBins,histRangeMin_,histRangeMax_);
  allFedsE2vsE1Hist_       = new TH2F("E2vsE1_AllURecHits","E2vsE1_AllURecHits;E_1(ADC);E2(ADC)",numBins,histRangeMin_,histRangeMax_,numBins,histRangeMin_,histRangeMax_);
  allFedsE9vsE1Hist_       = new TH2F("E9vsE1_AllURecHits","E9vsE1_AllURecHits;E_1(ADC);E9(ADC)",numBins,histRangeMin_,histRangeMax_,numBins,histRangeMin_,histRangeMax_);
  allFedsTimingHist_       = new TH1F("JitterForAllFeds","JitterForAllFeds;Jitter (MaxSample - 5)",78,-7,7);
  allFedsTimingVsFreqHist_ = new TH2F("JitterVsFreqAllEvent","Jitter Vs Freq All events;Jitter (MaxSample - 5);Event Number",78,-7,7,2000,0.,200000);
  allFedsTimingVsAmpHist_  = new TH2F("JitterVsAmpAllEvents","Jitter Vs Amp All Events;Jitter (MaxSample - 5);Amplitude",78,-7,7,numBins,histRangeMin_,histRangeMax_);
  allFedsFrequencyHist_    = new TH1F("FrequencyAllEvent","Frequency for All events",2000,0.,200000);
  allFedsiPhiProfileHist_  = new TH1F("iPhiProfileAllEvents","iPhi Profile all events;i#phi",360,1.,361.);
  allFedsiEtaProfileHist_  = new TH1F("iEtaProfileAllEvents","iEta Profile all events;i#eta",172,-86,86);
  allOccupancy_            = new TH2F("OccupancyAllEvents","Occupancy all events;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarse_      = new TH2F("OccupancyAllEventsCoarse","Occupancy all events Coarse;i#phi;i#eta",360/5,1,361.,172/5,-86,86);
  allFedsNumXtalsInE9Hist_ = new TH1F("NumXtalsInE9AllHist","Number of active Xtals in E9;NumXtals",10,0,10);

  allFedsTimingPhiHist_          = new TH2F("JitterPhiAllFEDs","Jitter vs Phi for all FEDs (TT binning)",72,1,361,78,-7,7);
  allFedsTimingPhiEbpHist_       = new TH2F("JitterPhiEBP","Jitter vs Phi for FEDs in EB+ (TT binning) ;i#phi;jitter (MaxSample -5)",72,1,361,78,-7,7);
  allFedsTimingPhiEbmHist_       = new TH2F("JitterPhiEBM","Jitter vs Phi for FEDs in EB- (TT binning);i#phi;jitter (MaxSample -5)",72,1,361,78,-7,7);
  
  numberofCosmicsHist_ = new TH1F("numberofCosmicsPerEvent","Number of cosmics per event;Number of Cosmics",10,0,10);
  numberofGoodEvtFreq_  = new TH1F("frequencyOfGoodEvents","Number of events with cosmic vs Event;Event Number;Number of Good Events/100 Events",2000,0,200000);
  }

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalSuperURecHitHists::endJob()
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
      cerr << "EcalSuperURecHitHists: Error: This shouldn't happen!" << endl;
    }
    // Write out timing hist
    hist = FEDsAndTimingHists_[itr->first];
    if(hist!=0)
      hist->Write();
    else
    {
      cerr << "EcalSuperURecHitHists: Error: This shouldn't happen!" << endl;
    }
	
	hist = FEDsAndFrequencyHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndiPhiProfileHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndiEtaProfileHists_[itr->first];
	hist->Write();
	
	hist = FEDsAndE2Hists_[itr->first];
	hist->Write();
	
	hist = FEDsAndE9Hists_[itr->first];
	hist->Write();
	
	hist = FEDsAndNumXtalsInE9Hists_[itr->first];
	hist->Write();
	
	TH2F* hist2 = FEDsAndTimingVsAmpHists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndTimingVsFreqHists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndE2vsE1Hists_[itr->first];
	hist2->Write();
	
	hist2 = FEDsAndE9vsE1Hists_[itr->first];
	hist2->Write();

	hist2 = FEDsAndOccupancyHists_[itr->first];
	hist2->Write();

	hist2 = FEDsAndTimingVsPhiHists_[itr->first];
	hist2->Write();

    root_file_.cd();
  }
  allFedsHist_->Write();
  allFedsE2Hist_->Write();
  allFedsE9Hist_->Write();
  allFedsE2vsE1Hist_->Write();
  allFedsE9vsE1Hist_->Write();
  allFedsTimingHist_->Write();
  allFedsTimingVsAmpHist_->Write();
  allFedsFrequencyHist_->Write();
  allFedsTimingVsFreqHist_->Write();
  allFedsiEtaProfileHist_->Write();
  allFedsiPhiProfileHist_->Write();
  allOccupancy_->Write();
  allOccupancyCoarse_->Write();
  allFedsNumXtalsInE9Hist_->Write();
  allFedsTimingPhiHist_->Write();
  allFedsTimingPhiEbpHist_->Write();
  allFedsTimingPhiEbmHist_->Write();

  numberofCosmicsHist_->Write();
  numberofGoodEvtFreq_->Write();

  root_file_.Close();

  std::string channels;
  for(std::vector<int>::const_iterator itr = maskedChannels_.begin();
      itr != maskedChannels_.end(); ++itr)
  {
    channels+=intToString(*itr);
    channels+=",";
  }
  
  LogWarning("EcalSuperURecHitHists") << "Masked channels are: " << channels << " and that is all!";

  LogWarning("EcalSuperURecHitHists") << "---> Number of cosmic events: " << cosmicCounter_ << " in " << naiveEvtNum_ << " events.";

}


std::string EcalSuperURecHitHists::intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}

