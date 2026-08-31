#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "Sound.h"

class Player
{
public:
	explicit Player();
	~Player();

	bool Create();
	bool SetSound(std::shared_ptr<Sound> sound);
	bool Play();
	bool IsFinished() const;

private:
	void PlayBuffer();

	XAUDIO2_BUFFER audioBuffer{};
	WAVEFORMATEX audioDesc{};
	IXAudio2* audioDriver = nullptr;
	IXAudio2MasteringVoice* audioMasteringVoice = nullptr;
	IXAudio2SourceVoice* audioSourceVoice = nullptr;
	std::shared_ptr<Sound> soundRef;
	std::atomic_bool stopRequested{ false };
	std::atomic_bool finished{ true };
	std::thread playThread;
	bool comInitialized = false;
};
