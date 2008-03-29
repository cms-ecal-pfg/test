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
#include "TGraph.h"
#include "TNtuple.h"


//
// class declaration
//

class EcalSuperURecHitHists : public edm::EDAnalyzer {
   public:
      explicit EcalSuperURecHitHists(const edm::ParameterSet&);
      ~EcalSuperURecHitHists();


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

  TH1F* allFedsHist_;
  TH1F* allFedsE2Hist_;
  TH1F* allFedsE9Hist_;
  TH1F* allFedsTimingHist_;
  TH1F* allFedsFrequencyHist_;
  TH1F* allFedsiPhiProfileHist_;
  TH1F* allFedsiEtaProfileHist_;
  TH1F* allFedsNumXtalsInE9Hist_;

  TH2F* allFedsE2vsE1Hist_;
  TH2F* allFedsE9vsE1Hist_;
  TH2F* allOccupancy_; //New file to do eta-phi occupancy
  TH2F* allOccupancyCoarse_; //New file to do eta-phi occupancy
  TH2F* allFedsTimingVsFreqHist_;
  TH2F* allFedsTimingVsAmpHist_;

  TFile* file;
  EcalFedMap* fedMap_;

  //  int cosmicCounter_;

  double thresholdCosmic_;
  double thresholdNeighborCosmic_;
  double thresholdSingleCosmic_;

  TH2F* allFedsE1vsE8Hist_;
  TH2F* allFedsE1vsE2mE1Hist_;

  TH1F* allFedsTimingHistCosmic_;
  
  TH2F* allFedsTimingPhiHist_;
  TH2F* allFedsTimingPhiHistCosmic_;
 
  TH2F* allFedsTimingPhiEbpHist_;
  TH2F* allFedsTimingPhiEbpHistCosmic_;

  TH2F* allFedsTimingPhiEbmHist_;
  TH2F* allFedsTimingPhiEbmHistCosmic_;

  TH2F* cosmicOccupancyTTHist_;
  TH2F* cosmicOccupancyHist_;
 
  TH2F* cosmicAmplitudeTTHist_;  
  TH2F* cosmicAmplitudeHist_;

  TH2F* cosmicAmplitudeEtaTTHist_;  
  TH2F* cosmicAmplitudeEtaHist_;
  TH2F* cosmicAmplitudePhiTTHist_;  
  TH2F* cosmicAmplitudePhiHist_;

  std::map<int,TH2F*> FEDsAndOccupancyHists_;  
  std::map<int,TH2F*> FEDsAndAmplitudeVsEtaHists_; 
  std::map<int,TH2F*> FEDsAndAmplitudeVsPhiHists_; 

  // cosmics 
  std::map<int,TH1F*> cosmicFEDsAndTimingHists_;
  std::map<int,TH2F*> cosmicFEDsAndOccupancyHists_;  
  std::map<int,TH2F*> cosmicFEDsAndAmplitudeVsEtaHists_; 
  std::map<int,TH2F*> cosmicFEDsAndAmplitudeVsPhiHists_; 

  int naiveEvtNum_; 
  int cosmicCounter_;
};
