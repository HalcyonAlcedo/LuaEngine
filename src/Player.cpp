#include "Player.h"

Player::Player() : AudioDriver(nullptr), AudioMVoice(nullptr), AudioSVoice(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}


Player::~Player()
{
	break_flag.clear();
	play_thread.join();
	if (AudioSVoice) AudioSVoice->DestroyVoice();
	if (AudioMVoice) AudioMVoice->DestroyVoice();
	if (AudioDriver) AudioDriver->Release();
	CoUninitialize();
}


bool Player::Create()
{
	if (FAILED(XAudio2Create(&AudioDriver, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return false;

	if (FAILED(AudioDriver->CreateMasteringVoice(&AudioMVoice)))
		return false;

	return true;
}


bool Player::SetSound(Sound& sound)
{
	AudioDesc = sound.DataDescription();
	if (FAILED(AudioDriver->CreateSourceVoice(&AudioSVoice, &AudioDesc)))
		return false;

	AudioBuffer.LoopBegin = 0;
	AudioBuffer.LoopCount = 0;
	AudioBuffer.LoopLength = 0;
	AudioBuffer.pContext = nullptr;
	AudioBuffer.PlayBegin = 0;
	AudioBuffer.PlayLength = 0;
	AudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	AudioBuffer.AudioBytes = sound.Size();
	AudioBuffer.pAudioData = sound.Data();

	if (FAILED(AudioSVoice->FlushSourceBuffers()))
		return false;

	if (FAILED(AudioSVoice->SubmitSourceBuffer(&AudioBuffer)))
		return false;

	return true;
}


bool Player::Play()
{
	if (AudioSVoice == nullptr) return false;
	if (FAILED(AudioSVoice->Start())) return false;
	std::thread t(&Player::PlayBuffer, this);
	play_thread.swap(t);
	return true;
}


void Player::PlayBuffer()
{
	while (break_flag.test_and_set())
	{
		XAUDIO2_VOICE_STATE State;
		AudioSVoice->GetState(&State);
		if (State.BuffersQueued <= 0) break;
		Sleep(10);
	}
}