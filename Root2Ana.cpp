#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphSmooth.h"
#include "TMath.h"

void Root2Ana()
{
	const int NWAV=8, NSAM=1024, NRS=20; //NRS is the Number of sampling points duiring RiSe time
	const double ST=200; //Sampling Time in unit of ps
	const double TH=150; //THreshold for valid signal in unit of channel
	const double P=0.2; //Percentage of amplitude for leading-edge timing
		
	float wave[NWAV][NSAM]={0};
	float waveSP[NWAV][NSAM]={0};
	double base[NWAV]={0}, amp[NWAV]={0}, time[NWAV]={0}, tD[NWAV][NWAV]={0};
	double amp1=0, amp2=0;
	string strRoot, strAna;
	
	long long iEnt=0;
	
	int iWv=0, iPk=0, nBs=0;
	
	int i=0, j=0;
	
	string strFilNam, strRead;
	ifstream fileName("listOfRuns.txt");
	while(!fileName.eof()&&fileName.peek()!=EOF)
	{
		if(fileName.peek()!='#')
		{
			//Reading input file with digitizer data
			fileName>>strFilNam;
			getline(fileName, strRead);
			
			strRoot="/home/enlab/WaveDump/RootData/"+strFilNam+".root";
			TFile *fRoot=new TFile(strRoot.c_str());
			if(!fRoot)
			{
				cout<<"Error in opening "<<strRoot<<"!\n";
				continue;
			}
			TTree *tData;
			fRoot->GetObject("tData", tData);
			if(!tData)
			{
				cout<<"Error in reading tree of tData\n";
				continue;
			}
			
			memset(wave, 0, sizeof(wave));
			memset(waveSP, 0, sizeof(waveSP));
			tData->SetBranchAddress("wave", wave);
			tData->SetBranchAddress("waveSP", waveSP);

			//file for output of analysis results			
			strAna="/home/enlab/WaveDump/AnaData/ana_"+strFilNam+".root";
			printf("*************************************************************************************\nNow converting %s to %s\n*************************************************************************************\n\n", strRoot.c_str(), strAna.c_str());
			

			TFile *fAna=new TFile(strAna.c_str(), "RECREATE");
			TTree *tAna=new TTree("tAna", "tree for analyzing digitizer");
			tAna->Branch("base", base, ("base["+to_string(NWAV)+"]/D").c_str());
			tAna->Branch("amp", amp, ("amp["+to_string(NWAV)+"]/D").c_str());
			tAna->Branch("time", time, ("time["+to_string(NWAV)+"]/D").c_str());
			tAna->Branch("tD", tD, ("tD["+to_string(NWAV)+"]["+to_string(NWAV)+"/D").c_str());
			
			//loops over each event in inpu file
			for(iEnt=0; iEnt<tData->GetEntries(); iEnt++)
			{
				tData->GetEntry(iEnt);
					//memset commands clears values in each array
				memset(base, 0, sizeof(base));
				memset(amp, 0, sizeof(amp));
				memset(time, 0, sizeof(time));
				memset(tD, 0, sizeof(tD));
				
				//YOUR CALCULATIONS BEGIN HERE!
				for(iWv=0; iWv<NWAV; iWv++)
				{
					TGraph *grWave=new TGraph(NSAM, waveSP[iWv], wave[iWv]);
					TGraphSmooth *gsWave = new TGraphSmooth();
					TGraph *grWaveSm = gsWave->SmoothKern(grWave,"normal",2, NSAM);
//					cout<<grWaveSm->GetN()<<endl;
					double *waveSm=grWaveSm->GetY();
					
					iPk=TMath::LocMin(grWaveSm->GetN(),waveSm);
					if(iPk>NRS)
						nBs=iPk-NRS;
					if(abs(waveSm[iPk]-TMath::Mean(waveSm, waveSm+nBs))>TH)
					{
						base[iWv]=TMath::Mean(waveSm, waveSm+nBs);
						
						amp[iWv]=abs(waveSm[iPk]-base[iWv]);
						
						amp1=0;
						amp2=0;
						for(i=nBs; i<iPk; i++)
						{
							amp1=abs(waveSm[i]-base[iWv]);
							amp2=abs(waveSm[i+1]-base[iWv]);
							if(amp1<=P*amp[iWv]&&amp2>=P*amp[iWv])
							{
								if(abs(amp2-amp1)>0)
									time[iWv]=ST*(i*(amp2-P*amp[iWv])+(i+1)*(P*amp[iWv]-amp1))/(amp2-amp1);
								else
									time[iWv]=ST*(2*i+1)/2.0;
							}
						}
					}					
				}
				// calculate time difference between any two channels
				for(i=0; i<NWAV-1; i++)
					for(j=i+1; j<NWAV; j++)
						if(time[i]>0&&time[j]>0)
						{
							tD[i][j]=time[i]-time[j];
							tD[j][i]=-tD[i][j];
						}
				//YOUR CALCULATIONS END HERE
				tAna->Fill();
			}
			tAna->Write();
			fAna->Close();
			delete fAna;
			fRoot->Close();
			delete fRoot;
		}	
		else
			getline(fileName, strRead);	
	}
	fileName.close();
}

void StandaloneApplication(int argc, char** argv)
{
	Root2Ana();
}

int main(int argc, char** argv)
{
	TApplication app("ROOT Application", &argc, argv);
	StandaloneApplication(app.Argc(), app.Argv());
	// app.Run();
	return 0;
}
