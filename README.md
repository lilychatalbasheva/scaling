# scaling
quick code to scale different pThat ranges to get good statistics for less likely events (i.e. higher momentum jets)

Program asks for user input on how many events you want to generate per pThat range, in case you want to generate for example 1000 events with [20, 40GeV] and then 10,000 events for [40, 60GeV], etc. for less likely events. 

Ultimately, you can customize the pThat ranges to be something else instead of [20, 40], [40, 100], [100, inf]. But it's a good way to get statistics from different pThat ranges. 

The scaling formula used is taken from the Pythia website, but you can customize that as well, i.e. if you want to scale per number of jets or something else. 

    double binWidth = hCurrent->GetBinWidth(1);
    double sumofweights = pythia.info.weightSum();
    double sigma = pythia.info.sigmaGen();
     
    hCurrent->Scale(sigma/(sumofweights*binWidth));

The Makefile was created for some other programs (i.e. "random") as well, but you should be able to run "make graph5" with it to run this scaling code. 

Have fun!
