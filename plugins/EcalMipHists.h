// -*- C++ -*-
//
// Package:   EcalMipHists 
// Class:     EcalMipHists 
// 
/**\class EcalMipHists EcalMipHists.cc

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
#include <set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDataFrame.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"

#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TProfile.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

class EcalMipHists : public edm::EDAnalyzer {
public:
	explicit EcalMipHists(const edm::ParameterSet&);
	~EcalMipHists();

private:
	virtual void beginJob(const edm::EventSetup&) ;
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;
	std::string intToString(int num);
	void makeTree();

// ----------member data ---------------------------

	edm::InputTag EBDigis_;
	edm::InputTag EBUncalibratedRecHitCollection_;
	edm::InputTag headerProducer_;
        const EcalElectronicsMapping* ecalElectronicsMap_;

	//histograms - for each quantity, one 2d map and one vector of crystal histogram 
	
	TProfile2D * prof2_XtalJitter_;
	TH1F  * v_h1_XtalJitter_[61200];

	TProfile2D * prof2_XtalAmpli_;
	TH1F  * v_h1_XtalAmpli_[61200];

	TProfile2D * prof2_XtalPed_;
	TH1F  * v_h1_XtalPed_[61200];

	//for jitter: vector of crystal profiles
        TProfile * v_prof_XtalPulse_[61200];

	edm::Service<TFileService> fs_;
	TFileDirectory XtalJitterDir_;
	TFileDirectory XtalAmpliDir_;
	TFileDirectory XtalPedDir_;
        TFileDirectory XtalPulseDir_;

	//other parameters
	int runNum_;
	int eventNum_;
};
