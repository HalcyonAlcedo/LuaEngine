#include <thread>
#include <atomic>
#include "Sound.h"

class Player
{
public:

	explicit Player();
	virtual ~Player();

	bool SetSound(Sound&);
	bool Create();
	bool Play();

protected:

	void PlayBuffer();

	XAUDIO2_BUFFER AudioBuffer;
	WAVEFORMATEX AudioDesc;

	IXAudio2* AudioDriver;
	IXAudio2MasteringVoice* AudioMVoice;
	IXAudio2SourceVoice* AudioSVoice;

	std::atomic_flag break_flag;
	std::thread play_thread;
};