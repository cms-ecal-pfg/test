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
// Original Author:  Caterina DOGLIONI
//         Created:  Tu Apr 22 5:46:22 CEST 2008
// $Id$
//
//

// system include files

#include "CaloOnlineTools/EcalTools/plugins/EcalChannelAnalyzer.h"
#include "TCut.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"

#define MAX_XTALS 61200

using namespace edm;

//
// constructors and destructor
//
EcalChannelAnalyzer::EcalChannelAnalyzer(const edm::ParameterSet& iConfig)
	:inputTreeFileName_ (iConfig.getUntrackedParameter<std::string>("inputTreeFileName","")),
	inputHistoFileName_(iConfig.getUntrackedParameter<std::string>("inputHistoFileName","")),
	outputFileName_(iConfig.getUntrackedParameter<std::string>("outputFileName","")),
	v_cuts_ (iConfig.getUntrackedParameter< std::vector<std::string> >  ("v_cuts") )

{
	//--- INITIALIZATIONS 
	nCuts_=v_cuts_.size();
	initHistTypeMaps();

	//--- CLEANING OF VECTORS
	for (int i=0; i < MAX_XTALS; i++) {	
		xtalBitmask_[i] = 0;
	}

	//--- OPENING FILES

	//TODO: try/catch 
	fin_tree_= TFile::Open(inputTreeFileName_.c_str(),"READ");
	if (fin_tree_->IsZombie()) {
		std::cout << "Error opening tree file" << std::endl;
		exit(-1);
	}

	//TODO: try/catch 
	fin_histo_= TFile::Open(inputHistoFileName_.c_str(),"READ");
	if (fin_histo_->IsZombie()) {
		std::cout << "Error opening histo file" << std::endl;
		exit(-1);
	}

	//TODO: try/catch
	fout_=TFile::Open(outputFileName_.c_str(),"RECREATE");
	if (fout_->IsZombie()) {
		std::cout << "Error opening output file" << std::endl;
		exit(-1);
	}

	//--- INIT TREE

	//TODO:name of tree from .cfg + try/catch 
	t_=(TTree*)fin_tree_->Get("xtal_tree");

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

// ------------ method called for each event  ------------

	void
EcalChannelAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	//do nothing!
}//end analyze


// ------------ method called once each job just before starting event loop  ------------
	void 
EcalChannelAnalyzer::beginJob(const edm::EventSetup&)
{

	//filling event list vector with lists produced by cuts
	//by construction, list made from cut in v_cuts_[i] is in v_eventList_[i] 
	fillEventListVector(v_cuts_);

	//loop on all event lists 
	for (unsigned int i=0; i<v_eventList_.size(); i++){
		//loop on events in event lists
		for (unsigned int j=0; j<(unsigned int)v_eventList_[i].GetN(); j++) {

			t_->GetEntry(v_eventList_[i].GetEntry(j));

			//if bitmask hasn't been allocated for selected crystal, allocate it
			if (!xtalBitmask_[hashedIndex]) xtalBitmask_[hashedIndex] = new std::vector<bool> (v_cuts_.size());
			//flagging the cut relative to the entryList
			xtalBitmask_[hashedIndex]->operator[](i)=1;

		}
		//adding current event list to total event list (removing duplicate crystals)
		totalEventList_.Add(&v_eventList_[i]); 

	}

	//loop on events in event lists
	for (unsigned int j=0; j<(unsigned int)totalEventList_.GetN(); j++) {

		t_->GetEntry(totalEventList_.GetEntry(j));
		//dirty trick with enum + implicit converter 
		for (unsigned int i=0; i<EcalChannelAnalyzer::NTYPES; i++) {
			writeHistFromFile(hashedIndex, ism, ic, (EcalChannelAnalyzer::n_h1Type)i);
		} 

	}

}



// ------------ method called once each job just after ending the event loop  ------------
	void 
EcalChannelAnalyzer::endJob() 
{

	//printouts

	std::cout << "------ORDERED LIST OF CUTS-----" << std::endl;
	
	for (unsigned int i=0; i<v_eventList_.size(); ++i){
		std::cout << i << ". " << v_eventList_[i].GetTitle() << std::endl; 
	}

	std::cout << "------LISTS FOR SINGLE CUTS-----" << std::endl;

	for (unsigned int i=0; i<v_eventList_.size(); i++){
		std::cout << v_eventList_[i].GetTitle() << std::endl; 
		t_->SetEventList(&v_eventList_[i]);
		t_->Scan();
	}

	std::cout << "------ALL CRYSTALS-----" << std::endl;

	t_->SetEventList(&totalEventList_);
	t_->Scan();

	//cleanup
	for (unsigned int i=0; i<MAX_XTALS; i++) {
		if (xtalBitmask_[i]!=0)  delete xtalBitmask_[i];
	}

	fin_tree_->Close();
	fin_histo_->Close();
	fout_->Close();
}


