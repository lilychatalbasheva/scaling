//trying to plot number of events against energy for multiple jets (not leading jet)
//include your libraries/headers from main142.cc
#include "Pythia8/Pythia.h"
#include <iostream>  // needed for std::cin and std::cout
#include "TCanvas.h"
#include "TString.h"
#include "TH1.h" // ROOT, for histogramming.
#include "TFile.h" // ROOT, for saving file.
#include "TF1.h" // ROOT, for histogramming.
#include "TMath.h" // using it for pi function
#include "TApplication.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

using namespace Pythia8;

double pTmin_jet = 20; //only jets with pt>ptmin_jet are included in the vector jets; fastjet returns jets with only pt > ptmin_jet
int sanity_check = 0;

//define functions for four vectors
double Calculate_pX(double pT, double eta, double phi) {
    return (pT * TMath::Cos(phi));
}

double Calculate_pY(double pT, double eta, double phi) {
    return (pT * TMath::Sin(phi));
}

double Calculate_pZ(double pT, double eta, double phi) {
    return (pT * TMath::SinH(eta));
}

double Calculate_E(double pT, double eta, double phi) {
    double pZ = Calculate_pZ(pT, eta, phi);
    return TMath::Sqrt(pT * pT + pZ * pZ);
}

int main(){

  //TApplication app("app", &argc, argv); 

  int hbins=50;
  int hmax = 150;
  //ranges for pt_min and pt_max
  std::vector<std::pair<int, int>> pThatRanges = {
          {20, 40},
          {40, 100},
          {100, -1} 
      };
    std::vector<int> events_per_range;

  // define new histograms file
  TFile* outFile = new TFile("histograms.root", "RECREATE");
  TH1F* hTotal = new TH1F("hTotal", "Leading Jet pT; Jet pT; Number of Events", hbins, 0, hmax);
  
  TCanvas* c1 = new TCanvas("c1", "Energy Dist", 800, 600);

  // =============================================================================================================================================
  // FIRST LOOP FOR EACH RANGE OF PT IN HISTOGRAMMING
  // =============================================================================================================================================

      for (auto range : pThatRanges) {

        Pythia pythia;
        // pythia event generator settings, for a matrix of pt_min and pt_max that we will sum over in the end to get the histogram
        // initialize pythia for each range of pt set in the beginning
        pythia.readString("HardQCD:all=on");      
        pythia.readString("Beams:eCM = 13000.");

      int pmin = range.first;
      int pmax = range.second;

      // get user input on number of events for each pt min pt max range
          int events;
      std::cout << "Number of events for pTHat [" << range.first
                << ", " << (range.second > 0 ? std::to_string(range.second) : "∞") << "]: ";
      std::cin >> events;
      events_per_range.push_back(events);

      std::string minStr = "PhaseSpace:pTHatMin = " + std::to_string(pmin); // pythia reads matrix for generator instructions on min and max range
      pythia.readString(minStr);

          if (pmax > 0) {
          std::string maxStr = "PhaseSpace:pTHatMax = " + std::to_string(pmax);
          pythia.readString(maxStr);
        }

        // create a histogram hCurrent for each range and in the end all the histograms will be weighted and added to hTotal
      std::string hName = "hCurrent_" + std::to_string(pmin) + "_" + (pmax > 0 ? std::to_string(pmax) : "max");
      TH1F* hCurrent = new TH1F(hName.c_str(), "Jet Slice", hbins, 0, hmax);
      hCurrent->Sumw2();

      // setup jet algorithm
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, 0.4);
    std::vector<fastjet::PseudoJet> jet_components;
      // if Pythia fails to initialize, exit with error.
      if (!pythia.init()) return 1;

      //point of for loop is to generate events until pythia.next() returns false
      //and to create vectors jets with all final particles, then a jet clustering from algorithm

  // =============================================================================================================================================
  // SECOND TIER LOOP FOR EVENT GENERATION (i counter runs until )
  // =============================================================================================================================================

      for(int i=0; i<events; ++i){ // generate i number of events in pythia
    jet_components.clear();     
      if (!pythia.next()) continue;

  // =============================================================================================================================================
  // THIRD TIER LOOP FOR JET CLUSTERING (j counter runs through all particles in an event i and selects all final events and arranges them in a vector)
  // =============================================================================================================================================

          for(int j=0; j<pythia.event.size(); ++j){ // select final particles
              if(pythia.event[j].isFinal())
                  jet_components.push_back(fastjet::PseudoJet(pythia.event[j].px(), pythia.event[j].py(), pythia.event[j].pz(), pythia.event[j].e())); //adds particle to end of vector
          }

          fastjet::ClusterSequence cs(jet_components, jetDef); //makes a jet from the component particles from j-for loop for each i event
          std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(pTmin_jet); //filters through jets to ensure they are above ptmin_jet for each i event

          // Fill histogram for single-jet events
        if(jets.empty()){
        ++sanity_check;
        }

        if (!jets.empty()) {
  //we want to create a weighted histogram according to weighted count per bin = (# of events)/(luminosity*bin width)
  //since the number of events in each bin should be scaled accordingly
  //double sigma = pythia.info.sigmaGen();
  //double lum = events/sigma;
  //double weight = 1/(lum*binwidth);

      hCurrent->Fill(jets[0].pt());  // only fill with leading jet pt if a jet exists
      
      //hCurrent->Fill(pythia.info.pTHat());
    }

    }

  // check whether the area under the curve is equal to 1 after normalization
  // double area = hTotal->Integral();
  // std::cout << "Area under the histogram = " << area << std::endl;
  // std::cout << "Total cross section = " << pythia.info.sigmaGen() << std::endl;

  // check whether area and sigma are equal
  // std::cout << "entries in histogram / accepted events = " << hTotal->GetEntries() / (events + sanity_check) << std::endl;
  // std::cout << "events generated = " << events + sanity_check << std::endl;

  
       // scale histograms -- does it make sense to use the same weight/scale for every histogram?
      double binWidth = hCurrent->GetBinWidth(1);
      double sumofweights = pythia.info.weightSum();
      double sigma = pythia.info.sigmaGen();
     
      hCurrent->Scale(sigma/(sumofweights*binWidth));
      hTotal->Add(hCurrent);
      hCurrent->Reset();

    pythia.stat(); // error detection at end of program
  
  }
      // save to file asking for user input

  std::string filename;
  std::cout << "Enter a filename: ";
  std::cin >> filename;

  std::string fullPath = std::string(getenv("HOME")) + "/Downloads/" + filename + ".png";

  hTotal->Write();
  hTotal->Draw("HIST");
  c1->SaveAs(fullPath.c_str());
  outFile->Close();

  //app.Run();

  return 0;
    
   }
