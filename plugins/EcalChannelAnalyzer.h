// -*- C++ -*-
//
// Package:   EcalChannelAnalyzer 
// Class:     EcalChannelAnalyzer 
// 
/**\class EcalChannelAnalyzer EcalChannelAnalyzer.cc

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

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDataFrame.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TProfile.h"
#include "TEntryList.h"
#include "TCut.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

class EcalChannelAnalyzer : public edm::EDAnalyzer {
public:
	explicit EcalChannelAnalyzer(const edm::ParameterSet&);
	~EcalChannelAnalyzer();

private:
	virtual void beginJob(const edm::EventSetup&) ;
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;

	TEntryList * getEntryListFromCut (const TCut& cut);
	TEventList * getEventListFromCut (const TCut& cut);
	std::string intToString(int num);

// ----------member data --------------------------

//tree
TTree * t_;
//filename for input (where the TTree will be retrieved)
std::string inputFileName_;
TFile * fin_;

//tree variables

int	ic;
int     ism;
int     ieta;
int     iphi;
int     hashedIndex;
float   ped_avg;
float   ped_rms;
float   ampli_avg;
float   ampli_rms;
float   jitter_avg;
float   jitter_rms;
float   ampli_fracBelowThreshold;
int     entries;

//branches

TBranch        *b_ic;   //!
TBranch        *b_ism;   //!
TBranch        *b_ieta;   //!
TBranch        *b_iphi;   //!
TBranch        *b_hashedIndex;   //!
TBranch        *b_ped_avg;   //!
TBranch        *b_ped_rms;   //!
TBranch        *b_ampli_avg;   //!
TBranch        *b_ampli_rms;   //!
TBranch        *b_jitter_avg;   //!
TBranch        *b_jitter_rms;   //!
TBranch        *b_ampli_fracBelowThreshold;   //!
TBranch        *b_entries;   //!
 
};
