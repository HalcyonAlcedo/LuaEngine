#include "Sound.h"

#include <algorithm>
#include <cstring>

namespace {
constexpr DWORD MakeFourCC(char a, char b, char c, char d)
{
	return static_cast<DWORD>(static_cast<unsigned char>(a)) |
		(static_cast<DWORD>(static_cast<unsigned char>(b)) << 8) |
		(static_cast<DWORD>(static_cast<unsigned char>(c)) << 16) |
		(static_cast<DWORD>(static_cast<unsigned char>(d)) << 24);
}

constexpr DWORD fourccRIFF = MakeFourCC('R', 'I', 'F', 'F');
constexpr DWORD fourccDATA = MakeFourCC('d', 'a', 't', 'a');
constexpr DWORD fourccFMT = MakeFourCC('f', 'm', 't', ' ');
constexpr DWORD fourccWAVE = MakeFourCC('W', 'A', 'V', 'E');
}

Sound::Sound() = default;

Sound::~Sound()
{
	if (inputFile.is_open()) {
		inputFile.close();
	}
}

BYTE* Sound::Data() const
{
	return data.get();
}

WAVEFORMATEX Sound::DataDescription() const
{
	return dataDesc.Format;
}

unsigned Sound::NumberOfSamples() const
{
	return samplesCount;
}

float Sound::DurationInSeconds() const
{
	return secondsDuration;
}

unsigned Sound::Size() const
{
	return dataSize;
}

bool Sound::LoadFromFile(const std::string& filename)
{
	if (inputFile.is_open()) {
		inputFile.close();
	}

	data.reset();
	dataDesc = {};
	dataSize = 0;
	samplesCount = 0;
	secondsDuration = 0.0f;

	inputFile.open(filename, std::ios::binary | std::ios::in);
	if (!inputFile) {
		return false;
	}

	unsigned chunkSize = 0;
	unsigned chunkOffset = 0;
	DWORD chunkType = 0;

	if (!FindChunk(fourccRIFF, chunkSize, chunkOffset)) {
		return false;
	}
	if (!ReadChunk(&chunkType, chunkOffset, sizeof(chunkType))) {
		return false;
	}
	if (chunkType != fourccWAVE) {
		return false;
	}

	if (!FindChunk(fourccFMT, chunkSize, chunkOffset)) {
		return false;
	}
	const unsigned formatBytes = std::min<unsigned>(chunkSize, sizeof(dataDesc));
	if (!ReadChunk(&dataDesc, chunkOffset, formatBytes)) {
		return false;
	}

	if (!FindChunk(fourccDATA, chunkSize, chunkOffset)) {
		return false;
	}
	if (chunkSize == 0) {
		return false;
	}

	data = std::make_unique<BYTE[]>(chunkSize);
	if (!ReadChunk(data.get(), chunkOffset, chunkSize)) {
		data.reset();
		return false;
	}

	dataSize = chunkSize;
	CalculateMetrics();
	return true;
}

bool Sound::FindChunk(DWORD id, unsigned& size, unsigned& offset)
{
	if (!inputFile) {
		return false;
	}

	inputFile.clear();
	inputFile.seekg(0, std::ios::beg);
	offset = 0;
	size = 0;

	while (inputFile) {
		DWORD chunkId = 0;
		DWORD chunkSize = 0;

		inputFile.read(reinterpret_cast<char*>(&chunkId), sizeof(chunkId));
		inputFile.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		if (!inputFile) {
			break;
		}

		offset += sizeof(DWORD) * 2;
		if (chunkId == id) {
			size = chunkSize;
			return true;
		}

		if (chunkId == fourccRIFF) {
			inputFile.seekg(sizeof(DWORD), std::ios::cur);
			offset += sizeof(DWORD);
		}
		else {
			const unsigned paddedSize = chunkSize + (chunkSize & 1U);
			inputFile.seekg(paddedSize, std::ios::cur);
			offset += paddedSize;
		}
	}

	return false;
}

bool Sound::ReadChunk(void* buffer, unsigned offset, unsigned size)
{
	if (!buffer || size == 0 || !inputFile) {
		return false;
	}

	inputFile.clear();
	inputFile.seekg(offset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(buffer), size);
	return inputFile.good() || inputFile.gcount() == size;
}

void Sound::CalculateMetrics()
{
	if (dataDesc.Format.nBlockAlign == 0 || dataDesc.Format.nSamplesPerSec == 0) {
		samplesCount = 0;
		secondsDuration = 0.0f;
		return;
	}

	samplesCount = dataSize / dataDesc.Format.nBlockAlign;
	secondsDuration = static_cast<float>(samplesCount) / static_cast<float>(dataDesc.Format.nSamplesPerSec);
}
