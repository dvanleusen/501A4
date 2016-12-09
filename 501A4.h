/**
* Assignment 4
* @author Daniel Van Leusen
* Student id: 10064708
* E-mail: danvanleusen@yahoo.co.uk
* @version Dec 8, 2016
* <p>
* This is a simple object with only primitive for instance variable.
*/
#include <wtypes.h>

#define USE_Test                  

typedef struct __RIFF
{
	BYTE riff[4];
	DWORD riffSize;
	BYTE wave[4];

} _RIFF, *_LPRIFF;

typedef struct __FMT
{
	BYTE chunkId[4];
	DWORD chunkSize;
	SHORT audioFormat;
	SHORT numChannels;
	DWORD sampleRate;
	DWORD byteRate;
	SHORT blockAlign;
	SHORT bitsPerSample;

} _FMT, *_LPFMT;

class WaveFile
{
public:
	WaveFile(void);
	virtual ~WaveFile(void) {};

public:
	// Public methods
	void ReadWaveFile(char *filename);
	BOOL WriteWaveFile(char *filename);
	BOOL WriteWaveFile(char *filename,int a[],  long size);
	BOOL IsValid() { return (waveData != NULL); }
	LPBYTE GetData() { return waveData; }
	DWORD GetSize() { return waveDataSize; }
	SHORT GetChannels() { return waveFmt.numChannels; }
	DWORD GetSampleRate() { return waveFmt.sampleRate; }
	SHORT GetBitsPerSample() { return waveFmt.bitsPerSample; }
	BOOL Convolve(char* impluseFile, char* outputFile, bool bLevel1);

private:
	// Private members
	_RIFF waveRiff;
	_FMT waveFmt;
	LPBYTE waveData;
	DWORD waveDataSize;
};
