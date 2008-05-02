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
//
// Original Author:  Seth COOPER
//         Created:  Th Nov 22 5:46:22 CEST 2007
// $Id$
//
//


// system include files
#include <memory>
#include <vector>
#include <map>
#include <set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"

#include "CaloOnlineTools/EcalTools/interface/EcalFedMap.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TNtuple.h"


//
// class declaration
//

class EcalCosmicsHists : public edm::EDAnalyzer {
   public:
      explicit EcalCosmicsHists(const edm::ParameterSet&);
      ~EcalCosmicsHists();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int num);
      void initHists(int);

    // ----------member data ---------------------------

  edm::InputTag EcalUncalibratedRecHitCollection_;
  int runNum_;
  double histRangeMax_, histRangeMin_;
  double minSeedAmp_;
  double minTimingAmp_;
  std::string fileName_;
  
  std::vector<int> maskedChannels_;
  std::vector<int> maskedFEDs_;
  std::vector<std::string> maskedEBs_;
  std::map<int,TH1F*> FEDsAndHists_;
  std::map<int,TH1F*> FEDsAndE2Hists_;
  std::map<int,TH1F*> FEDsAndE9Hists_;
  std::map<int,TH1F*> FEDsAndTimingHists_;
  std::map<int,TH1F*> FEDsAndFrequencyHists_;
  std::map<int,TH1F*> FEDsAndiPhiProfileHists_;
  std::map<int,TH1F*> FEDsAndNumXtalsInE9Hists_;
  std::map<int,TH1F*> FEDsAndiEtaProfileHists_;
  std::map<int,TH2F*> FEDsAndTimingVsFreqHists_;
  std::map<int,TH2F*> FEDsAndTimingVsAmpHists_;
  std::map<int,TH2F*> FEDsAndE2vsE1Hists_;
  std::map<int,TH2F*> FEDsAndE9vsE1Hists_;
  std::map<int,TH2F*> FEDsAndOccupancyHists_;  
  std::map<int,TH2F*> FEDsAndTimingVsPhiHists_;  
  std::map<int,TH2F*> FEDsAndTimingVsModuleHists_;  

  TH1F* allFedsHist_;
  TH1F* allFedsE2Hist_;
  TH1F* allFedsE9Hist_;
  TH1F* allFedsTimingHist_;
  TH1F* allFedsFrequencyHist_;
  TH1F* allFedsiPhiProfileHist_;
  TH1F* allFedsiEtaProfileHist_;
  TH1F* allFedsNumXtalsInE9Hist_;
  TH1F* numberofCosmicsHist_;
  TH1F* numberofGoodEvtFreq_;

  TH2F* allFedsE2vsE1Hist_;
  TH2F* allFedsE9vsE1Hist_;
  TH2F* allOccupancy_; //New file to do eta-phi occupancy
  TH2F* allOccupancyCoarse_; //New file to do eta-phi occupancy
  TH2F* allFedsTimingVsFreqHist_;
  TH2F* allFedsTimingVsAmpHist_;
  TH2F* allFedsTimingPhiHist_;
  TH2F* allFedsTimingPhiEbpHist_;
  TH2F* allFedsTimingPhiEbmHist_;
  TH3F* allFedsTimingPhiEtaHist_;


  TFile* file;
  EcalFedMap* fedMap_;

  double minCosmicE1_;
  double minCosmicE2_;
  
  int naiveEvtNum_; 
  int cosmicCounter_;
};
