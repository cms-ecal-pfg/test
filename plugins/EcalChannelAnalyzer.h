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
// Original Author:  Caterina DOGLIONI
//         Created:  Th Nov 22 5:46:22 CEST 2007
// $Id$
//
//

// system include files
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <string>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TEventList.h"
#include "TCut.h"

//
// class declaration
//

class EcalChannelAnalyzer : public edm::EDAnalyzer {
public:
	explicit EcalChannelAnalyzer(const edm::ParameterSet&);
	~EcalChannelAnalyzer();

private:

	//enum for graph type
	enum n_h1Type { H1_AMPLI=0 , H1_PED , H1_JITTER, PROF_PULSE , NTYPES}; 


	virtual void beginJob(const edm::EventSetup&) ;
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;

	//helpers
	TEventList * getEventListFromCut (const TCut& cut);
	std::string intToString(int num);
	std::string printBitmask(std::vector<bool> * bitmask);
	std::string printBitmaskCuts(std::vector<bool> * bitmask);
	void initHistTypeMaps();
	void fillEventListVector(const std::vector<std::string>& v_cuts);   	
	void writeHistFromFile(int hashedIndex, int ism, int ic, n_h1Type H1TYPE); 

// ----------member data --------------------------

//tree
TTree * t_;
//filename for input (where the TTree will be retrieved)
std::string inputTreeFileName_;
std::string inputHistoFileName_;
std::string outputFileName_;

std::vector<std::string> v_cuts_;

TFile * fin_tree_;
TFile * fin_histo_;
TFile * fout_;

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

//map for directories:hist type
std::map < EcalChannelAnalyzer::n_h1Type , std::string > h1TypeToDirectoryMap_;

//map for hist name:hist type
std::map < EcalChannelAnalyzer::n_h1Type , std::string > h1TypeToNameMap_;

//vector for TEventLists (from TCuts)
std::vector <TEventList> v_eventList_;

//total TEventList (no duplicate crystals)
TEventList totalEventList_;

//vector for bitmasks (which cuts?)
std::vector<bool> * xtalBitmask_[61200];

//number of cuts
int nCuts_;

};
