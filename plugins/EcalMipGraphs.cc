// -*- C++ -*-
//
// Package:   EcalMipGraphs 
// Class:     EcalMipGraphs 
// 
/**\class EcalMipGraphs EcalMipGraphs.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Th Nov 22 5:46:22 CEST 2007
// $Id$
//
//

#include "CaloOnlineTools/EcalTools/plugins/EcalMipGraphs.h"

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
EcalMipGraphs::EcalMipGraphs(const edm::ParameterSet& iConfig) :
  EcalUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection")),
  EBDigis_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
  headerProducer_ (iConfig.getParameter<edm::InputTag> ("headerProducer")),
  runNum_(-1),
  side_ (iConfig.getUntrackedParameter<int>("side", 3)),
  givenSeedCry_ (iConfig.getUntrackedParameter<int>("seedCry",0)),
  threshold_ (iConfig.getUntrackedParameter<double>("amplitudeThreshold", 12.0)),
  thresholdNeighbor_ (iConfig.getUntrackedParameter<double>("amplitudeThresholdNeighbor", 12.0)),
  thresholdSingle_ (iConfig.getUntrackedParameter<double>("amplitudeThresholdSingle", 20.0)), 
  thresholdLoose_ (iConfig.getUntrackedParameter<double>("amplitudeThresholdLoose", 7.0)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("ecalMipGraphs")))
{
  vector<int> listDefaults;
  listDefaults.push_back(-1);
  
  maskedChannels_ = iConfig.getUntrackedParameter<vector<int> >("maskedChannels", listDefaults);
  maskedFEDs_ = iConfig.getUntrackedParameter<vector<int> >("maskedFEDs", listDefaults);

  vector<string> defaultMaskedEBs;
  defaultMaskedEBs.push_back("none");
  maskedEBs_ =  iConfig.getUntrackedParameter<vector<string> >("maskedEBs",defaultMaskedEBs);
  
  fedMap_ = new EcalFedMap();

  // timing plots
  string title1 = "Jitter for all FEDs";
  string name1 = "JitterAllFEDs";
  allFedsTimingHist_ = new TH1F(name1.c_str(),title1.c_str(),14,-7,7);

  title1 = "Jitter vs Phi for all FEDs (TT binning)";
  name1 = "JitterPhiAllFEDs";
  allFedsTimingPhiHist_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "cosmicJitterAllFEDs";
  title1 = "Cosmic Jitter for all FEDs";
  allFedsTimingHistCosmic_ = new TH1F(name1.c_str(),title1.c_str(),14,-7,7);
  
  name1 = "cosmicJitterPhiAllFEDs";
  title1 = "Cosmic Jitter Phi for all FEDs (TT binning)";
  allFedsTimingPhiHistCosmic_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "JitterPhiEBP";
  title1 = "Jitter vs Phi for FEDs in EB+ (TT binning)";
  allFedsTimingPhiEbpHist_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "cosmicJitterPhiEBP";
  title1 = "Cosmic Jitter Phi for FEDs in EB+ (TT binning)";
  allFedsTimingPhiEbpHistCosmic_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "JitterPhiEBM";
  title1 = "Jitter vs Phi for FEDs in EB- (TT binning)";
  allFedsTimingPhiEbmHist_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "cosmicJitterPhiEBM";
  title1 = "Cosmic Jitter Phi for FEDs in EB- (TT binning)";
  allFedsTimingPhiEbmHistCosmic_ = new TH2F(name1.c_str(),title1.c_str(),72,-10,350,14,-7,7);

  name1 = "amplitudeRecHits";
  title1 = "Amplitude for all RecHits with ADC > 7";
  recHitAmplitudeHist_ = new TH1F(name1.c_str(),title1.c_str(),200,0,200);
  
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
  
  for (int i=0; i<10; i++)        abscissa[i] = i;
  naiveEvtNum_ = 0;

  cosmicCounter_ = 0;

  barrelCosmicOccupancyHist_ = new TH2F("barrelCosmicOccupancy","Cosmic Occupancy (xtal binning)",5*72,-10,350,5*34,-86,86);
  barrelCosmicOccupancyTTHist_ = new TH2F("barrelCosmicOccupancyTT","Cosmic Occupancy (TT binning)",72,-10,350,34,-86,86);

  barrelCosmicAmplitudeHist_ = new TH2F("barrelCosmicAmplitude","Cosmic Amplitude (xtal binning)",5*72,-10,350,5*34,-86,86);
  barrelCosmicAmplitudeTTHist_ = new TH2F("barrelCosmicAmplitudeTT","Cosmic Amplitude (TT binning)",72,-10,350,34,-86,86);

  barrelCosmicAmplitudeEtaHist_ = new TH2F("barrelCosmicAmplitudeEta","Cosmic Eta Amplitude (xtal binning)",5*34,-86,86,100,0,100);
  barrelCosmicAmplitudeEtaTTHist_ = new TH2F("barrelCosmicAmplitudeEtaTT","Cosmic Eta Amplitude (TT binning)",34,-86,86,100,0,100);

  barrelCosmicAmplitudePhiHist_ = new TH2F("barrelCosmicAmplitudePhi","Cosmic Phi Amplitude (xtal binning)",5*72,-10,350,100,0,100);
  barrelCosmicAmplitudePhiTTHist_ = new TH2F("barrelCosmicAmplitudePhiTT","Cosmic Phi Amplitude (TT binning)",5*72,-10,350,100,0,100);

  barrelCosmicE1vE8_ = new TH2F("barrelCosmicE1vE8","Cosmic E1vE8",50,0,50,50,0,50);
  barrelCosmicE1vE2_ = new TH2F("barrelCosmicE1vE2","Cosmic E1vE2",50,0,50,50,0,50);
  barrelCosmicE1vE9_ = new TH2F("barrelCosmicE1vE9","Cosmic E1vE9",50,0,50,50,0,50);
  barrelCosmicE1vE1E2_ = new TH2F("barrelCosmicE1vE1E2","Cosmic E1vE1E2",50,0,50,50,0,50);

}


EcalMipGraphs::~EcalMipGraphs()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalMipGraphs::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // to count cosmic events
  bool hasGoodCosmic_ = false;

  // get the headers
  // (one header for each supermodule)
  edm::Handle<EcalRawDataCollection> DCCHeaders;
  iEvent.getByLabel(headerProducer_, DCCHeaders);
  map<int,EcalDCCHeaderBlock> FEDsAndDCCHeaders_;

  for (EcalRawDataCollection::const_iterator headerItr= DCCHeaders->begin();
		  headerItr != DCCHeaders->end (); 
		  ++headerItr) 
  {
    FEDsAndDCCHeaders_[headerItr->id()+600] = *headerItr;
  }

  int ievt = iEvent.id().event();
  int graphCount = 0;
  
  naiveEvtNum_++;

  if(runNum_==-1)
  {
    runNum_ = iEvent.id().run();
    fileName_+=intToString(runNum_);
    fileName_+=".graph.root";
    file_ = TFile::Open(fileName_.c_str(),"RECREATE");
    TH1::AddDirectory(false);
    eventsAndSeedCrys_ = new TNtuple("eventsSeedCrys","Events and Seed Crys Mapping","LV1A:ic:fed");
  }

  //We only want the 3x3's for this event...
  listAllChannels.clear();
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);
  Handle<EcalUncalibratedRecHitCollection> hits;

  ESHandle<CaloTopology> caloTopo;
  iSetup.get<CaloTopologyRecord>().get(caloTopo);
  
  //TODO: improve try/catch behavior
  try
  {
    iEvent.getByLabel(EcalUncalibratedRecHitCollection_, hits);
    int neh = hits->size();
    LogDebug("EcalMipGraphs") << "event " << ievt << " hits collection size " << neh;
  }
  catch ( exception& ex)
  {
    LogWarning("EcalMipGraphs") << EcalUncalibratedRecHitCollection_ << " not available";
  }

  for (EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr)
  {
    EcalUncalibratedRecHit hit = (*hitItr);
    int ic = 0;
    int hashedIndex= 0;
    //EEDetId eeDet;
    //cout << "Subdetector field is: " << hit.id().subdetId() << endl;
    EBDetId ebDet = hit.id();
    //TODO: make it work for endcap FEDs also
    //if(ebDet.isValid())
    //{
    ic = ebDet.ic();
    hashedIndex = ebDet.hashedIndex();
    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(ebDet);
    //}
    //else
    //{
    //  eeDet = hit.id();
    //  if(!eeDet.isValid())
    //  {
    //    LogWarning("EcalMipGraphs") << "Unable to find hashedIndex for hit in event " << ievt_;
    //    continue;
    //  }
    //  else
    //  {
    //    ic = eeDet.hashedIndex();
    //    elecId = ecalElectronicsMap->getElectronicsId(eeDet);
    //  }
    //}
    
    int FEDid = 600+elecId.dccId();

    float ampli = hit.amplitude();
    float jitter = hit.jitter();

    //// BE SURE TO REMOVE THIS!!!!!! TOYOKO
    //    if (ampli < 40) continue;

    if (ampli > 5) recHitAmplitudeHist_->Fill(ampli);

    vector<int>::iterator result;
    result = find(maskedFEDs_.begin(), maskedFEDs_.end(), FEDid);
    if(result != maskedFEDs_.end())
    {
      LogWarning("EcalMipGraphs") << "skipping uncalRecHit for FED " << FEDid << " ; amplitude " << ampli;
      continue;
    }      

    result = find(maskedChannels_.begin(), maskedChannels_.end(), hashedIndex);
    if  (result != maskedChannels_.end())
    {
      LogWarning("EcalMipGraphs") << "skipping uncalRecHit for channel: " << ic << " in fed: " << FEDid << " with amplitude " << ampli ;
      continue;
    } 
    

    if(fedIds[FEDid-601]==0)
      {
	// occupancy plots by SM
	string name = "cosmicOccup_"+intToString(FEDid);
	string title = "Cosmic Occupancy for "+fedMap_->getSliceFromFed(FEDid);
	fedIdOccupancyHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),85,1,86,20,1,21);

	name = "cosmicOccupEta_"+intToString(FEDid);
	title = "Cosmic Occupancy Eta Profile for "+fedMap_->getSliceFromFed(FEDid);
	fedIdOccupancyEtaHistMap_[FEDid] = new TH1F(name.c_str(),title.c_str(),85,1,86);

	name = "cosmicOccupPhi_"+intToString(FEDid);
	title = "Cosmic Occupancy Phi Profile for "+fedMap_->getSliceFromFed(FEDid);
	fedIdOccupancyPhiHistMap_[FEDid] = new TH1F(name.c_str(),title.c_str(),20,1,21);
	
	// amplitude plots by SM
	name = "cosmicAmpEta_"+intToString(FEDid);
	title = "Cosmic Eta Amplitude for "+fedMap_->getSliceFromFed(FEDid);
	fedIdAmplitudeEtaHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),100,0,100,85,1,86);

	name = "cosmicAmpPhi_"+intToString(FEDid);
	title = "Cosmic Phi Amplitude for "+fedMap_->getSliceFromFed(FEDid);
	fedIdAmplitudePhiHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),100,0,100,20,1,21);

	fedIds[FEDid-601]++;

      }

    // E1vE8 plotted using lower threshold
    if( !givenSeedCry_ && ampli > thresholdLoose_ )
      {
	float E8 = 0.0;       
	float E2 = -99999.;
	
	vector<DetId> neighbors = caloTopo->getWindow(ebDet,3,3);
	
	for(vector<DetId>::const_iterator itr = neighbors.begin(); itr != neighbors.end(); ++itr)
	  {
	    vector<EcalUncalibratedRecHit>::const_iterator recItr;
	    recItr = hits->find(*itr);
	    if(recItr==hits->end())
	      continue;
	    if (ebDet!= *itr) {
	      E8 += recItr->amplitude();
	      if (recItr->amplitude() > E2 ) E2 = recItr->amplitude();
	      LogWarning("EcalMipGraphs") <<  intToString(naiveEvtNum_) 
					  << " E1 " << ampli << " : E2 " << E2 << " : " << recItr->amplitude()<< " : E8 " << E8 << " : " << thresholdLoose_ << endl;
	    }
	  }
	barrelCosmicE1vE8_->Fill(ampli,E8);
	barrelCosmicE1vE2_->Fill(ampli,E2);	    
	barrelCosmicE1vE9_->Fill(ampli,E8+ampli);
	barrelCosmicE1vE1E2_->Fill(ampli,E2+ampli);	    
      }
    // end of E1 and E8
    
    if(ampli > threshold_ && !givenSeedCry_)
    {
      // only produce output if no seed cry is given by user and amplitude makes cut
      LogWarning("EcalMipGraphs") << "channel: " << ic <<  " in fed: " << FEDid <<  "  ampli: " << ampli << " jitter " << jitter
        << " Event: " << ievt;
    }
    
    if(hashedIndex == givenSeedCry_ || (!givenSeedCry_ && ampli > threshold_))
    {
      float E8 = 0.0;
      if(!givenSeedCry_)
      {	
	bool hasGoodNeighbor = false;
	vector<DetId> neighbors = caloTopo->getWindow(ebDet,side_,side_); // what if side!=3 ?
	for(vector<DetId>::const_iterator itr = neighbors.begin(); itr != neighbors.end(); ++itr)
	  {
	    vector<EcalUncalibratedRecHit>::const_iterator recItr;
	    recItr = hits->find(*itr);
	    if(recItr==hits->end())
	      continue;
	    if (ebDet!= *itr) E8 += recItr->amplitude();
	    if(recItr->amplitude() > thresholdNeighbor_ && ebDet != *itr) {
	      hasGoodNeighbor = true;	      
	    }
	  }	  
	// if amplitude is above thresholdSingle then no need to check neighbors
	if (ampli < thresholdSingle_) {
	  if(!hasGoodNeighbor)
	    continue;
	}
      }
      
      hasGoodCosmic_ = true;

      cout << endl;
      LogWarning("EcalMipGraphs") << "hit! " << intToString(naiveEvtNum_) << " amp " << ampli << " : E8 " << E8 << " : " << fedMap_->getSliceFromFed(FEDid) << " : ic " <<  ebDet.ic() << " : ieta " << ebDet.ieta() << " iphi " << ebDet.iphi() << " : ietaSM " << ebDet.ietaSM() << " iphiSM " << ebDet.iphiSM() << " : hashedIndex " << ebDet.hashedIndex() << " : nCosmics " << hasGoodCosmic_ << " " << cosmicCounter_ << endl;      

      fedIdOccupancyHistMap_[FEDid]->Fill(ebDet.ietaSM(),ebDet.iphiSM());
      fedIdOccupancyEtaHistMap_[FEDid]->Fill(ebDet.ietaSM());
      fedIdOccupancyPhiHistMap_[FEDid]->Fill(ebDet.iphiSM());

      fedIdAmplitudeEtaHistMap_[FEDid]->Fill(ebDet.ietaSM(),ampli);
      fedIdAmplitudePhiHistMap_[FEDid]->Fill(ebDet.iphiSM(),ampli);

      barrelCosmicOccupancyHist_->Fill(ebDet.iphi()-11,ebDet.ieta());
      barrelCosmicAmplitudeHist_->Fill(ebDet.iphi()-11,ebDet.ieta(),ampli);

      barrelCosmicOccupancyTTHist_->Fill(ebDet.iphi()-11,ebDet.ieta());
      barrelCosmicAmplitudeTTHist_->Fill(ebDet.iphi()-11,ebDet.ieta(),ampli);

      barrelCosmicAmplitudeEtaHist_->Fill(ebDet.ieta(),ampli);
      barrelCosmicAmplitudeEtaTTHist_->Fill(ebDet.ieta(),ampli);

      barrelCosmicAmplitudePhiHist_->Fill(ebDet.iphi()-11,ampli);
      barrelCosmicAmplitudePhiTTHist_->Fill(ebDet.iphi()-11,ampli);

      allFedsTimingHistCosmic_->Fill(hit.jitter());
      allFedsTimingPhiHistCosmic_->Fill(ebDet.iphi()-11,hit.jitter());
      if (FEDid>=610&&FEDid<=627)  allFedsTimingPhiEbmHistCosmic_->Fill(ebDet.iphi()-11,hit.jitter());
      if (FEDid>=628&&FEDid<=645)  allFedsTimingPhiEbpHistCosmic_->Fill(ebDet.iphi()-11,hit.jitter());

      eventsAndSeedCrys_->Fill(naiveEvtNum_, ic, FEDid);
      crysAndAmplitudesMap_[hashedIndex] = ampli;
      vector<DetId> neighbors = caloTopo->getWindow(ebDet,side_,side_);
      for(vector<DetId>::const_iterator itr = neighbors.begin(); itr != neighbors.end(); ++itr)
      {
        listAllChannels.insert(*itr);
      }
    }
   

    TH1F* timingHist = FEDsAndTimingHists_[FEDid];
    if(timingHist==0)
    {
      initHists(FEDid);
      timingHist = FEDsAndTimingHists_[FEDid];
    }
    
    timingHist->Fill(hit.jitter());
    allFedsTimingHist_->Fill(hit.jitter());
    allFedsTimingPhiHist_->Fill(ebDet.iphi()-11,hit.jitter());
    if (FEDid>=610&&FEDid<=627)  allFedsTimingPhiEbmHist_->Fill(ebDet.iphi()-11,hit.jitter());
    if (FEDid>=628&&FEDid<=645)  allFedsTimingPhiEbpHist_->Fill(ebDet.iphi()-11,hit.jitter());
  }

  // retrieving crystal digi from Event
  edm::Handle<EBDigiCollection>  digis;
  iEvent.getByLabel(EBDigis_, digis);

  for(std::set<EBDetId>::const_iterator chnlItr = listAllChannels.begin(); chnlItr!= listAllChannels.end(); ++chnlItr)
  {
      //find digi we need  -- can't get find() to work; need DataFrame(DetId det) to work? 
      //TODO: use find(), launching it twice over EB and EE collections

    int ic = (*chnlItr).ic();
    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(*chnlItr);
    int FEDid = 600+elecId.dccId();
    string sliceName = fedMap_->getSliceFromFed(FEDid);
    int hashedIndex = (*chnlItr).hashedIndex();
    EBDigiCollection::const_iterator digiItr = digis->begin();
    
    while(digiItr != digis->end() && ((*digiItr).id()!=*chnlItr))
    {
      ++digiItr;
    }
    if(digiItr==digis->end())
    {
      LogWarning("EcalMipGraphs") << "Cannot find digi for ic:" << ic
        << " FED:" << FEDid << " evt:" << naiveEvtNum_;
      continue;
    }
    
    //EBDataFrame df = (*digis)[hashedIndex];
    
    //cout << "the detId is: " << (*chnlItr).rawId() << endl;
    //cout << "the detId found: " << df.id().rawId() << endl;
    
    
    int gainId = FEDsAndDCCHeaders_[FEDid].getMgpaGain();
    int gainHuman;
    if      (gainId ==1) gainHuman =12;
    else if (gainId ==2) gainHuman =6;
    else if (gainId ==3) gainHuman =1;
    else                 gainHuman =-1; 

    int sample0GainId = EBDataFrame(*digiItr).sample(0).gainId();
    for (int i=0; (unsigned int)i< (*digiItr).size() ; ++i ) {
      EBDataFrame df(*digiItr); 
      ordinate[i] = df.sample(i).adc(); // accounf for possible gain !=12?
      if(df.sample(i).gainId()!=sample0GainId)
        LogWarning("EcalMipGraphs") << "Gain switch detected in evt:" <<
	  //          naiveEvtNum_ << " sample:" << i << " ic:" << ic << " FED:" << FEDid;
          naiveEvtNum_ << " sample:" << i << " ic:" << ic << " hashedIndex: " << hashedIndex << " FED:" << FEDid << " gainId: " << df.sample(i).gainId() << " adc: " << df.sample(i).adc() << " sample 0: " << df.sample(0).gainId() << " " << df.sample(0).adc() << endl;
    }

    TGraph oneGraph(10, abscissa,ordinate);
    string name = "Graph_ev" + intToString(naiveEvtNum_) + "_ic" + intToString(ic)
      + "_FED" + intToString(FEDid);
    string gainString = (gainId==1) ? "Free" : intToString(gainHuman);
    string title = "Event" + intToString(naiveEvtNum_) + "_lv1a" + intToString(ievt) +
      "_ic" + intToString(ic) + "_" + sliceName + "_gain" + gainString;
    map<int,float>::const_iterator itr;
    itr = crysAndAmplitudesMap_.find(hashedIndex);
    if(itr!=crysAndAmplitudesMap_.end())
      title+="_Amp"+intToString((int)itr->second);
    
    oneGraph.SetTitle(title.c_str());
    oneGraph.SetName(name.c_str());
    graphs.push_back(oneGraph);
    graphCount++;
  }
  
  if(graphs.size()==0)
    return;
  
  writeGraphs();

  if (hasGoodCosmic_) cosmicCounter_++;

}

void EcalMipGraphs::writeGraphs()
{
  int graphCount = 0;
  file_->cd();
  std::vector<TGraph>::iterator gr_it;
  for (gr_it = graphs.begin(); gr_it !=  graphs.end(); gr_it++ )
  {
    graphCount++;
    if(graphCount % 100 == 0)
      LogInfo("EcalMipGraphs") << "Writing out graph " << graphCount << " of "
        << graphs.size(); 

    (*gr_it).Write(); 
  }
  
  graphs.clear();
}
  


// insert the hist map into the map keyed by FED number
void EcalMipGraphs::initHists(int FED)
{
  using namespace std;
  
  string title1 = "Jitter for ";
  title1.append(fedMap_->getSliceFromFed(FED));
  string name1 = "JitterFED";
  name1.append(intToString(FED));
  TH1F* timingHist = new TH1F(name1.c_str(),title1.c_str(),14,-7,7);
  FEDsAndTimingHists_[FED] = timingHist;
  FEDsAndTimingHists_[FED]->SetDirectory(0);
}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalMipGraphs::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalMipGraphs::endJob()
{
  writeGraphs();
  eventsAndSeedCrys_->Write();
  barrelCosmicE1vE8_->Write();
  barrelCosmicE1vE2_->Write();
  barrelCosmicE1vE9_->Write();
  barrelCosmicE1vE1E2_->Write();
  barrelCosmicOccupancyHist_->Write();
  barrelCosmicAmplitudeHist_->Write();
  barrelCosmicOccupancyTTHist_->Write();
  barrelCosmicAmplitudeTTHist_->Write();
  barrelCosmicAmplitudeEtaTTHist_->Write();
  barrelCosmicAmplitudeEtaHist_->Write();
  barrelCosmicAmplitudePhiTTHist_->Write();
  barrelCosmicAmplitudePhiHist_->Write();

  for(map<int,TH2F*>::const_iterator itr = fedIdOccupancyHistMap_.begin();
      itr != fedIdOccupancyHistMap_.end(); ++itr)
  {
    itr->second->Write();
    TProfile* tx = (TProfile*) (itr->second->ProfileX());
    tx->Write();    
    TProfile* ty = (TProfile*) (itr->second->ProfileY());
    ty->Write();    
  }

  for(map<int,TH1F*>::const_iterator itr = fedIdOccupancyEtaHistMap_.begin();
      itr != fedIdOccupancyEtaHistMap_.end(); ++itr)
  {
    itr->second->Write();
  }

  for(map<int,TH1F*>::const_iterator itr = fedIdOccupancyPhiHistMap_.begin();
      itr != fedIdOccupancyPhiHistMap_.end(); ++itr)
  {
    itr->second->Write();
  }


  for(map<int,TH2F*>::const_iterator itr = fedIdAmplitudeEtaHistMap_.begin();
      itr != fedIdAmplitudeEtaHistMap_.end(); ++itr)
  {
    itr->second->Write();
    TProfile* tx = (TProfile*) (itr->second->ProfileX());
    tx->Write();    
    TProfile* ty = (TProfile*) (itr->second->ProfileY());
    ty->Write();    
  }

  for(map<int,TH2F*>::const_iterator itr = fedIdAmplitudePhiHistMap_.begin();
      itr != fedIdAmplitudePhiHistMap_.end(); ++itr)
  {
    itr->second->Write();
    TProfile* tx = (TProfile*) (itr->second->ProfileX());
    tx->Write();    
    TProfile* ty = (TProfile*) (itr->second->ProfileY());
    ty->Write();    
  }
  allFedsTimingHistCosmic_->Write();
  allFedsTimingPhiHistCosmic_->Write();
  allFedsTimingPhiEbpHistCosmic_->Write();
  allFedsTimingPhiEbmHistCosmic_->Write();

  for(map<int,TH1F*>::const_iterator itr = FEDsAndTimingHists_.begin();
      itr != FEDsAndTimingHists_.end(); ++itr)
  {
    TH1F* hist = itr->second;
    if(hist!=0)
      hist->Write();
    else
    {
      cerr << "EcalMipGraphs: Error: This shouldn't happen!" << endl;
    }
  }
  allFedsTimingHist_->Write();
  allFedsTimingPhiHist_->Write();
  allFedsTimingPhiEbmHist_->Write();
  allFedsTimingPhiEbpHist_->Write();

  recHitAmplitudeHist_->Write();

  file_->Close();
  std::string channels;
  for(std::vector<int>::const_iterator itr = maskedChannels_.begin();
      itr != maskedChannels_.end(); ++itr)
  {
    channels+=intToString(*itr);
    channels+=",";
  }
  
  LogWarning("EcalMipGraphs") << "Masked channels are: " << channels << " and that is all!";

  LogWarning("EcalMipGraphs") << "Number of cosmic events: " << cosmicCounter_;

}


std::string EcalMipGraphs::intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}
