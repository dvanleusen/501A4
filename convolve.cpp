/**
* Assignment 4
* @author Daniel Van Leusen
* Student id: 10064708
* E-mail: danvanleusen@yahoo.co.uk
* @version Dec 8, 2016
* <p>
* This is a simple object with only primitive for instance variable.
*/

#include "convolve.h"
#include <stdio.h>
#include <ctime>
#include <iostream>
using namespace std;
//======================================================================
#define IGNORE_Last 
#ifndef USE_Test
WaveFile w_DryRecording, w_Impluse;
time_t  printTime()
{
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	cout << ( now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday << " "
		<< now->tm_hour << ":"
		<< now->tm_min << ":"
		<< now->tm_sec
		<< endl;
	return t;
}
int main(int argc, char *argv[])
{
	//_WAVE w = ReadWaveFile("DrumsDry.wav");
	if (argc < 4) {
		printf("%s \n", "parameter is wrong.");
		return 1;
	}
	cout << "Starting...: ";
	
	time_t startTime = printTime();

	char *drFile = argv[1];
	char *imFile = argv[2];
	char *outputFile = argv[3];
	bool bLevel1 = false;
#ifndef IGNORE_Last
	if (argc > 4)
		bLevel1= strncmp(argv[4], "true",4) == 0;
#endif
	w_DryRecording.ReadWaveFile(drFile);
	w_DryRecording.Convolve(imFile, outputFile, bLevel1);
	cout << "Convolution file is saved as \""
		<< outputFile
		<< "\" successfully."
		<< endl;
	
	cout << "End Time: ";
	time_t endTime  = printTime();
	//total cost time
	cout <<"Total cost time: "<< difftime(endTime, startTime)<<" seconds." << endl;
	cout << "Press Enter To Exit...";
	cin.get();
	return 0;
}
#endif



