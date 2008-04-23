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

#include "CaloOnlineTools/EcalTools/plugins/EcalChannelAnalyzer.h"
#include "TCut.h"
#include "TTree.h"
#include "TFile.h"

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
EcalChannelAnalyzer::EcalChannelAnalyzer(const edm::ParameterSet& iConfig)
	:inputFileName_ (iConfig.getUntrackedParameter<string>("inputFileName",""))
{

if (inputFileName_=="") std::cout << "no fileName" << std::endl;

//TODO: try/catch 
fin_= TFile::Open(inputFileName_.c_str(),"READ");
if (fin_->IsZombie()) {
       cout << "Error opening file" << endl;
       exit(-1);
}

//TODO:name of tree from .cfg + try/catch 
t_=(TTree*)fin_->Get("xtal_tree");

//std::cout << "treename=" << t_->GetName() << std::endl;

t_->SetBranchAddress("ic", &ic, &b_ic);
t_->SetBranchAddress("ism", &ism, &b_ism);
t_->SetBranchAddress("ieta", &ieta, &b_ieta);
t_->SetBranchAddress("iphi", &iphi, &b_iphi);
t_->SetBranchAddress("hashedIndex", &hashedIndex, &b_hashedIndex);
t_->SetBranchAddress("ped_avg", &ped_avg, &b_ped_avg);
t_->SetBranchAddress("ped_rms", &ped_rms, &b_ped_rms);
t_->SetBranchAddress("ampli_avg", &ampli_avg, &b_ampli_avg);
t_->SetBranchAddress("ampli_rms", &ampli_rms, &b_ampli_rms);
t_->SetBranchAddress("jitter_avg", &jitter_avg, &b_jitter_avg);
t_->SetBranchAddress("jitter_rms", &jitter_rms, &b_jitter_rms);
t_->SetBranchAddress("ampli_fracBelowThreshold", &ampli_fracBelowThreshold, &b_ampli_fracBelowThreshold);
t_->SetBranchAddress("entries", &entries, &b_entries);

}
	
EcalChannelAnalyzer::~EcalChannelAnalyzer()
{
}

//
// member functions
//

// ------------ method called to for each event  ------------

void
EcalChannelAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
//do nothing!
}//end analyze


// ------------ method called once each job just before starting event loop  ------------
void 
EcalChannelAnalyzer::beginJob(const edm::EventSetup&)
{

TCut cut1 = "ped_avg > 225";
TEntryList * entryList;
TEventList * eventList;
entryList=getEntryListFromCut(cut1); 
eventList=getEventListFromCut(cut1); 

//something wrong with this!
std::cout << (long int)entryList->GetN() << std::endl;
//therefore, the loop doesn't work (like this)
/*for (int i=0; i< entryList->GetN(); i++) {

	t_->GetEntry(entryList->Next());
	std::cout << "ism=" << ism << " ic=" << ic << " ped_avg=" << ped_avg << std::endl;

}
*/

t_->SetEventList(eventList);
t_->Scan("ism:ic:ped_avg");

}



// ------------ method called once each job just after ending the event loop  ------------
void 
EcalChannelAnalyzer::endJob() 
{
std::cout << "closing file" << std::endl;
fin_->Close();
}


//-------------- "helper" methods -----------------
TEntryList *
EcalChannelAnalyzer::getEntryListFromCut(const TCut& cut) {

	t_->Draw(">>myList", cut, "entrylist");
	TEntryList *list = (TEntryList*)gDirectory->Get("myList");

	return list;

}


TEventList *
EcalChannelAnalyzer::getEventListFromCut(const TCut& cut) {

	t_->Draw(">>myList", cut);
	TEventList *list = (TEventList*)gDirectory->Get("myList");

	return list;

}


std::string EcalChannelAnalyzer::intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}


