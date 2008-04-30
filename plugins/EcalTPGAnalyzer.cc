// -*- C++ -*-
//
// Class:      EcalTPGAnalyzer
//
//
// Original Author:  Pascal Paganini
//
//


// system include files
#include <memory>
#include <utility>

// user include files
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalTriggerPrimitiveDigi.h"

#include "Geometry/CaloTopology/interface/EcalTrigTowerConstituentsMap.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

#include "EcalTPGAnalyzer.h"

#include <TMath.h>

using namespace edm;
class CaloSubdetectorGeometry;

EcalTPGAnalyzer::EcalTPGAnalyzer(const edm::ParameterSet&  iConfig)
{
  label_= iConfig.getParameter<std::string>("Label");
  producer_= iConfig.getParameter<std::string>("Producer");
  digi_label_= iConfig.getParameter<std::string>("DigiLabel");
  digi_producer_=  iConfig.getParameter<std::string>("DigiProducer");
  emul_label_= iConfig.getParameter<std::string>("EmulLabel");
  emul_producer_=  iConfig.getParameter<std::string>("EmulProducer");
  adcCut_ =  iConfig.getParameter<int>("ADCCut");
  shapeCut_ =  iConfig.getParameter<int>("shapeCut");
  occupancyCut_ =  iConfig.getParameter<int>("occupancyCut");
  tpgRef_ =  iConfig.getParameter<int>("TPGEmulatorIndexRef") ;

  histfile_ = new TFile("histosTPG.root","RECREATE");

  //histo
  shape_ = new TH2F("shape","Tower Shape",11, -1, 10, 120, -1.1, 1.1) ; 

  occupancyTP_ = new TH2F("occupancyTP", "Occupancy TP data", 38, -19, 19, 73, 0, 73) ;
  occupancyTP_->GetXaxis()->SetTitle("eta index") ;
  occupancyTP_->GetYaxis()->SetTitle("phi index") ;
  occupancyTPEmul_ = new TH2F("occupancyTPEmul", "Occupancy TP emulator", 38, -19, 19, 73, 0, 73) ;
  occupancyTPEmul_->GetXaxis()->SetTitle("eta index") ;
  occupancyTPEmul_->GetYaxis()->SetTitle("phi index") ;
  occupancyTPEmulMax_ = new TH2F("occupancyTPEmulMax", "Occupancy TP emulator max", 38, -19, 19, 73, 0, 73) ;
  occupancyTPEmulMax_->GetXaxis()->SetTitle("eta index") ;
  occupancyTPEmulMax_->GetYaxis()->SetTitle("phi index") ;

  crystalVsTP_ = new TH2F("crystalVsTP", "tower Ener vs TP", 256, 0., 256., 301, -1., 300.) ;
  crystalVsTP_->GetXaxis()->SetTitle("TP (ADC)") ;
  crystalVsTP_->GetYaxis()->SetTitle("Tower E(ADC)") ;
  crystalVsEmulTP_ = new TH2F("crystalVsEmulTP", "tower Ener vs Emulator TP", 256, 0., 256., 301, -1., 300.) ;
  crystalVsEmulTP_->GetXaxis()->SetTitle("TP (ADC)") ;
  crystalVsEmulTP_->GetYaxis()->SetTitle("Tower E(ADC)") ;
  crystalVsEmulTPMax_ = new TH2F("crystalVsEmulTPMax", "tower Ener vs Max Emulator TP", 256, 0., 256., 301, -1., 300.) ;
  crystalVsEmulTPMax_->GetXaxis()->SetTitle("TP (ADC)") ;
  crystalVsEmulTPMax_->GetYaxis()->SetTitle("Tower E(ADC)") ;
  TPVsEmulTP_ = new TH2F("TPVsEmulTP", "TP vs Emulator TP", 256, 0., 256., 256, 0., 256.) ;
  TPVsEmulTP_->GetXaxis()->SetTitle("TP Emul (ADC)") ;
  TPVsEmulTP_->GetYaxis()->SetTitle("TP Data (ADC)") ;
  
  TP_ = new TH1F("TP", "TP", 256, 0., 256.) ;
  TP_->GetXaxis()->SetTitle("TP (ADC)") ;
  TPEmul_ = new TH1F("TPEmul", "TP Emulator", 256, 0., 256.) ;
  TPEmul_->GetXaxis()->SetTitle("TP (ADC)") ;
  TPEmulMax_ = new TH1F("TPEmulMax", "TP Emulator max", 256, 0., 256.) ;
  TPEmulMax_->GetXaxis()->SetTitle("TP (ADC)") ;

  TPMatchEmul_ = new TH1F("TPMatchEmul", "TP data matching Emulator",6 , -1., 5.) ;
  TPEmulMaxIndex_ = new TH1F("TPEmulMaxIndex", "Index of the max TP from Emulator", 6, -1., 5.) ;
}


