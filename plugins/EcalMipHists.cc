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

#include "CaloOnlineTools/EcalTools/plugins/EcalMipHists.h"

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
EcalMipHists::EcalMipHists(const edm::ParameterSet& iConfig) :
  EBDigis_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
  EBUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBUncalibratedRecHitCollection")),
  headerProducer_ (iConfig.getParameter<edm::InputTag>("headerProducer")),
  XtalJitterDir_ (fs_->mkdir("XtalJitter")),
  XtalAmpliDir_ (fs_->mkdir("XtalAmpli")),
  XtalPedDir_ (fs_->mkdir("XtalPed")),
  XtalPulseDir_ (fs_->mkdir("XtalPulse")),
  runNum_(-1),
  eventNum_(0)
{
  //cleaning up vectors
  for (int i=0; i<61200; i++) {
    v_h1_XtalJitter_[i]=0;
    v_h1_XtalAmpli_[i]=0;
    v_h1_XtalPed_[i]=0;
    v_prof_XtalPulse_[i]=0;
  }
  //making sure that hists keep track of overflows
  TH1::StatOverflows(1);
}

EcalMipHists::~EcalMipHists()
{
}

//
// member functions
//

// ------------ method called to for each event  ------------
  void
EcalMipHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  //run number && event number
  runNum_=iEvent.id().run();
  //recalculating event number - some ST data had resetting counters due to DAQ
  eventNum_++;

  std::cout << "starting analysis for event " << eventNum_ << std::endl;

  // get the headers
  // (one header for each FED)
  edm::Handle<EcalRawDataCollection> DCCHeaders;
  iEvent.getByLabel(headerProducer_, DCCHeaders);
  map<int,EcalDCCHeaderBlock> FEDsAndDCCHeaders_;
  for (EcalRawDataCollection::const_iterator headerItr= DCCHeaders->begin();
      headerItr != DCCHeaders->end (); 
      ++headerItr) 
  {
    FEDsAndDCCHeaders_[headerItr->id()+600] = *headerItr;
  }

  // retrieving crystal digis
  edm::Handle<EBDigiCollection>  EBdigis;
  iEvent.getByLabel(EBDigis_, EBdigis);
  // retrieving uncalibrated recHits
  edm::Handle<EcalUncalibratedRecHitCollection> EBhits; 
  iEvent.getByLabel(EBUncalibratedRecHitCollection_, EBhits);
  
  for (EcalUncalibratedRecHitCollection::const_iterator hitItr = EBhits->begin(); hitItr != EBhits->end(); ++hitItr)
  {
    
    EBDetId ebDetId = hitItr->id();
    int xtal_hashed = ebDetId.hashedIndex();
    double jitter = hitItr->jitter();
    double amplitude = hitItr->amplitude();
    
    // find the digi
    EBDigiCollection::const_iterator digiItr = EBdigis->begin();

    while(digiItr != EBdigis->end() && ((*digiItr).id()!=ebDetId))
    {
      ++digiItr;
    }
    if(digiItr==EBdigis->end())
    {
      LogWarning("EcalMipHists") << "Cannot find digi for ic:" << ebDetId.ic()
        << " evt:" << eventNum_;
      continue;
    }

    EBDataFrame digi(*digiItr);
    double pedestal = (double)(digi.sample(0).adc() + digi.sample(1).adc())/(double)2;
    
    //----------------XTALS ampli/jitter

    //booking jitter histograms
    if (!v_h1_XtalJitter_[xtal_hashed]) {
      //TFileDirectory::make histogram if it has never been booked before
      v_h1_XtalJitter_[xtal_hashed] = XtalJitterDir_.make<TH1F> (intToString(xtal_hashed).c_str(),intToString(xtal_hashed).c_str(),11,0,11);
      //being paranoid about overflows
      v_h1_XtalJitter_[xtal_hashed]->StatOverflows(1);
      //being paranoid about pointers (but not really doing much)
      if (!v_h1_XtalJitter_[xtal_hashed]) std::cout << "TFileService (xtal jitter) had a problem and you will have a problem soon (segfault)" << std::endl;
    }

    //filling histogram and map
    v_h1_XtalJitter_[xtal_hashed]->Fill(jitter);
    prof2_XtalJitter_->Fill(ebDetId.iphi()-0.5, ebDetId.ieta(), jitter);

    //amplitude
    if (!v_h1_XtalAmpli_[xtal_hashed]) {
      v_h1_XtalAmpli_[xtal_hashed] = XtalAmpliDir_.make<TH1F> (intToString(xtal_hashed).c_str(),intToString(xtal_hashed).c_str(),500,0,500);
      v_h1_XtalAmpli_[xtal_hashed]->StatOverflows(1);
      if (!v_h1_XtalAmpli_[xtal_hashed]) std::cout << "TFileService (xtal ampli) had a problem and you will have a problem soon (segfault)" << std::endl;
    }

    v_h1_XtalAmpli_[xtal_hashed]->Fill(amplitude-pedestal);
    prof2_XtalAmpli_->Fill(ebDetId.iphi()-0.5, ebDetId.ieta(), amplitude-pedestal);


    //-------------------XTALS ped

    //std::cout << "pedestal=" << pedestal << std::endl;

    if (!v_h1_XtalPed_[xtal_hashed]) {
      v_h1_XtalPed_[xtal_hashed] = XtalPedDir_.make<TH1F> (intToString(xtal_hashed).c_str(),intToString(xtal_hashed).c_str(),1000,0,1000);
      v_h1_XtalPed_[xtal_hashed]->StatOverflows(1);
      if (!v_h1_XtalPed_[xtal_hashed]) std::cout << "TFileService (xtals ped) had a problem and you will have a problem soon (segfault)" << std::endl;
    }

    v_h1_XtalPed_[xtal_hashed]->Fill(pedestal);
    prof2_XtalPed_->Fill(ebDetId.iphi()-0.5, ebDetId.ieta(), pedestal);

    //-------------------XTALS pulse

    if (!v_prof_XtalPulse_[xtal_hashed]) {
      v_prof_XtalPulse_[xtal_hashed] = XtalPulseDir_.make<TProfile> (intToString(xtal_hashed).c_str(),intToString(xtal_hashed).c_str(),10,0.5,10.5);
      if (!v_prof_XtalPulse_[xtal_hashed]) std::cout << "TFileService (xtals pulse) had a problem and you will have a problem soon (segfault)" << std::endl;
    }
    for (int i=0; i<10; i++) {

      double sample = digi.sample(i).adc();
      v_prof_XtalPulse_[xtal_hashed]->Fill(i+0.5,sample,1);

    }

  }//end RecHit loop
}//end analyze


