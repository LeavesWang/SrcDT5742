#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
using namespace std;

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"

void Txt2Root()
{
	const int NWAV=8, NTR=1, NSAM=1024;
	
	unsigned int iEvt=0;
	
	vector<string> *pWaveHead=0;
	vector<string> *pTrigHead=0;
	
	float wave[NWAV][NSAM]={0}, trig[NTR][NSAM]={0}, waveSP[NWAV][NSAM]={0}, trigSP[NWAV][NSAM]={0};
	
	string strWaveName[NWAV], strTrigName[NTR], strTxt, strRoot;
	ifstream ifWave[NWAV], ifTrig[NTR];
	
	int i, j;
	int nW, nT;
	
	string strWaveRead, strTrigRead;
	
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
				strWaveName[nW]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_wave_"+to_string(nW)+".txt";
				ifWave[nW].open(strWaveName[nW].c_str());
				if(!ifWave[nW].is_open())
				{
					cout<<"Cannot open "<<strWaveName[nW]<<endl;
					break;
				}
			}
			if(nW<NWAV)
				continue;

			for(nT=0; nT<NTR; nT++)
			{
				strTrigName[nT]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_TR_0_"+to_string(nT)+".txt";
				ifTrig[nT].open(strTrigName[nT].c_str());
				if(!ifTrig[nT].is_open())
				{
					cout<<"Cannot open "<<strTrigName[nT]<<endl;
					break;
				}
			}
			if(nT<NTR)
				continue;
							
			strTxt="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_*.txt";
			strRoot="/home/enlab/WaveDump/RootData/"+strFilNam+".root";
			printf("*************************************************************************************\nNow converting %s to %s\n*************************************************************************************\n\n", strTxt.c_str(), strRoot.c_str());
			
			TFile *fRoot=new TFile(strRoot.c_str(), "RECREATE");
			TTree *tData=new TTree("tData", "tree for digitizer");
			
			tData->Branch("iEvt", &iEvt, "iEvt/i");
			tData->Branch("waveHead", &pWaveHead);
			tData->Branch("wave", wave, ("wave["+to_string(NWAV)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("waveSP", waveSP, ("waveSP["+to_string(NWAV)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("trigHead", &pTrigHead);
			tData->Branch("trig", trig, ("trig["+to_string(NTR)+"]["+to_string(NSAM)+"]/F").c_str());
			tData->Branch("trigSP", trigSP, ("trigSP["+to_string(NTR)+"]["+to_string(NSAM)+"]/F").c_str());
			
			iEvt=0;
			do
			{
				pWaveHead->clear();
				pTrigHead->clear();
				memset(wave, 0, sizeof(wave));
				memset(trig, 0, sizeof(trig));
				
				for(i=0; i<NWAV; i++)
					for(j=0; j<NSAM; j++)
						waveSP[i][j]=j;
				for(i=0; i<NTR; i++)
					for(j=0; j<NSAM; j++)
						trigSP[i][j]=j;
				
				nW=0;
				while(nW<NWAV&&!ifWave[nW].eof()&&ifWave[nW].peek()!=EOF)
				{
					// cout<<strWaveName[nW]<<endl;
					strWaveRead="";
					for(i=0; i<8; i++)
					{
						getline(ifWave[nW], strRead);
						strWaveRead+=strRead+"  ";
					}
					pWaveHead->push_back(strWaveRead);
					// cout<<*(pWaveHead->begin()+nW)<<"\n\n";
					for(i=0; i<NSAM; i++)
					{
						ifWave[nW]>>wave[nW][i];
						getline(ifWave[nW], strRead);
					}
					nW++;
				}

				nT=0;	
				while(nT<NTR&&!ifTrig[nT].eof()&&ifTrig[nT].peek()!=EOF)
				{
					// cout<<strTrigName[nT]<<endl;
					strTrigRead="";
					for(i=0; i<8; i++)
					{
						getline(ifTrig[nT], strRead);
						strTrigRead+=strRead+"  ";
					}
					pTrigHead->push_back(strTrigRead);
					// cout<<*(pTrigHead->begin()+nT)<<"\n\n";
					for(i=0; i<NSAM; i++)
					{
						ifTrig[nT]>>trig[nT][i];
						getline(ifTrig[nT], strRead);
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
				ifWave[i].close();
				
			for(i=0; i<NTR; i++)
				ifTrig[i].close();
		}
		else
			getline(fileName, strRead);	
	}
	fileName.close();
}

void StandaloneApplication(int argc, char** argv)
{
	Txt2Root();
}

int main(int argc, char** argv)
{
	TApplication app("ROOT Application", &argc, argv);
	StandaloneApplication(app.Argc(), app.Argv());
	// app.Run();
	return 0;
}