EcalTPGAnalyzer::~EcalTPGAnalyzer()
{
  shape_->Write() ;
  occupancyTP_->Write() ;
  occupancyTPEmul_->Write() ;
  occupancyTPEmulMax_->Write() ;
  crystalVsTP_->Write() ;
  crystalVsEmulTP_->Write() ;
  crystalVsEmulTPMax_->Write() ;
  TPVsEmulTP_->Write() ;
  TP_->Write() ;
  TPEmul_->Write() ;
  TPEmulMax_->Write() ;
  TPMatchEmul_->Write() ; 
  TPEmulMaxIndex_->Write() ;

  histfile_->Write();
  histfile_->Close();
}

void EcalTPGAnalyzer::analyze(const edm::Event& iEvent, const  edm::EventSetup & iSetup)
{
  using namespace edm;
  using namespace std;

  map<EcalTrigTowerDetId, towerEner> mapTower ;
  map<EcalTrigTowerDetId, towerEner>::iterator itTT ;
  
  // Get xtal digi inputs
  edm::Handle<EBDigiCollection> digi;
  iEvent.getByLabel(digi_label_, digi_producer_, digi);
  for (unsigned int i=0;i<digi.product()->size();i++) {
    const EBDataFrame & df = (*(digi.product()))[i];
    
    int gain, adc ;
    float E_xtal = 0. ; 
    int theSamp = 0 ;
    float mean = 0., max = -999 ; 
    for (int samp = 0 ; samp<10 ; samp++) {
      adc = df[samp].adc() ;
      if (samp<2) mean += adc ;
      if (adc>max) {
	max = adc ;
	theSamp = samp ;
      }
    }
    mean /= 2 ;
    if (mean>0 && max >= mean + adcCut_) {
      gain = df[theSamp].gainId() ;
      adc = df[theSamp].adc() ;
      if (gain == 1) E_xtal = (adc-mean) ;
      if (gain == 2) E_xtal = 2.*(adc-mean) ;
      if (gain == 3) E_xtal = 12.*(adc-mean) ;
      if (gain == 0) E_xtal = 12.*(adc-mean) ;
    }
    const EBDetId & id=df.id();
    const EcalTrigTowerDetId towid= id.tower();
    itTT = mapTower.find(towid) ;
    if (itTT != mapTower.end()) {
      (itTT->second).eRec_ += E_xtal ;
      for (int samp = 0 ; samp<10 ; samp++) (itTT->second).data_[samp] += df[samp].adc()-mean ;
      (itTT->second).iphi_ = towid.iphi() ;
      (itTT->second).ieta_ = towid.ieta() ;
    }
    else {
      towerEner tE ;
      tE.eRec_ = E_xtal ;
      for (int samp = 0 ; samp<10 ; samp++) tE.data_[samp] = df[samp].adc()-mean ;
      tE.iphi_ = towid.iphi() ;
      tE.ieta_ = towid.ieta() ;
      mapTower[towid] = tE ;
    }
  }


  // Get TP data
  edm::Handle<EcalTrigPrimDigiCollection> tp;
  iEvent.getByLabel(label_,producer_,tp);

  for (unsigned int i=0;i<tp.product()->size();i++) {
    EcalTriggerPrimitiveDigi d = (*(tp.product()))[i];
    const EcalTrigTowerDetId TPtowid= d.id();

    itTT = mapTower.find(TPtowid) ;
    if (itTT != mapTower.end()) {
      (itTT->second).tpgADC_ = d.compressedEt() ;
      (itTT->second).ttf_ = d.ttFlag() ;
      (itTT->second).fg_ = d.fineGrain() ;      
    }
    else {
      towerEner tE ;
      tE.iphi_ = TPtowid.iphi() ;
      tE.ieta_ = TPtowid.ieta() ;
      tE.tpgADC_ = d.compressedEt() ;
      tE.ttf_ = d.ttFlag() ;
      tE.fg_ = d.fineGrain() ;    
      mapTower[TPtowid] = tE ;
    }
  }

  // Get Emulators TP
   edm::Handle<EcalTrigPrimDigiCollection> tpEmul ;
   iEvent.getByLabel(emul_label_, emul_producer_, tpEmul);
   for (unsigned int i=0;i<tpEmul.product()->size();i++) {
     EcalTriggerPrimitiveDigi d = (*(tpEmul.product()))[i];
     const EcalTrigTowerDetId TPtowid= d.id();
     itTT = mapTower.find(TPtowid) ;
     if (itTT != mapTower.end())
       for (int j=0 ; j<5 ; j++) (itTT->second).tpgEmul_[j] = (d[j].raw() & 0x1ff) ;
     else {
       towerEner tE ;
       tE.iphi_ = TPtowid.iphi() ;
       tE.ieta_ = TPtowid.ieta() ;
       for (int j=0 ; j<5 ; j++) tE.tpgEmul_[j] = (d[j].raw() & 0x1ff) ;
       mapTower[TPtowid] = tE ;
     }
   }

  // fill histograms
  for (itTT = mapTower.begin() ; itTT != mapTower.end() ; ++itTT ) {
    fillShape(itTT->second) ;
    fillOccupancyPlots(itTT->second) ;
    fillEnergyPlots(itTT->second) ;
    fillTPMatchPlots(itTT->second) ;
  }

}
  
