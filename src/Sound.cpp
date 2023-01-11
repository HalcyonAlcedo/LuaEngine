#include "Sound.h"

#ifdef _XBOX
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

Sound::Sound() : samplesCount(0), dataSize(0), secondsDuration(0)
{
}


Sound::~Sound()
{
}


BYTE* Sound::Data() const
{
	return data.get();
}


WAVEFORMATEX Sound::DataDescription()
{
	return dataDesc.Format;
}


unsigned Sound::NumberOfSamples() const
{
	return samplesCount;
}


float Sound::DurationInSeconds() const
{
	return	secondsDuration;
}


unsigned Sound::Size() const
{
	return dataSize;
}


bool Sound::LoadFromFile(const std::string& Filename)
{
	inputFile.open(Filename, std::ios::binary | std::ios::in);
	if (!inputFile) return false;

	unsigned ChunkSize, ChunkOffset, ChunkType;

	if (!FindChunk(fourccRIFF, ChunkSize, ChunkOffset)) return false;
	if (!ReadChunk(&ChunkType, ChunkOffset, ChunkSize)) return false;
	if (ChunkType != fourccWAVE) return false;

	if (!FindChunk(fourccFMT, ChunkSize, ChunkOffset)) return false;
	if (!ReadChunk(&dataDesc, ChunkOffset, ChunkSize)) return false;

	if (!FindChunk(fourccDATA, ChunkSize, ChunkOffset)) return false;
	data.reset(new BYTE[ChunkSize]);
	if (!ReadChunk(data.get(), ChunkOffset, ChunkSize)) return false;

	dataSize = ChunkSize;
	CalculateMetrics();
	return true;
}


bool Sound::FindChunk(unsigned Id, unsigned& Size, unsigned& Offset)
{
	bool Completed = false;
	unsigned ChunkId, ChunkSize;
	Offset = 0;

	inputFile.seekg(0);

	while (inputFile)
	{
		inputFile.read((char*)&ChunkId, sizeof(ChunkId));
		inputFile.read((char*)&ChunkSize, sizeof(ChunkSize));

		if (ChunkId == fourccRIFF)
		{
			ChunkSize = sizeof(DWORD);
			inputFile.seekg(ChunkSize, inputFile.cur);
		}
		else inputFile.seekg(ChunkSize, inputFile.cur);

		Offset += sizeof(DWORD) * 2;
		if (ChunkId == Id) Completed = true, Size = ChunkSize;
		if (ChunkId == Id) break;

		Offset += ChunkSize;
	}
	return Completed;
}


bool Sound::ReadChunk(void* Buffer, unsigned Offset, unsigned Size)
{
	if (!Buffer || Size <= 0) return false;

	inputFile.seekg(Offset, inputFile.beg);
	inputFile.read((char*)Buffer, Size);

	return true;
}


void Sound::CalculateMetrics()
{
	samplesCount = dataSize / (dataDesc.Format.nBlockAlign);
	secondsDuration = float(samplesCount / dataDesc.Format.nSamplesPerSec);
}