// ------------ method called once each job just before starting event loop  ------------
  void 
EcalMipHists::beginJob(const edm::EventSetup&)
{
  //booking maps

  //Jitter
  std::string name = "Jitter_xtal_avg";
  std::string title = "Jitter (clock of max sample) per xtal (avg)";
  prof2_XtalJitter_=fs_->make<TProfile2D>(name.c_str(), title.c_str(), 360, 0 , 360, 170,-85 , 85);

  //Amplitude
  name = "Ampli_xtal_avg";
  title = "Max amplitude per xtal (avg)";
  prof2_XtalAmpli_=fs_->make<TProfile2D>(name.c_str(), title.c_str(), 360, 0, 360, 170,-85 , 85);

  //Pedestal
  name = "Ped_xtal_avg";
  title = "Pedestal (avg of first two samples) per xtal (avg)";
  prof2_XtalPed_=fs_->make<TProfile2D>(name.c_str(), title.c_str(), 360, 0, 360, 170,-85 , 85);

}



// ------------ method called once each job just after ending the event loop  ------------
  void 
EcalMipHists::endJob() 
{
  //TODO: PULSE FITTERS 

  makeTree();

  std::cout << "Writing files...this might take a while" << std::endl;

}


  void 
EcalMipHists::makeTree()
{

  //TODO: PLACE TREE SOMEWHERE ELSE (.cfg)
  TFile f_xtalTree("tree.root", "RECREATE");

  //creating file-resident tree (scared of how it's going to be handled...)
  //TODO: find a better name
  TTree tree_xtal;

  //variables
  int ic;
  int ism;
  int ieta;
  int iphi;
  int entries;
  int hashedIndex;
  float ped_avg;
  float ped_rms;  
  float ampli_avg;
  float ampli_rms;
  float jitter_avg;
  float jitter_rms;
  float ampli_fracBelowThreshold;

  tree_xtal.Branch("ic" , &ic, "ic/I");
  tree_xtal.Branch("ism" , &ism, "ism/I");
  tree_xtal.Branch("ieta" , &ieta, "ieta/I");
  tree_xtal.Branch("iphi" , &iphi, "iphi/I");
  tree_xtal.Branch("hashedIndex" , &hashedIndex, "hashedIndex/I");
  tree_xtal.Branch("ped_avg" , &ped_avg, "ped_avg/F");
  tree_xtal.Branch("ped_rms" , &ped_rms, "ped_rms/F");
  tree_xtal.Branch("ampli_avg" , &ampli_avg, "ampli_avg/F");
  tree_xtal.Branch("ampli_rms" , &ampli_rms, "ampli_rms/F");
  tree_xtal.Branch("jitter_avg" , &jitter_avg, "jitter_avg/F");
  tree_xtal.Branch("jitter_rms" , &jitter_rms, "jitter_rms/F");
  tree_xtal.Branch("ampli_fracBelowThreshold", &ampli_fracBelowThreshold, "ampli_fracBelowThreshold/F");
  tree_xtal.Branch("entries",&entries, "entries/I");

  //must check for unread crystals: do not fill the tree if any of the histogram is absent
  //TODO: REPORT SOMEWHERE IF ONE OF THE HISTOGRAMS IS MISSING (can it happen? it could, if different digi and recHit loops - e.g. skipping URH for ... errors)

  for (int i=0; i<61200; i++) {

    if (!v_h1_XtalAmpli_[i] || !v_h1_XtalPed_[i] || !v_h1_XtalJitter_[i]) continue;

    EBDetId detId = EBDetId::unhashIndex(i);
    ic = detId.ic();
    ism = detId.ism();
    ieta = detId.ieta();
    iphi = detId.iphi();
    hashedIndex = detId.hashedIndex(); //TODO: check it is i
    ped_avg = v_h1_XtalPed_[i]->GetMean();
    ped_rms = v_h1_XtalPed_[i]->GetRMS();
    ampli_avg = v_h1_XtalAmpli_[i]->GetMean();
    ampli_rms = v_h1_XtalAmpli_[i]->GetRMS();
    jitter_avg = v_h1_XtalJitter_[i]->GetMean();
    jitter_rms = v_h1_XtalJitter_[i]->GetRMS();
    entries = (int)v_h1_XtalAmpli_[i]->GetEntries();
    ampli_fracBelowThreshold = v_h1_XtalAmpli_[i]->Integral(0,10)/v_h1_XtalAmpli_[i]->Integral(0,10000);//FIXME: hardwired? overflows should be included though

    tree_xtal.Fill();

  }
  //saving the tree on file

  tree_xtal.Write();
  f_xtalTree.Close();

}

std::string EcalMipHists::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}



