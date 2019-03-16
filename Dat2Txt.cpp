#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
using namespace std;

int main(int argc, char** argv)
{
	const int NWAV=8, NTR=1, NSAM=1024;
	
	unsigned int waveHead[NWAV][6]={0}, trigHead[NTR][6]={0};
	
	float wave[NWAV][NSAM]={0}, trig[NTR][NSAM]={0};
	
	string strWaveDat[NWAV], strTrigDat[NTR];
	string strWaveTxt[NWAV], strTrigTxt[NTR];
	string strDat, strTxt;
	FILE *fWaveDat[NWAV], *fTrigDat[NTR];
	FILE *fWaveTxt[NWAV], *fTrigTxt[NTR];
	
	int i;
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
				strWaveDat[nW]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_wave_"+to_string(nW)+".dat";
				if((fWaveDat[nW]=fopen(strWaveDat[nW].c_str(), "rb"))==NULL) 
				{
					cout<<"Cannot open "<<strWaveDat[nW]<<endl;
					break;
				}
				strWaveTxt[nW]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_wave_"+to_string(nW)+".txt";
				fWaveTxt[nW]=fopen(strWaveTxt[nW].c_str(), "w");
			}
			if(nW<NWAV)
				continue;
			
			for(nT=0; nT<NTR; nT++)
			{
				strTrigDat[nT]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_TR_0_"+to_string(nT)+".dat";
				if((fTrigDat[nT]=fopen(strTrigDat[nT].c_str(), "rb"))==NULL) 
				{
					cout<<"Cannot open "<<strTrigDat[nT]<<endl;
					break;
				}
				strTrigTxt[nT]="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_TR_0_"+to_string(nT)+".txt";
				fTrigTxt[nT]=fopen(strTrigTxt[nT].c_str(), "w");
			}
			if(nT<NTR)
				continue;
			
			strDat="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_*.dat";
			strTxt="/home/enlab/WaveDump/RawData/"+strFilNam+"/"+strFilNam+"_*.txt";
			printf("*************************************************************************************\nNow converting %s to %s\n*************************************************************************************\n\n", strDat.c_str(), strTxt.c_str());
			
			do
			{
				memset(waveHead, 0, sizeof(waveHead));
				memset(wave, 0, sizeof(wave));
				memset(trigHead, 0, sizeof(trigHead));
				memset(trig, 0, sizeof(trig));
				
				nW=0;
				// ns=0;
				while(nW<NWAV&&!feof(fWaveDat[nW]))
				{
					// cout<<strWaveDat[nW]<<endl;
					if(fread(waveHead[nW], 4, 6, fWaveDat[nW]) != 6)
					{
						// cout<<"Reading error or end of "<<strWaveDat[nW]<<endl;
						break;
					}
					// ns=(int)fread(wave[nW], 4, NSAM, fWaveDat[nW]);
					if(fread(wave[nW], 4, NSAM, fWaveDat[nW])!=NSAM)
					{
						// cout<<"Reading error! or end of "<<strWaveDat[nW]<<endl;
						break;
					}
					fprintf(fWaveTxt[nW], "Record Length: %d\n", (waveHead[nW][0]-24)/4);
					fprintf(fWaveTxt[nW], "BoardID: %2d\n", waveHead[nW][1]);
					fprintf(fWaveTxt[nW], "Channel: %d\n", waveHead[nW][3]);
					fprintf(fWaveTxt[nW], "Event Number: %d\n", waveHead[nW][4]);
					fprintf(fWaveTxt[nW], "Pattern: 0x%04X\n", waveHead[nW][2] & 0xFFFF);
					fprintf(fWaveTxt[nW], "Trigger Time Stamp: %u\n", waveHead[nW][5]);
					fprintf(fWaveTxt[nW], "DC offset (DAC): NULL\n");
					fprintf(fWaveTxt[nW], "Start Index Cell: NULL\n");
					for(i=0; i<NSAM; i++)
						fprintf(fWaveTxt[nW], "%f\n", wave[nW][i]);
					nW++;
				}

				nT=0;				
				while(nT<NTR&&!feof(fTrigDat[nT]))
				{
					// cout<<strTrigDat[nT]<<endl;
					if(fread(trigHead[nT], 4, 6, fTrigDat[nT]) != 6)
					{
						// cout<<"Reading error or end of "<<strTrigDat[nT]<<"\n";
						break;
					}
					if(fread(trig[nT], 4, NSAM, fTrigDat[nT])!=NSAM)
					{
						// cout<<"Reading error or end of "<<strTrigDat[nT]<<"\n";
						break;
					}
					fprintf(fTrigTxt[nT], "Record Length: %d\n", trigHead[nT][0]);
					fprintf(fTrigTxt[nT], "BoardID: %2d\n", trigHead[nT][1]);
					fprintf(fTrigTxt[nT], "Channel: %d\n", trigHead[nT][3]);
					fprintf(fTrigTxt[nT], "Event Number: %d\n", trigHead[nT][4]);
					fprintf(fTrigTxt[nT], "Pattern: 0x%04X\n", trigHead[nT][2] & 0xFFFF);
					fprintf(fTrigTxt[nT], "Trigger Time Stamp: %u\n", trigHead[nT][5]);
					fprintf(fTrigTxt[nT], "DC offset (DAC): NULL\n");
					fprintf(fTrigTxt[nT], "Start Index Cell: NULL\n");
					for(i=0; i<NSAM; i++)
						fprintf(fTrigTxt[nT], "%f\n", trig[nT][i]);
					nT++;
				}				
			} while(nW==NWAV&&nT==NTR);

			for(i=0; i<NWAV; i++)
			{
				fclose(fWaveTxt[i]);
				fclose(fWaveDat[i]);
			}
			
			for(i=0; i<nT; i++)
			{
				fclose(fTrigTxt[i]);
				fclose(fTrigDat[i]);
			}
		}
		else
			getline(fileName, strRead);	
	}
	fileName.close();
	
	return 0;
}
