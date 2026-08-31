#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <fstream>
#include <memory>
#include <string>
#include <xaudio2.h>

class Sound
{
public:
	explicit Sound();
	~Sound();

	bool LoadFromFile(const std::string& filename);
	BYTE* Data() const;
	WAVEFORMATEX DataDescription() const;
	float DurationInSeconds() const;
	unsigned NumberOfSamples() const;
	unsigned Size() const;

private:
	bool FindChunk(DWORD id, unsigned& size, unsigned& offset);
	bool ReadChunk(void* buffer, unsigned offset, unsigned size);
	void CalculateMetrics();

	WAVEFORMATEXTENSIBLE dataDesc{};
	std::unique_ptr<BYTE[]> data;
	std::ifstream inputFile;
	unsigned samplesCount = 0;
	unsigned dataSize = 0;
	float secondsDuration = 0.0f;
};
