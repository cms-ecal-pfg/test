//
// Macro to produce ECAL cosmic plots
//
void DrawCosmicPlots(Char_t* infile = 0, Int_t runNum=0, Char_t* fileType = "png", Bool_t printPics = kTRUE, Char_t* dirName = ".")
{

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0); gStyle->SetOptStat(10);

  if (!infile) {
    cout << " No input file specified !" << endl;
    return;
  }

  cout << "Producing cosmics plots for: " << infile << endl;

  TFile* f = new TFile(infile);

  int runNumber = 0;
  if (runNum==0) {
    runNumber = runNumberHist->GetBinContent(1);
    cout << "Run Number: " << runNumber << endl;
  } else {
    runNumber = runNum;
  }

  char name[100];  

  const int nHists1=25;
  const int nHists2=17;
  const int nHists = nHists1+nHists2;
  cout << nHists1 << " " << nHists2 << " " << nHists << endl;;

  TCanvas* c[nHists];
  char cname[100]; 

  for (int i=0; i<nHists1; i++) {
    sprintf(cname,"c%i",i);
    int x = (i%3)*600;     //int x = (i%3)*600;
    int y = (i/3)*100;     //int y = (i/3)*200;
    c[i] =  new TCanvas(cname,cname,x,y,600,400);
    //    cout << i << " : " << x << " , " << y << endl;
  }

  for (int i=nHists1; i<nHists; i++) {
    sprintf(cname,"c%i",i);
    int x = ((i-nHists1)%3)*600;     //int x = (i%3)*600;
    int y = ((i-nHists1)/3)*100;     //int y = (i/3)*200;
    c[i] =  new TCanvas(cname,cname,x,y,600,400);
    //    cout << i << " : " << x << " , " << y << endl;
  }

  char runChar[50];
  sprintf(runChar,", run %i",runNumber);

  c[0]->cd();
  gStyle->SetOptStat(1110);
  SeedEnergyAllFEDs->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",SeedEnergyAllFEDs->GetTitle()); 
  strcat(mytitle,runChar); SeedEnergyAllFEDs->SetTitle(mytitle);
  c[0]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_SeedEnergyAllFEDs_%i.%s",dirName,runNumber,fileType); c[0]->Print(name); }


  c[1]->cd();
  E2_AllClusters->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",E2_AllClusters->GetTitle()); 
  strcat(mytitle,runChar); E2_AllClusters->SetTitle(mytitle);
  c[1]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_E2_AllClusters_%i.%s",dirName,runNumber,fileType); c[1]->Print(name); }

  c[2]->cd();  
  energy_AllClusters->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",energy_AllClusters->GetTitle()); 
  strcat(mytitle,runChar); energy_AllClusters->SetTitle(mytitle);
  c[2]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_energy_AllClusters_%i.%s",dirName,runNumber,fileType); c[2]->Print(name); }

  c[3]->cd();  
  NumXtalsInE9AllHist->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",NumXtalsInE9AllHist->GetTitle()); 
  strcat(mytitle,runChar); NumXtalsInE9AllHist->SetTitle(mytitle);
  c[3]->SetLogy(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_NumXtalsInE9AllHist_%i.%s",dirName,runNumber,fileType); c[3]->Print(name); }

  c[4]->cd();
  energyvsE1_AllClusters->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",energyvsE1_AllClusters->GetTitle()); 
  strcat(mytitle,runChar); energyvsE1_AllClusters->SetTitle(mytitle);
  c[4]->SetLogy(0);
  c[4]->SetLogz(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_energyvsE1_AllClusters_%i.%s",dirName,runNumber,fileType); c[4]->Print(name); }

  c[5]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse->SetTitle(mytitle);
  OccupancyAllEventsCoarse->SetMinimum(1);
  OccupancyAllEventsCoarse->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse->GetYaxis()->SetNdivisions(2);
  c[5]->SetLogy(0);
  c[5]->SetLogz(1);
  c[5]->SetGridx(1);
  c[5]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_%i.%s",dirName,runNumber,fileType); c[5]->Print(name); }
  
  c[6]->cd();
  OccupancyAllEvents->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents->SetTitle(mytitle);
    OccupancyAllEvents->SetMinimum(1);  
    //        OccupancyAllEvents->SetMaximum(100);  
  OccupancyAllEvents->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents->GetYaxis()->SetNdivisions(2);
  c[6]->SetLogy(0);
  c[6]->SetLogz(1);
  c[6]->SetGridx(1);
  c[6]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_%i.%s",dirName,runNumber,fileType); c[6]->Print(name); }

  c[7]->cd();
  gStyle->SetOptStat(10);
  TrueOccupancyAllEventsCoarse->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",TrueOccupancyAllEventsCoarse->GetTitle()); 
  strcat(mytitle,runChar); TrueOccupancyAllEventsCoarse->SetTitle(mytitle);
  TrueOccupancyAllEventsCoarse->SetMinimum(1);
  TrueOccupancyAllEventsCoarse->GetXaxis()->SetNdivisions(-18);
  TrueOccupancyAllEventsCoarse->GetYaxis()->SetNdivisions(2);
  c[7]->SetLogy(0);
  c[7]->SetLogz(1);
  //  c[7]->SetGridx(1);
  //  c[7]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_TrueOccupancyAllEventsCoarse_%i.%s",dirName,runNumber,fileType); c[7]->Print(name); }
  
  c[8]->cd();
  TrueOccupancyAllEvents->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",TrueOccupancyAllEvents->GetTitle()); 
  strcat(mytitle,runChar); TrueOccupancyAllEvents->SetTitle(mytitle);
  TrueOccupancyAllEvents->SetMinimum(1);  
  TrueOccupancyAllEvents->GetXaxis()->SetNdivisions(-18);
  TrueOccupancyAllEvents->GetYaxis()->SetNdivisions(2);
  c[8]->SetLogy(0);
  c[8]->SetLogz(1);
  //  c[8]->SetGridx(1);
  //  c[8]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_TrueOccupancyAllEvents_%i.%s",dirName,runNumber,fileType); c[8]->Print(name); }


  c[9]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds->SetTitle(mytitle);
  c[9]->SetLogy(0);
  c[9]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_%i.%s",dirName,runNumber,fileType); c[9]->Print(name); }

  c[10]->cd();
  timeVsAmpAllEvents->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",timeVsAmpAllEvents->GetTitle()); 
  strcat(mytitle,runChar); timeVsAmpAllEvents->SetTitle(mytitle);
  timeVsAmpAllEvents->SetMinimum(1);
  c[10]->SetLogy(0);
  c[10]->SetLogz(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeVsAmpAllFeds_%i.%s",dirName,runNumber,fileType); c[10]->Print(name); }

  c[11]->cd();
  timePhiAllFEDs->ProfileX()->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timePhiAllFEDs->GetTitle()); 
  strcat(mytitle,runChar); timePhiAllFEDs->SetTitle(mytitle);
  timePhiAllFEDs->ProfileX()->GetYaxis()->SetTitle("Relative Time (1 clock = 25ns)");
  c[11]->SetLogy(0);
  c[11]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiAllFeds_%i.%s",dirName,runNumber,fileType); c[11]->Print(name); }

  c[12]->cd();
  timeEBP->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBP->GetTitle()); 
  strcat(mytitle,runChar); timeEBP->SetTitle(mytitle);
  c[12]->SetLogy(0);
  c[12]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeEBP_%i.%s",dirName,runNumber,fileType); c[12]->Print(name); }

  c[13]->cd();
  timeEBM->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBM->GetTitle()); 
  strcat(mytitle,runChar); timeEBM->SetTitle(mytitle);
  c[13]->SetLogy(0);
  c[13]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeEBM_%i.%s",dirName,runNumber,fileType); c[13]->Print(name); }

  c[14]->cd();
  timeEBPTop->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBPTop->GetTitle()); 
  strcat(mytitle,runChar); timeEBPTop->SetTitle(mytitle);
  c[14]->SetLogy(0);
  c[14]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeEBPTop_%i.%s",dirName,runNumber,fileType); c[14]->Print(name); }

  c[15]->cd();
  timeEBMTop->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBMTop->GetTitle()); 
  strcat(mytitle,runChar); timeEBMTop->SetTitle(mytitle);
  c[15]->SetLogy(0);
  c[15]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeETopBM_%i.%s",dirName,runNumber,fileType); c[15]->Print(name); }

  c[16]->cd();
  timeEBPBottom->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBPBottom->GetTitle()); 
  strcat(mytitle,runChar); timeEBPBottom->SetTitle(mytitle);
  c[16]->SetLogy(0);
  c[16]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeEBPBottom_%i.%s",dirName,runNumber,fileType); c[16]->Print(name); }

  c[17]->cd();
  timeEBMBottom->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeEBMBottom->GetTitle()); 
  strcat(mytitle,runChar); timeEBMBottom->SetTitle(mytitle);
  c[17]->SetLogy(0);
  c[17]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeEBMBottom_%i.%s",dirName,runNumber,fileType); c[17]->Print(name); }

  c[18]->cd();
  FrequencyAllEvent->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",FrequencyAllEvent->GetTitle()); 
  strcat(mytitle,runChar); FrequencyAllEvent->SetTitle(mytitle);
  c[18]->SetLogy(0);
  c[18]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_FrequencyAllEvent_%i.%s",dirName,runNumber,fileType); c[18]->Print(name); }

  c[19]->cd();
  timeVsFreqAllEvent->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",timeVsFreqAllEvent->GetTitle()); 
  strcat(mytitle,runChar); timeVsFreqAllEvent->SetTitle(mytitle);
  timeVsFreqAllEvent->SetMinimum(1);
  c[19]->SetLogy(0);
  c[19]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeVsFreqAllFeds_%i.%s",dirName,runNumber,fileType); c[19]->Print(name); }

  c[20]->cd();
  numberofCosmicsPerEvent->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",numberofCosmicsPerEvent->GetTitle()); 
  strcat(mytitle,runChar); numberofCosmicsPerEvent->SetTitle(mytitle);
  c[20]->SetLogy(0);
  c[20]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_numberofCosmicsPerEvent_%i.%s",dirName,runNumber,fileType); c[20]->Print(name); }

  c[21]->cd();
  frequencyOfGoodEvents->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",frequencyOfGoodEvents->GetTitle()); 
  strcat(mytitle,runChar); frequencyOfGoodEvents->SetTitle(mytitle);
  c[21]->SetLogy(0);
  c[21]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_frequencyOfGoodEvents_%i.%s",dirName,runNumber,fileType); c[21]->Print(name); }


  c[22]->cd();
  OccupancyAllEvents->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents->SetTitle(mytitle);
  OccupancyAllEvents->SetMinimum(1);  
  OccupancyAllEvents->SetMaximum(100);  
  OccupancyAllEvents->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents->GetYaxis()->SetNdivisions(2);
  c[22]->SetLogy(0);
  c[22]->SetLogz(1);
  c[22]->SetGridx(1);
  c[22]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsMax100_%i.%s",dirName,runNumber,fileType); c[22]->Print(name); }

  c[23]->cd();
  OccupancyAllEventsCoarse->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse->SetTitle(mytitle);
  OccupancyAllEventsCoarse->SetMinimum(1);  
  OccupancyAllEventsCoarse->SetMaximum(100);  
  OccupancyAllEventsCoarse->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse->GetYaxis()->SetNdivisions(2);
  c[22]->SetLogy(0);
  c[22]->SetLogz(1);
  c[22]->SetGridx(1);
  c[22]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarseMax100_%i.%s",dirName,runNumber,fileType); c[22]->Print(name); }

  c[24]->cd();  
  energyHigh_AllClusters->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",energyHigh_AllClusters->GetTitle()); 
  strcat(mytitle,runChar); energyHigh_AllClusters->SetTitle(mytitle);
  c[24]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_energyHigh_AllClusters_%i.%s",dirName,runNumber,fileType); c[24]->Print(name); }


  // triggered

  c[25]->cd();
  triggerHist->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",triggerHist->GetTitle()); 
  strcat(mytitle,runChar); triggerHist->SetTitle(mytitle);
  c[25]->SetLogy(0);
  c[25]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_triggerHist_%i.%s",dirName,runNumber,fileType); c[25]->Print(name); }

  c[26]->cd();
  triggerExclusiveHist->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",triggerExclusiveHist->GetTitle()); 
  strcat(mytitle,runChar); triggerExclusiveHist->SetTitle(mytitle);
  c[26]->SetLogy(0);
  c[26]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_triggerExclusiveHist_%i.%s",dirName,runNumber,fileType); c[26]->Print(name); }


  // ECAL triggered
  c[27]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse_ECAL->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse_ECAL->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse_ECAL->SetTitle(mytitle);
  OccupancyAllEventsCoarse_ECAL->SetMinimum(1);
  OccupancyAllEventsCoarse_ECAL->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse_ECAL->GetYaxis()->SetNdivisions(2);
  c[27]->SetLogy(0);
  c[27]->SetLogz(0);
  c[27]->SetGridx(1);
  c[27]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_ECAL_%i.%s",dirName,runNumber,fileType); c[27]->Print(name); }
  
  c[28]->cd();
  OccupancyAllEvents_ECAL->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents_ECAL->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents_ECAL->SetTitle(mytitle);
    OccupancyAllEvents_ECAL->SetMinimum(1);  
    //        OccupancyAllEvents_ECAL->SetMaximum(100);  
  OccupancyAllEvents_ECAL->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents_ECAL->GetYaxis()->SetNdivisions(2);
  c[28]->SetLogy(0);
  c[28]->SetLogz(0);
  c[28]->SetGridx(1);
  c[28]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_ECAL_%i.%s",dirName,runNumber,fileType); c[28]->Print(name); }

  c[29]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds_ECAL->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds_ECAL->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds_ECAL->SetTitle(mytitle);
  c[29]->SetLogy(0);
  c[29]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_ECAL_%i.%s",dirName,runNumber,fileType); c[29]->Print(name); }


  // HCAL triggered
  c[30]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse_HCAL->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse_HCAL->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse_HCAL->SetTitle(mytitle);
  OccupancyAllEventsCoarse_HCAL->SetMinimum(1);
  OccupancyAllEventsCoarse_HCAL->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse_HCAL->GetYaxis()->SetNdivisions(2);
  c[30]->SetLogy(0);
  c[30]->SetLogz(0);
  c[30]->SetGridx(1);
  c[30]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_HCAL_%i.%s",dirName,runNumber,fileType); c[30]->Print(name); }
  
  c[31]->cd();
  OccupancyAllEvents_HCAL->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents_HCAL->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents_HCAL->SetTitle(mytitle);
    OccupancyAllEvents_HCAL->SetMinimum(1);  
    //        OccupancyAllEvents_HCAL->SetMaximum(100);  
  OccupancyAllEvents_HCAL->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents_HCAL->GetYaxis()->SetNdivisions(2);
  c[31]->SetLogy(0);
  c[31]->SetLogz(0);
  c[31]->SetGridx(1);
  c[31]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_HCAL_%i.%s",dirName,runNumber,fileType); c[31]->Print(name); }

  c[32]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds_HCAL->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds_HCAL->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds_HCAL->SetTitle(mytitle);
  c[32]->SetLogy(0);
  c[32]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_HCAL_%i.%s",dirName,runNumber,fileType); c[32]->Print(name); }


  // DT triggered
  c[33]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse_DT->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse_DT->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse_DT->SetTitle(mytitle);
  OccupancyAllEventsCoarse_DT->SetMinimum(1);
  OccupancyAllEventsCoarse_DT->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse_DT->GetYaxis()->SetNdivisions(2);
  c[33]->SetLogy(0);
  c[33]->SetLogz(0);
  c[33]->SetGridx(1);
  c[33]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_DT_%i.%s",dirName,runNumber,fileType); c[33]->Print(name); }
  
  c[34]->cd();
  OccupancyAllEvents_DT->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents_DT->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents_DT->SetTitle(mytitle);
    OccupancyAllEvents_DT->SetMinimum(1);  
    //        OccupancyAllEvents_DT->SetMaximum(100);  
  OccupancyAllEvents_DT->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents_DT->GetYaxis()->SetNdivisions(2);
  c[34]->SetLogy(0);
  c[34]->SetLogz(0);
  c[34]->SetGridx(1);
  c[34]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_DT_%i.%s",dirName,runNumber,fileType); c[34]->Print(name); }

  c[35]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds_DT->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds_DT->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds_DT->SetTitle(mytitle);
  c[35]->SetLogy(0);
  c[35]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_DT_%i.%s",dirName,runNumber,fileType); c[35]->Print(name); }


  // RPC triggered
  c[36]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse_RPC->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse_RPC->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse_RPC->SetTitle(mytitle);
  OccupancyAllEventsCoarse_RPC->SetMinimum(1);
  OccupancyAllEventsCoarse_RPC->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse_RPC->GetYaxis()->SetNdivisions(2);
  c[36]->SetLogy(0);
  c[36]->SetLogz(0);
  c[36]->SetGridx(1);
  c[36]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_RPC_%i.%s",dirName,runNumber,fileType); c[36]->Print(name); }
  
  c[37]->cd();
  OccupancyAllEvents_RPC->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents_RPC->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents_RPC->SetTitle(mytitle);
    OccupancyAllEvents_RPC->SetMinimum(1);  
    //        OccupancyAllEvents_RPC->SetMaximum(100);  
  OccupancyAllEvents_RPC->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents_RPC->GetYaxis()->SetNdivisions(2);
  c[37]->SetLogy(0);
  c[37]->SetLogz(0);
  c[37]->SetGridx(1);
  c[37]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_RPC_%i.%s",dirName,runNumber,fileType); c[37]->Print(name); }

  c[38]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds_RPC->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds_RPC->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds_RPC->SetTitle(mytitle);
  c[38]->SetLogy(0);
  c[38]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_RPC_%i.%s",dirName,runNumber,fileType); c[38]->Print(name); }



  // CSC triggered
  c[39]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse_CSC->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEventsCoarse_CSC->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEventsCoarse_CSC->SetTitle(mytitle);
  OccupancyAllEventsCoarse_CSC->SetMinimum(1);
  OccupancyAllEventsCoarse_CSC->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse_CSC->GetYaxis()->SetNdivisions(2);
  c[39]->SetLogy(0);
  c[39]->SetLogz(0);
  c[39]->SetGridx(1);
  c[39]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse_CSC_%i.%s",dirName,runNumber,fileType); c[39]->Print(name); }
  
  c[40]->cd();
  OccupancyAllEvents_CSC->Draw("colz");
  char mytitle[100]; sprintf(mytitle,"%s",OccupancyAllEvents_CSC->GetTitle()); 
  strcat(mytitle,runChar); OccupancyAllEvents_CSC->SetTitle(mytitle);
    OccupancyAllEvents_CSC->SetMinimum(1);  
    //        OccupancyAllEvents_CSC->SetMaximum(100);  
  OccupancyAllEvents_CSC->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents_CSC->GetYaxis()->SetNdivisions(2);
  c[40]->SetLogy(0);
  c[40]->SetLogz(0);
  c[40]->SetGridx(1);
  c[40]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents_CSC_%i.%s",dirName,runNumber,fileType); c[40]->Print(name); }

  c[41]->cd();
  gStyle->SetOptStat(1110);
  timeForAllFeds_CSC->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",timeForAllFeds_CSC->GetTitle()); 
  strcat(mytitle,runChar); timeForAllFeds_CSC->SetTitle(mytitle);
  c[41]->SetLogy(0);
  c[41]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeForAllFeds_CSC_%i.%s",dirName,runNumber,fileType); c[41]->Print(name); }



  // fancy timing plots
  //
  gStyle->SetOptStat(10);

  cTiming = new TCanvas("cTiming","(phi,eta,timing)",900,600);
  cTiming->cd(1);
  cTiming->SetGridx();
  cTiming->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_%i.%s",dirName,runNumber,fileType); cTiming->Print(name); }



  // fancy timing plots (ECAL)
  //
  gStyle->SetOptStat(10);

  cTiming_ECAL = new TCanvas("cTiming_ECAL","(phi,eta,timing)",900,600);
  cTiming_ECAL->cd(1);
  cTiming_ECAL->SetGridx();
  cTiming_ECAL->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs_ECAL");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_ECAL_%i.%s",dirName,runNumber,fileType); cTiming_ECAL->Print(name); }



  // fancy timing plots (HCAL)
  //
  gStyle->SetOptStat(10);

  cTiming_HCAL = new TCanvas("cTiming_HCAL","(phi,eta,timing)",900,600);
  cTiming_HCAL->cd(1);
  cTiming_HCAL->SetGridx();
  cTiming_HCAL->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs_HCAL");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_HCAL_%i.%s",dirName,runNumber,fileType); cTiming_HCAL->Print(name); }



  // fancy timing plots (DT)
  //
  gStyle->SetOptStat(10);

  cTiming_DT = new TCanvas("cTiming_DT","(phi,eta,timing)",900,600);
  cTiming_DT->cd(1);
  cTiming_DT->SetGridx();
  cTiming_DT->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs_DT");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_DT_%i.%s",dirName,runNumber,fileType); cTiming_DT->Print(name); }


  // fancy timing plots (RPC)
  //
  gStyle->SetOptStat(10);

  cTiming_RPC = new TCanvas("cTiming_RPC","(phi,eta,timing)",900,600);
  cTiming_RPC->cd(1);
  cTiming_RPC->SetGridx();
  cTiming_RPC->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs_RPC");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_RPC_%i.%s",dirName,runNumber,fileType); cTiming_RPC->Print(name); }


  // fancy timing plots (CSC)
  //
  gStyle->SetOptStat(10);

  cTiming_CSC = new TCanvas("cTiming_CSC","(phi,eta,timing)",900,600);
  cTiming_CSC->cd(1);
  cTiming_CSC->SetGridx();
  cTiming_CSC->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("timePhiEtaAllFEDs_CSC");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

   char mytitle[100]; sprintf(mytitle,"%s",ayx->GetTitle()); 
   strcat(mytitle,runChar); ayx->SetTitle(mytitle);

  int nxb = ayx->GetNbinsX();
  int nyb = ayx->GetNbinsY();

  char tempErr[200];
  for (int i=1; i<=nxb; i++ ) {
    for (int j=1; j<=nyb; j++ ) {
      //printf("(%i,%i) -> %0.3f %0.3f\n",i,j,ayx->GetBinContent(i,j),ayx->GetBinError(i,j));
      
      double xcorr = ayx->GetXaxis()->GetBinCenter(i);
      double ycorr = ayx->GetYaxis()->GetBinCenter(j);
      sprintf(tempErr,"%0.2f",ayx->GetBinError(i,j));
      int nBin = ayx->GetBin(i,j,0);
      int nBinEntries = ayx->GetBinEntries(nBin);
      //cout << "         (x,y) nbin nentries: (" << xcorr << "," << ycorr;
      //cout << ")  " << nBin  << " " << nBinEntries << endl;
      //sprintf(tempErr,"%0.2f",ayx->GetBinContent(i,j));

      // print RMS of mean
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(23);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
      
      // print number of bin entries
      sprintf(tempErr,"%i",nBinEntries);
      if (nBinEntries!=0) {
	tex = new TLatex(xcorr,ycorr,tempErr);
	tex->SetTextAlign(21);
	tex->SetTextFont(42);
	tex->SetTextSize(0.025);
	tex->SetLineWidth(2);
	tex->Draw();
      }
    }
  }
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timePhiEtaAllFEDs_CSC_%i.%s",dirName,runNumber,fileType); cTiming_CSC->Print(name); }


  return;

}
