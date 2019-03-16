#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
using namespace std;

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"

void Dat2Root()
{
	const int NWAV=8, NTR=1, NSAM=1024;
	
	unsigned int iEvt=0;
	unsigned int waveHead[NWAV][6]={0}, trigHead[NTR][6]={0};
	float wave[NWAV][NSAM]={0}, trig[NTR][NSAM]={0}, waveSP[NWAV][NSAM]={0}, trigSP[NWAV][NSAM]={0};	
	
	string strWaveName[NWAV], strTrigName[NTR], strDat, strRoot;
	FILE *ifWave[NWAV], *ifTrig[NTR];
	
	int i, j;
	int nW, nT;
	
	string strFilNam, strRead;
	ifstream fileName("listOfRuns.txt");
	while(!fileName.eof()&&fileName.peek()!=EOF)
	{
		if(fileName.peek()!='#')
		{
			fileName>>strFilNam;
			getline(fileName, strRead);
			
			for(nW=0; nW<NWAV; nW++)
			{
				strWaveName[nW]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_wave_"+to_string(nW)+".dat";
				if((ifWave[nW]=fopen(strWaveName[nW].c_str(), "rb"))==NULL) 
				{
					cout<<"Cannot open "<<strWaveName[nW]<<endl;
					break;
				}
			}
			if(nW<NWAV)
				continue;
			
			for(nT=0; nT<NTR; nT++)
			{
				strTrigName[nT]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_TR_0_"+to_string(nT)+".dat";
				if((ifTrig[nT]=fopen(strTrigName[nT].c_str(), "rb"))==NULL) 
				{
					cout<<"Cannot open "<<strTrigName[nT]<<endl;
					break;
				}
			}
			if(nT<NTR)
				continue;
			
			strDat="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_*.dat";
			strRoot="/home/enlab/WaveDump/RootData/"+strFilNam+".root";
			printf("*************************************************************************************\nNow converting %s to %s\n*************************************************************************************\n\n", strDat.c_str(), strRoot.c_str());
			
			TFile *fRoot=new TFile(strRoot.c_str(), "RECREATE");
			TTree *tData=new TTree("tData", "tree for digitizer");
			
			tData->Branch("iEvt", &iEvt, "iEvt/i");
			tData->Branch("waveHead", waveHead, ("waveHead["+to_string(NWAV)+"][6]/i").c_str());
			tData->Branch("wave", wave, ("wave["+to_string(NWAV)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("waveSP", waveSP, ("waveSP["+to_string(NWAV)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("trigHead", trigHead, ("trigHead["+to_string(NTR)+"][6]/i").c_str());
			tData->Branch("trig", trig, ("trig["+to_string(NTR)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("trigSP", trigSP, ("trigSP["+to_string(NTR)+"]["+to_string(NSAM)+"]/F").c_str());
			
			iEvt=0;
			do
			{
				memset(waveHead, 0, sizeof(waveHead));
				memset(wave, 0, sizeof(wave));
				memset(trigHead, 0, sizeof(trigHead));
				memset(trig, 0, sizeof(trig));
				
				for(i=0; i<NWAV; i++)
					for(j=0; j<NSAM; j++)
						waveSP[i][j]=j;
				for(i=0; i<NTR; i++)
					for(j=0; j<NSAM; j++)
						trigSP[i][j]=j;
				
				nW=0;
				while(nW<NWAV&&!feof(ifWave[nW]))
				{
					// cout<<strWaveName[nW]<<endl;
					if(fread(waveHead[nW], 4, 6, ifWave[nW]) != 6)
					{
						// cout<<"Reading error or end of "<<strWaveName[nW]<<endl;
						break;
					}
					if(fread(wave[nW], 4, NSAM, ifWave[nW])!=NSAM)
					{
						// cout<<"Reading error! or end of "<<strWaveName[nW]<<endl;
						break;
					}
					nW++;
				}

				nT=0;				
				while(nT<NTR&&!feof(ifTrig[nT]))
				{
					// cout<<strTrigName[nT]<<endl;
					if(fread(trigHead[nT], 4, 6, ifTrig[nT]) != 6)
					{
						// cout<<"Reading error or end of "<<strTrigName[nT]<<"\n";
						break;
					}
					if(fread(trig[nT], 4, NSAM, ifTrig[nT])!=NSAM)
					{
						// cout<<"Reading error or end of "<<strTrigName[nT]<<"\n";
						break;
					}
					nT++;
				}
				
				if(nW==NWAV&&nT==NTR)
				{
					tData->Fill();
					iEvt++;
				}
				
			} while(nW==NWAV&&nT==NTR);
			
			tData->Write();
			fRoot->Close();
			delete fRoot;

			for(i=0; i<NWAV; i++)
				fclose(ifWave[i]);
			
			for(i=0; i<nT; i++)
				fclose(ifTrig[i]);
		}
		else
			getline(fileName, strRead);	
	}
	fileName.close();
}

void StandaloneApplication(int argc, char** argv)
{
	Dat2Root();
}

int main(int argc, char** argv)
{
	TApplication app("ROOT Application", &argc, argv);
	StandaloneApplication(app.Argc(), app.Argv());
	// app.Run();
	return 0;
}