//-------------- "helper" methods -----------------
TEventList *
EcalChannelAnalyzer::getEventListFromCut(const TCut& cut) {

	t_->Draw(">>List", cut);
	TEventList *list = (TEventList*)gDirectory->Get("List");

	return list;

}

void EcalChannelAnalyzer::initHistTypeMaps() {

	//FIXME: change this when name of 1st analyzer is changed
	h1TypeToDirectoryMap_[EcalChannelAnalyzer::H1_AMPLI]="ecalMipHists/XtalAmpli";
	h1TypeToDirectoryMap_[EcalChannelAnalyzer::H1_PED]="ecalMipHists/XtalPed";
	h1TypeToDirectoryMap_[EcalChannelAnalyzer::H1_JITTER]="ecalMipHists/XtalJitter";
	h1TypeToDirectoryMap_[EcalChannelAnalyzer::PROF_PULSE]="ecalMipHists/XtalPulse";

	h1TypeToNameMap_[EcalChannelAnalyzer::H1_AMPLI]="ampli";
	h1TypeToNameMap_[EcalChannelAnalyzer::H1_PED]="ped";
	h1TypeToNameMap_[EcalChannelAnalyzer::H1_JITTER]="jitter";
	h1TypeToNameMap_[EcalChannelAnalyzer::PROF_PULSE]="pulse";
}

void EcalChannelAnalyzer::fillEventListVector(const std::vector<std::string> & v_cuts) {

	for (unsigned int i=0; i<v_cuts.size(); i++) {

		TCut bufferCut = v_cuts[i].c_str();
		TEventList eventList = *getEventListFromCut(bufferCut); 
		v_eventList_.push_back(eventList);

	}

}

void EcalChannelAnalyzer::writeHistFromFile (const int hashedIndex, const int ism, const int ic, const EcalChannelAnalyzer::n_h1Type H1_TYPE) {


	//getting histogram from input file
	std::string dirbuffer = h1TypeToDirectoryMap_[H1_TYPE]+"/"+intToString(hashedIndex);
	fin_histo_->cd();
	TH1F * hist  = (TH1F*)fin_histo_->Get(dirbuffer.c_str());

	//name: ism_ic_typeOfHistogram_bitmask
	//title: ism_ic_typeOfHistogram:cut1:cut2
	std::string histName = intToString(ism) + "_" + intToString(ic) + "_" + h1TypeToNameMap_[H1_TYPE] + "_" + printBitmask(xtalBitmask_[hashedIndex]); 
	std::string histTitle = intToString(ism) + "_" + intToString(ic) + "_" + h1TypeToNameMap_[H1_TYPE] + printBitmaskCuts(xtalBitmask_[hashedIndex]); 
	hist->SetName(histName.c_str());
	hist->SetTitle(histTitle.c_str());

	//writing histogram on output file
	fout_->cd();
	hist->Write();

}

std::string EcalChannelAnalyzer::intToString(int num)
{
	using namespace std;
	ostringstream myStream;
	myStream << num << flush;
	return(myStream.str()); //returns the string form of the stringstream object
}

std::string EcalChannelAnalyzer::printBitmask(std::vector<bool>* bitmask) {

	std::string bitmaskString;

	for (unsigned int i=0; i< bitmask->size(); i++) {
		//using intToString to add all bits in bitmask to the string
		bitmaskString += intToString(int(bitmask->operator[](i)));
	}

	return bitmaskString;
}

std::string EcalChannelAnalyzer::printBitmaskCuts(std::vector<bool>*bitmask) {

	std::string bitmaskCuts;

	for (unsigned int i=0; i< bitmask->size(); i++) {
		if (bitmask->operator[](i)) {
			//getting name of cut from TEntryList vector
			bitmaskCuts += ":";
			bitmaskCuts += v_eventList_[i].GetTitle();
		}
	}

	return bitmaskCuts;
}
