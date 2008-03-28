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
#include "DataFormats/EcalRawData/interface/EcalDCCHeaderBlock.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "CaloOnlineTools/EcalTools/interface/EcalFedMap.h"

#include "TFile.h"
#include "TProfile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TNtuple.h"


//
// class declaration
//

class EcalMipGraphs : public edm::EDAnalyzer {
   public:
      explicit EcalMipGraphs(const edm::ParameterSet&);
      ~EcalMipGraphs();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int num);
      void writeGraphs();
      void initHists(int);

    // ----------member data ---------------------------

  edm::InputTag EcalUncalibratedRecHitCollection_;
  edm::InputTag EBDigis_;
  edm::InputTag headerProducer_;

  int runNum_;
  int side_;
  int givenSeedCry_;
  double threshold_;
  double thresholdNeighbor_;
  double thresholdSingle_;
  double thresholdLoose_;
  std::string fileName_;

  std::set<EBDetId> listAllChannels;
    
  int abscissa[10];
  int ordinate[10];
  
  std::vector<TGraph> graphs;
  std::vector<int> maskedChannels_;
  std::vector<int> maskedFEDs_;
  std::vector<std::string> maskedEBs_;
  std::map<int,TH1F*> FEDsAndTimingHists_;
  std::map<int,float> crysAndAmplitudesMap_;
  
  TH1F* allFedsTimingHist_;

  // histograms for cosmics analysis
  TH2F* barrelCosmicOccupancyTTHist_;
  TH2F* barrelCosmicOccupancyHist_;
 
  TH2F* barrelCosmicAmplitudeTTHist_;  
  TH2F* barrelCosmicAmplitudeHist_;

  TH2F* barrelCosmicAmplitudeEtaTTHist_;  
  TH2F* barrelCosmicAmplitudeEtaHist_;
  TH2F* barrelCosmicAmplitudePhiTTHist_;  
  TH2F* barrelCosmicAmplitudePhiHist_;

  TH2F* barrelCosmicE1vE8_;
  TH2F* barrelCosmicE1vE2_;
  TH2F* barrelCosmicE1vE9_;
  TH2F* barrelCosmicE1vE1E2_;


  int fedIds[54];
  std::map<int,TH2F*> fedIdOccupancyHistMap_;
  std::map<int,TH1F*> fedIdOccupancyEtaHistMap_;
  std::map<int,TH1F*> fedIdOccupancyPhiHistMap_;

  std::map<int,TH2F*> fedIdAmplitudeEtaHistMap_;
  std::map<int,TH2F*> fedIdAmplitudePhiHistMap_;

  //  std::map<int,TH1F*> FEDsAndTimingHistsCosmic_;
  TH1F* allFedsTimingHistCosmic_;

  TH2F* allFedsTimingPhiHist_;
  TH2F* allFedsTimingPhiHistCosmic_;

  TH2F* allFedsTimingPhiEbpHist_;
  TH2F* allFedsTimingPhiEbpHistCosmic_;
  TH2F* allFedsTimingPhiEbmHist_;
  TH2F* allFedsTimingPhiEbmHistCosmic_;
  //

  TH1F* recHitAmplitudeHist_;

  TFile* file_;
  TNtuple* eventsAndSeedCrys_;
  EcalFedMap* fedMap_;
 
  int cosmicCounter_;

  int naiveEvtNum_; 

};