void EcalTPGAnalyzer::fillShape(towerEner & t)
{
  float max = 0. ;
  for (int i=0 ; i<10 ; i++) if (t.data_[i]>max) max = t.data_[i] ;
  if (max>0 && max>=shapeCut_)
    for (int i=0 ; i<10 ; i++) shape_->Fill(float(i), t.data_[i]/max) ;
}

void  EcalTPGAnalyzer::fillOccupancyPlots(towerEner & t)
{
  int max = 0 ;
  for (int i=0 ; i<5 ; i++) if ((t.tpgEmul_[i]&0xff)>max) max = (t.tpgEmul_[i]&0xff) ; 
  if (max >= occupancyCut_) occupancyTPEmulMax_->Fill(t.ieta_, t.iphi_) ;
  if ((t.tpgEmul_[tpgRef_]&0xff) >= occupancyCut_) occupancyTPEmul_->Fill(t.ieta_, t.iphi_) ;
  if (t.tpgADC_ >= occupancyCut_) occupancyTP_->Fill(t.ieta_, t.iphi_) ;
}

void  EcalTPGAnalyzer::fillEnergyPlots(towerEner & t)
{
  int max = 0 ;
  for (int i=0 ; i<5 ; i++) if ((t.tpgEmul_[i]&0xff)>max) max = (t.tpgEmul_[i]&0xff) ; 
  crystalVsTP_->Fill(t.tpgADC_, t.eRec_) ;
  crystalVsEmulTP_->Fill((t.tpgEmul_[tpgRef_]&0xff), t.eRec_) ;
  crystalVsEmulTPMax_->Fill(max, t.eRec_) ;
  TPVsEmulTP_->Fill((t.tpgEmul_[tpgRef_]&0xff), t.tpgADC_) ;
  TP_->Fill(t.tpgADC_) ;
  TPEmul_->Fill((t.tpgEmul_[tpgRef_]&0xff)) ;
  TPEmulMax_->Fill(max) ;
}

void EcalTPGAnalyzer::fillTPMatchPlots(towerEner & t)
{
  bool match(false) ;
  if (t.tpgADC_>0) {
    for (int i=0 ; i<5 ; i++)
      if ((t.tpgEmul_[i]&0xff) == t.tpgADC_) {
	TPMatchEmul_->Fill(i) ;
	match = true ;
      }
    if (!match) TPMatchEmul_->Fill(-1) ;
  }

  int max = 0 ;
  int index = -1 ;
  for (int i=0 ; i<5 ; i++) 
    if ((t.tpgEmul_[i]&0xff)>max) {
      max = (t.tpgEmul_[i]&0xff) ; 
      index = i ;
    }
  if (max>0) TPEmulMaxIndex_->Fill(index) ;

}
