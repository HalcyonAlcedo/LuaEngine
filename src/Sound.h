#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <xaudio2.h>

class Sound
{
public:

	explicit Sound();
	virtual	~Sound();

	bool LoadFromFile(const std::string&);
	BYTE* Data() const;
	WAVEFORMATEX DataDescription();
	float DurationInSeconds() const;
	unsigned NumberOfSamples() const;
	unsigned Size() const;

protected:

	bool FindChunk(unsigned, unsigned&, unsigned&);
	bool ReadChunk(void*, unsigned, unsigned);
	void CalculateMetrics();

	WAVEFORMATEXTENSIBLE dataDesc;
	std::unique_ptr <BYTE[]> data;
	std::ifstream inputFile;

	unsigned samplesCount;
	unsigned dataSize;
	float secondsDuration;
};