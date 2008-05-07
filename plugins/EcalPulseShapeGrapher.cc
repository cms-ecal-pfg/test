// -*- C++ -*-
//
// Package:    EcalPulseShapeGrapher
// Class:      EcalPulseShapeGrapher
// 
/**\class EcalPulseShapeGrapher EcalPulseShapeGrapher.cc Analyzers/EcalPulseShapeGrapher/src/EcalPulseShapeGrapher.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Tue Feb  5 11:35:45 CST 2008
// $Id$
//
//

#include "CaloOnlineTools/EcalTools/plugins/EcalPulseShapeGrapher.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalPulseShapeGrapher::EcalPulseShapeGrapher(const edm::ParameterSet& iConfig) :
EBUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBUncalibratedRecHitCollection")),
EBDigis_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
ampCut_ (iConfig.getUntrackedParameter<int>("AmplitudeCutADC", 13))
{
   //now do what ever initialization is needed

   std::vector<int> listDefaults;
   listDefaults.push_back(-1);
   listChannels_ = iConfig.getUntrackedParameter<std::vector<int> >("listChannels", listDefaults);

   for(std::vector<int>::const_iterator itr = listChannels_.begin(); itr != listChannels_.end(); ++itr)
   {
     std::string title = "Amplitude of cry "+intToString(*itr);
     std::string name = "ampOfCry"+intToString(*itr);
     ampHistMap_[*itr] = new TH1F(name.c_str(),title.c_str(),100,0,100);
     ampHistMap_[*itr]->GetXaxis()->SetTitle("ADC");

     title = "Amplitude (over 13 ADC) of cry "+intToString(*itr);
     name = "cutAmpOfCry"+intToString(*itr);
     cutAmpHistMap_[*itr] = new TH1F(name.c_str(),title.c_str(),100,0,100);
     cutAmpHistMap_[*itr]->GetXaxis()->SetTitle("ADC");

     title = "SNP amplitude of cry "+intToString(*itr);
     name = "SNPampOfCry"+intToString(*itr);
     snpAmpHistMap_[*itr] = new TH1F(name.c_str(),title.c_str(),100,0,100);
     snpAmpHistMap_[*itr]->GetXaxis()->SetTitle("ADC");
     
     title = "Cosmic amplitude of cry "+intToString(*itr);
     name = "cosmicAmpOfCry"+intToString(*itr);
     cosmicAmpHistMap_[*itr] = new TH1F(name.c_str(),title.c_str(),100,0,100);
     cosmicAmpHistMap_[*itr]->GetXaxis()->SetTitle("ADC");
     
     title = "Pulse shape of cry "+intToString(*itr);
     name = "PulseShapeCry"+intToString(*itr);
     pulseShapeHistMap_[*itr] = new TH2F(name.c_str(),title.c_str(),10,0,10,220,-20,2);
     pulseShapeHistMap_[*itr]->GetXaxis()->SetTitle("sample");

     title = "Raw Pulse shape of cry "+intToString(*itr);
     name = "RawPulseShapeCry"+intToString(*itr);
     rawPulseShapeHistMap_[*itr] = new TH2F(name.c_str(),title.c_str(),10,0,10,500,0,500);
     rawPulseShapeHistMap_[*itr]->GetXaxis()->SetTitle("sample");
     
     title = "Ped Max Pulse shape of cry "+intToString(*itr);
     name = "PedMaxPulseShapeCry"+intToString(*itr);
     pedMaxPulseShapeHistMap_[*itr] = new TH2F(name.c_str(),title.c_str(),10,0,10,1000,-50,50);
     pedMaxPulseShapeHistMap_[*itr]->GetXaxis()->SetTitle("sample");
     
     title = "Non-ped max Pulse shape of cry "+intToString(*itr);
     name = "NonPedMaxPulseShapeCry"+intToString(*itr);
     nonPedMaxPulseShapeHistMap_[*itr] = new TH2F(name.c_str(),title.c_str(),10,0,10,1000,-50,50);
     nonPedMaxPulseShapeHistMap_[*itr]->GetXaxis()->SetTitle("sample");
     
     title = "Amplitude of first sample, cry "+intToString(*itr);
     name = "AmpOfFirstSampleCry"+intToString(*itr);
     firstSampleHistMap_[*itr] = new TH1F(name.c_str(),title.c_str(),300,100,400);
     firstSampleHistMap_[*itr]->GetXaxis()->SetTitle("ADC");
   }
   
   fedMap_ = new EcalFedMap();
   
   for (int i=0; i<10; i++)
     abscissa[i] = i;
}


EcalPulseShapeGrapher::~EcalPulseShapeGrapher()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalPulseShapeGrapher::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;

   int numHitsWithActivity = 0;
   //int numCosmicsInEvent = 0;
   //int numMonstersInEvent = 0;
   
   //int eventNum = iEvent.id().event();
   //vector<EcalUncalibratedRecHit> sampleHitsPastCut;
   
   Handle<EcalUncalibratedRecHitCollection> sampleHits;
   iEvent.getByLabel(EBUncalibratedRecHitCollection_, sampleHits);
   //cout << "event: " << eventNum << " sample hits collection size: " << sampleHits->size() << endl;
   //Handle<EcalUncalibratedRecHitCollection> fittedHits;
   //iEvent.getByLabel(FittedUncalibratedRecHitCollection_, fittedHits);
   Handle<EBDigiCollection>  digis;
   iEvent.getByLabel(EBDigis_, digis);
   //cout << "event: " << eventNum << " digi collection size: " << digis->size() << endl;

   auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

   // Loop over the max-min hits
   for(EcalUncalibratedRecHitCollection::const_iterator hitItr = sampleHits->begin(); hitItr != sampleHits->end(); ++hitItr)
   {
     EcalUncalibratedRecHit hit = (*hitItr);
     float amplitude = hit.amplitude();

     EBDetId hitDetId = hit.id();

     // Get the Fedid
     EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(hitDetId);
     int FEDid = 600+elecId.dccId();
     string SMname = fedMap_->getSliceFromFed(FEDid);
     
     vector<int>::const_iterator itr = listChannels_.begin();
     while(itr != listChannels_.end() && (*itr) != hitDetId.hashedIndex())
     {
       itr++;
     }
     if(itr==listChannels_.end())
       continue;

     // Get the Fedid
     //EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(hitDetId);
     //int FEDid = 600+elecId.dccId();
     //string SMname = fedMap_->getSliceFromFed(FEDid);

     ampHistMap_[hitDetId.hashedIndex()]->Fill(amplitude);
     //cout << "Cry hash:" << hitDetId.hashedIndex() << " amplitude: " << amplitude << endl;

     if(amplitude < ampCut_)
       continue;

     cutAmpHistMap_[hitDetId.hashedIndex()]->Fill(amplitude);
     
     // Hit is probably a signal
     numHitsWithActivity++;
     EBDigiCollection::const_iterator digiItr= digis->begin();
     while(digiItr != digis->end() && digiItr->id() != hitItr->id())
     {
       digiItr++;
     }
     if(digiItr == digis->end())
       continue;

     EBDataFrame df(*digiItr); 
     int delta1 = abs(df.sample(1).adc()-df.sample(0).adc());
     int delta2 = abs(df.sample(2).adc()-df.sample(1).adc());
     // check the min/max amp. sample indices
     // I moved this from inside the if with the deltas
     int maxSampleAmp = -1000;
     int maxSampleIndex = -1;
     int minSampleAmp = 10000;
     int minSampleIndex = -1;
     for(int i=0; i<10; ++i)
     {
       if(df.sample(i).adc() > maxSampleAmp && !isnan(df.sample(i).adc()))
       {
         maxSampleAmp = df.sample(i).adc();
         maxSampleIndex = i;
       }
       if(df.sample(i).adc() < minSampleAmp && !isnan(df.sample(i).adc()))
       {
         minSampleAmp = df.sample(i).adc();
         minSampleIndex = i;
       }
     }

     //cout << "1) maxsample amp:" << maxSampleAmp << " maxSampleIndex:" << maxSampleIndex << endl;
     int baseline = (df.sample(0).adc()+df.sample(1).adc()+df.sample(2).adc())/3;
     for(int i=0; i<10;++i)
     {
       //cout << "Filling hist for:" << hitDetId.hashedIndex() << " with sample:" << i 
         //<< " amp:" <<(float)(df.sample(i).adc()-baseline)/(maxSampleAmp-baseline) << endl;
       //cout << "ADC of sample:" << df.sample(i).adc() << " baseline:" << baseline << " maxSampleAmp:" 
         //<< maxSampleAmp << endl << endl;
       pulseShapeHistMap_[hitDetId.hashedIndex()]->Fill(i, (float)(df.sample(i).adc()-baseline)/(maxSampleAmp-baseline));
       rawPulseShapeHistMap_[hitDetId.hashedIndex()]->Fill(i, (float)(df.sample(i).adc()));
       if(maxSampleIndex<2)
         pedMaxPulseShapeHistMap_[hitDetId.hashedIndex()]->Fill(i, (float)(df.sample(i).adc()-baseline)/(maxSampleAmp-baseline));
       else
         nonPedMaxPulseShapeHistMap_[hitDetId.hashedIndex()]->Fill(i, (float)(df.sample(i).adc()-baseline)/(maxSampleAmp-baseline));
     }

     firstSampleHistMap_[hitDetId.hashedIndex()]->Fill(df.sample(0).adc());
     
     if(delta1 < 3 || delta2 < 3)
     {
       // could it be a cosmic hit?

       if(!(maxSampleIndex > 6 || maxSampleIndex < 4) && minSampleIndex < 3)
       {
         cosmicAmpHistMap_[hitDetId.hashedIndex()]->Fill(amplitude);
       }
       else // when max/min sample check is out of range, but deltas are ok
       {
         snpAmpHistMap_[hitDetId.hashedIndex()]->Fill(amplitude);
       }
     }
     else
     {
       snpAmpHistMap_[hitDetId.hashedIndex()]->Fill(amplitude);
     }

   }
}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalPulseShapeGrapher::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalPulseShapeGrapher::endJob() {
  
   file_ = new TFile("test.root","RECREATE");
   TH1::AddDirectory(false);
   
  
   for(std::vector<int>::const_iterator itr = listChannels_.begin(); itr != listChannels_.end(); ++itr)
   {
     ampHistMap_[*itr]->Write();
     cutAmpHistMap_[*itr]->Write();
     cosmicAmpHistMap_[*itr]->Write();
     snpAmpHistMap_[*itr]->Write();
     firstSampleHistMap_[*itr]->Write();

     pulseShapeHistMap_[*itr]->Write();
     rawPulseShapeHistMap_[*itr]->Write();
     TProfile* t2 = (TProfile*) (rawPulseShapeHistMap_[*itr]->ProfileX());
     t2->Write();
     pedMaxPulseShapeHistMap_[*itr]->Write();
     nonPedMaxPulseShapeHistMap_[*itr]->Write();
     TProfile* t1 = (TProfile*) (pulseShapeHistMap_[*itr]->ProfileX());
     t1->Write();
   }

   
  file_->Write();
  file_->Close();
}

std::string EcalPulseShapeGrapher::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}
