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

  const int nHists = 24;

  TCanvas* c[nHists];
  char cname[100]; 

  for (int i=0; i<nHists; i++) {
    sprintf(cname,"c%i",i);
    int x = (i%3)*600;     //int x = (i%3)*600;
    int y = (i/3)*100;     //int y = (i/3)*200;
    c[i] =  new TCanvas(cname,cname,x,y,600,400);
    //    cout << x << " , " << y << endl;
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
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_timeforAllFeds_%i.%s",dirName,runNumber,fileType); c[9]->Print(name); }

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
  energyHigh_AllClusters->Draw();
  char mytitle[100]; sprintf(mytitle,"%s",energyHigh_AllClusters->GetTitle()); 
  strcat(mytitle,runChar); energyHigh_AllClusters->SetTitle(mytitle);
  c[23]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_energyHigh_AllClusters_%i.%s",dirName,runNumber,fileType); c[23]->Print(name); }



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

  return;

}
