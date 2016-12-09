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
#include <math.h>
#include <stdio.h>
#include<corecrt_malloc.h>

WaveFile::WaveFile(void)
{
	// Init members
	memset(&waveRiff, 0, sizeof(_RIFF));
	memset(&waveFmt, 0, sizeof(_FMT));
	waveData = NULL;
	waveDataSize = 0;	
}
void WaveFile::ReadWaveFile(char *filename)
{	
	// read wav file
	FILE* file = fopen(filename, "rb");
	if (file != NULL)
	{
		// Read RIFF part
		fread(&waveRiff, sizeof(_RIFF), 1, file);

		// Check RIFF part format if is equal to "wave"
		if (strncmp((LPCSTR)waveRiff.wave, "WAVE", 4) == 0)
		{
			// Read fmt sub chunk
			fread(&waveFmt, sizeof(_FMT), 1, file);

			if ((strncmp((LPCSTR)waveFmt.chunkId, "fmt", 3) == 0))
			{
				if (waveFmt.audioFormat == 1)
				{
					while (waveFmt.chunkSize > 16)
					{
						BYTE unuse;
						fread(&unuse, sizeof(BYTE), 1, file);
						waveFmt.chunkSize--;
					}
					// Read sub chunk
					BYTE chunkId[4];
					DWORD chunkSize;
					fread(chunkId, sizeof(BYTE), 4, file);
					fread(&chunkSize, sizeof(DWORD), 1, file);
					DWORD fileOffset = ftell(file);

					// Read data
					LPBYTE tmpData = (LPBYTE)malloc(waveRiff.riffSize * sizeof(BYTE));
					while (fileOffset < waveRiff.riffSize)
					{
						bool b = strncmp((LPCSTR)chunkId, "data", 4) == 0;
						if (b)
						{
							if (waveData == NULL)
								waveData = (LPBYTE)malloc(chunkSize * sizeof(BYTE));
							else
								waveData = (LPBYTE)realloc(waveData, (waveDataSize + chunkSize) * sizeof(BYTE));
							fread(waveData + waveDataSize, sizeof(BYTE), chunkSize, file);
							waveDataSize += chunkSize;
						}
						else
							fread(tmpData, sizeof(BYTE), chunkSize, file);

						// Read next chunk
						fread(chunkId, sizeof(BYTE), 4, file);
						fread(&chunkSize, sizeof(DWORD), 1, file);
						fileOffset = ftell(file);
					}
					free(tmpData);
					waveRiff.riffSize = waveDataSize + 36;
				}
			}
		}
		// Close .WAV file
		fclose(file);
	}

}
BOOL WaveFile::WriteWaveFile(char *filename)
{
	BOOL bResult = FALSE;

	// Save .WAV file
	FILE* file = fopen(filename, "wb");
	if (file != NULL)
	{
		// write RIFF
		
		fwrite(&waveRiff, sizeof(_RIFF), 1, file);

		//write fmt 
		fwrite(&waveFmt, sizeof(_FMT), 1, file);

		// Write data
		BYTE dataChunkid[4];		
		dataChunkid[0] = 'd';
		dataChunkid[1] = 'a';
		dataChunkid[2] = 't';
		dataChunkid[3] = 'a';
		fwrite(dataChunkid, sizeof(BYTE), 4, file);
		fwrite(&waveDataSize, sizeof(DWORD), 1, file);
		fwrite(waveData, sizeof(BYTE), waveDataSize, file);
		bResult = TRUE;

		fclose(file);
	}
	return bResult;
}
BOOL WaveFile::WriteWaveFile(char *filename, int a[],long size) {
	BOOL bResult = FALSE;

	// Save .WAV file
	FILE* file = fopen(filename, "wb");
	if (file != NULL)
	{
		DWORD chunk2Size = size*waveFmt.numChannels*waveFmt.bitsPerSample / 8;
		waveRiff.riffSize = chunk2Size + 36;
		// write RIFF

		fwrite(&waveRiff, sizeof(_RIFF), 1, file);

		//write fmt 
		fwrite(&waveFmt, sizeof(_FMT), 1, file);

		// Write data
		BYTE dataChunkid[4] = { 'd', 'a', 't', 'a' };
		fwrite(dataChunkid, sizeof(BYTE), 4, file);
		fwrite(&chunk2Size, sizeof(DWORD), 1, file);
		for (int i = 0; i < size; i++) {
			unsigned char array[2];

			array[1] = (unsigned char)((a[i] >> 8) & 0xFF);
			array[0] = (unsigned char)(a[i] & 0xFF);
			fwrite(array, sizeof(unsigned char), 2, file);
		}

		bResult = TRUE;

		fclose(file);
	}
	return bResult;
}
LPBYTE wordToLpByte(WORD *dstData, long size) {
	BYTE *lp;
	lp = new BYTE[2 * size];
	for (int i = 0; i < size; i++) {
		unsigned char array[2];

		array[1] = (unsigned char)((dstData[i] >> 8) & 0xFF);
		array[0] = (unsigned char)(dstData[i] & 0xFF);
		lp[2 * i] = array[0];
		lp[2 * i + 1] = array[1];
	}
	return lp;
}
BOOL WaveFile::Convolve(char* impluseFile, char* outputFile,bool bLevel1)
{
	BOOL bResult = FALSE;
	WaveFile impluseWave;
	impluseWave.ReadWaveFile(impluseFile);
	// Check for valid sound data
	if (IsValid()  && impluseWave.IsValid() )
	{
		// Check for valid sound format
		if ((waveFmt.numChannels == impluseWave.GetChannels())
			&& (waveFmt.sampleRate == impluseWave.GetSampleRate())
			&& (waveFmt.bitsPerSample == impluseWave.GetBitsPerSample()))
		{
			LPWORD lpSrcData = (LPWORD)impluseWave.GetData();
			LPWORD lpDstData = (LPWORD)waveData;
			
			// Mix .WAVs
			if (bLevel1) {
				long lpSrcSize = impluseWave.GetSize() / (waveFmt.bitsPerSample >> 3);
				float *srcData;
				srcData = new float[lpSrcSize];

				for (long i = 0; i < lpSrcSize; i++) {
					srcData[i] = (lpSrcData[i] - 32768) / 32768.0f;
					//lpSrcData++;
				}
				long lpDstSize = waveDataSize / (waveFmt.bitsPerSample >> 3);
				float *dstData;
				dstData = new float[lpDstSize];

				for (long i = 0; i < lpDstSize; i++) {
					dstData[i] = (lpDstData[i] - 32768) / 32768.0f;
					//lpDstData++;
				}

				int *aDstData;
				aDstData = new int[lpSrcSize + lpDstSize - 1];
				for (long i = 0; i < lpSrcSize + lpDstSize - 1; i++)
					aDstData[i] = 0;
				for (long i = 0; i < lpSrcSize; i++) {
					for (long j = 0; j < lpDstSize; j++) {
						aDstData[i + j] = aDstData[i + j] + dstData[j] * srcData[i];
					}
				}
				WriteWaveFile(outputFile, &aDstData[0], lpSrcSize + lpDstSize - 1);
				
			}
			else {
			//DFTFFT
				long sampleSize = min(waveDataSize, impluseWave.GetSize()) / (waveFmt.bitsPerSample >> 3);

				for (long i = 0; i < sampleSize; i++)
				{
					float sample1 = (*lpSrcData - 32768) / 32768.0f;
					float sample2 = (*lpDstData - 32768) / 32768.0f;
					if (fabs(sample1*sample2) > 0.25f)
						*lpDstData = (WORD)(*lpSrcData + *lpDstData);
					else
						*lpDstData = fabs(sample1) < fabs(sample2) ? *lpSrcData : *lpDstData;
					lpSrcData++;
					lpDstData++;
				}
				WriteWaveFile(outputFile);
			}
			bResult = TRUE;
		}
		else {
			if (waveFmt.numChannels != impluseWave.GetChannels())
				printf("%s \n", "Number of Channels is differnce.");
			else if(waveFmt.sampleRate == impluseWave.GetSampleRate())
				printf("%s \n", "Sample Rate is differnce.");
			else if (waveFmt.bitsPerSample == impluseWave.GetBitsPerSample())
				printf("%s \n", "Bits Per Sample is differnce.");
		}
	}

	return bResult;
}

