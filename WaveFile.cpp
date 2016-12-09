/**
* Assignment 4
* @author Daniel Van Leusen
* Student id: 10064708
* E-mail: danvanleusen@yahoo.co.uk
* @version Dec 8, 2016
* <p>
* This is a simple object with only primitive for instance variable.
*/
#include "501A4.h"
#include <math.h>
#include <stdio.h>
//#include <wtypes.h>
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

			if ((strncmp((LPCSTR)waveFmt.chunkId, "fmt", 3) == 0) && (waveFmt.audioFormat == 1))
			{
				while (waveFmt.chunkSize>16)
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
					if (strncmp((LPCSTR)chunkId, "data", 4) == 0)
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
		BYTE dataChunkid[4] = { 'd', 'a', 't', 'a' };
		fwrite(dataChunkid, sizeof(BYTE), 4, file);
		fwrite(&waveDataSize, sizeof(DWORD), 1, file);
		fwrite(waveData, sizeof(BYTE), waveDataSize, file);
		bResult = TRUE;

		fclose(file);
	}
	return bResult;
}
BOOL WaveFile::Convolve(WaveFile& wave)
{
	BOOL bResult = FALSE;

	// Check for valid sound data
	if (IsValid()  && wave.IsValid() )
	{
		// Check for valid sound format
		if ((waveFmt.numChannels == wave.GetChannels()) && (waveFmt.sampleRate == wave.GetSampleRate()) && (waveFmt.bitsPerSample == wave.GetBitsPerSample()))
		{
			// Mix .WAVs
			long sampleSize = min(waveDataSize, wave.GetSize()) / (waveFmt.bitsPerSample >> 3);
			switch (waveFmt.bitsPerSample)
			{
			case 8:
			{
				LPBYTE lpSrcData = wave.GetData();
				LPBYTE lpDstData = waveData;
				float gain = log10(20.0f);
				for (long i = 0; i<sampleSize; i++)
				{
					*lpDstData = (BYTE)(((*lpSrcData + *lpDstData) >> 1)*gain);
					lpSrcData++;
					lpDstData++;
				}
			}
			break;

			case 16:
			{
				LPWORD lpSrcData = (LPWORD)wave.GetData();
				LPWORD lpDstData = (LPWORD)waveData;
				for (long i = 0; i<sampleSize; i++)
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
			}
			break;
			}
			bResult = TRUE;
		}
	}

	return bResult;
}
