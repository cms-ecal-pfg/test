//
// Macro to produce ECAL cosmic plots
//
void DrawCosmicPlots(Char_t* infile = 0, Bool_t printPics = kTRUE, Char_t* dirName = ".")
{

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0); gStyle->SetOptStat(10);

  if (!infile) {
    cout << " No input file specified !" << endl;
    return;
  }

  cout << "Producing cosmics plots for: " << infile << endl;

  TFile* f = new TFile(infile);


  Char_t* runNumber = "";
  Char_t* fileType = "gif";

  char name[50];  

  const int nHists = 13;

  TCanvas* c[nHists];
  char cname[50]; 

  for (int i=0; i<nHists; i++) {
    sprintf(cname,"c%i",i);
    int x = (i%3)*600;
    int y = (i/3)*200;
    c[i] =  new TCanvas(cname,cname,x,y,600,400);
    cout << x << " , " << y << endl;
  }

  c[0]->cd();
  gStyle->SetOptStat(1110);
  URecHitsAllFEDs->Draw();
  c[0]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_URecHitsAllFEDs%s.%s",dirName,runNumber,fileType); c[0]->Print(name); }

  c[1]->cd();
  E2_AllURecHits->Draw();
  c[1]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_E2_AllURecHits%s.%s",dirName,runNumber,fileType); c[1]->Print(name); }

  c[2]->cd();  
  E9_AllURecHits->Draw();
  c[2]->SetLogy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_E9_AllURecHits%s.%s",dirName,runNumber,fileType); c[2]->Print(name); }

  c[3]->cd();  
  NumXtalsInE9AllHist->Draw();
  c[3]->SetLogy(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_NumXtalsInE9AllHist%s.%s",dirName,runNumber,fileType); c[3]->Print(name); }

  c[4]->cd();
  E2vsE1_AllURecHits->Draw("colz");
  c[4]->SetLogy(0);
  c[4]->SetLogz(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_E2vsE1_AllURecHits%s.%s",dirName,runNumber,fileType); c[4]->Print(name); }

  c[5]->cd();
  E9vsE1_AllURecHits->Draw("colz");
  c[5]->SetLogy(0);
  c[5]->SetLogz(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_E9vsE1_AllURecHits%s.%s",dirName,runNumber,fileType); c[5]->Print(name); }

  c[6]->cd();
  gStyle->SetOptStat(10);
  OccupancyAllEventsCoarse->Draw("colz");
  OccupancyAllEventsCoarse->SetMinimum(1);
  OccupancyAllEventsCoarse->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEventsCoarse->GetYaxis()->SetNdivisions(2);
  c[6]->SetLogy(0);
  c[6]->SetLogz(1);
  c[6]->SetGridx(1);
  c[6]->SetGridy(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEventsCoarse%s.%s",dirName,runNumber,fileType); c[6]->Print(name); }
  
  c[7]->cd();
  OccupancyAllEvents->Draw("colz");
  OccupancyAllEvents->SetMinimum(1);  
  OccupancyAllEvents->GetXaxis()->SetNdivisions(-18);
  OccupancyAllEvents->GetYaxis()->SetNdivisions(2);
  c[7]->SetLogy(0);
  c[7]->SetLogz(1);
  c[7]->SetGridx(1);
  c[7]->SetGridy();
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_OccupancyAllEvents%s.%s",dirName,runNumber,fileType); c[7]->Print(name); }

  c[8]->cd();
  gStyle->SetOptStat(1110);
  JitterForAllFeds->Draw();
  c[8]->SetLogy(0);
  c[8]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_JitterforAllFeds%s.%s",dirName,runNumber,fileType); c[8]->Print(name); }

  c[9]->cd();
  JitterVsAmpAllEvents->Draw("colz");
  JitterVsAmpAllEvents->SetMinimum(1);
  c[9]->SetLogy(0);
  c[9]->SetLogz(1);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_JitterVsAmpAllFeds%s.%s",dirName,runNumber,fileType); c[9]->Print(name); }

  c[10]->cd();
  JitterPhiAllFEDs->ProfileX()->Draw();
  c[10]->SetLogy(0);
  c[10]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_JitterPhiAllFeds%s.%s",dirName,runNumber,fileType); c[10]->Print(name); }

  c[11]->cd();
  FrequencyAllEvent->Draw();
  c[11]->SetLogy(0);
  c[11]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_FrequencyAllEvent%s.%s",dirName,runNumber,fileType); c[11]->Print(name); }

  c[12]->cd();
  JitterVsFreqAllEvent->Draw("colz");
  JitterVsFreqAllEvent->SetMinimum(1);
  c[12]->SetLogy(0);
  c[12]->SetLogz(0);
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_JitterVsFreqAllFeds%s.%s",dirName,runNumber,fileType); c[12]->Print(name); }


  // fancy timing plots
  //
  gStyle->SetOptStat(10);

  cTiming = new TCanvas("cTiming","(phi,eta,timing)",900,600);
  cTiming->cd(1);
  cTiming->SetGridx();
  cTiming->SetGridy();

  TH3F* h1 = (TH3F*)f->Get("JitterPhiEtaAllFEDs");
  TProfile2D* ayx = (TProfile2D*) h1->Project3DProfile("yx");
  ayx->Draw("colz");
  ayx->GetXaxis()->SetTitle("i#phi");
  ayx->GetYaxis()->SetTitle("i#eta");

  ayx->GetXaxis()->SetNdivisions(-18);
  ayx->GetYaxis()->SetNdivisions(2);

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
  if (printPics) { sprintf(name,"%s/cosmicsAnalysis_JitterPhiEtaAllFEDs%s.%s",dirName,runNumber,fileType); cTiming->Print(name); }

  return;

}
