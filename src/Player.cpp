#include "Player.h"

Player::Player()
{
	const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	comInitialized = SUCCEEDED(hr);
}

Player::~Player()
{
	stopRequested.store(true);
	if (audioSourceVoice) {
		audioSourceVoice->Stop(0);
	}
	if (playThread.joinable()) {
		playThread.join();
	}
	if (audioSourceVoice) {
		audioSourceVoice->DestroyVoice();
		audioSourceVoice = nullptr;
	}
	if (audioMasteringVoice) {
		audioMasteringVoice->DestroyVoice();
		audioMasteringVoice = nullptr;
	}
	if (audioDriver) {
		audioDriver->Release();
		audioDriver = nullptr;
	}
	if (comInitialized) {
		CoUninitialize();
	}
}

bool Player::Create()
{
	if (FAILED(XAudio2Create(&audioDriver, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		return false;
	}
	if (FAILED(audioDriver->CreateMasteringVoice(&audioMasteringVoice))) {
		return false;
	}
	return true;
}

bool Player::SetSound(std::shared_ptr<Sound> sound)
{
	if (!audioDriver || !sound || !sound->Data() || sound->Size() == 0) {
		return false;
	}

	if (audioSourceVoice) {
		audioSourceVoice->DestroyVoice();
		audioSourceVoice = nullptr;
	}

	soundRef = std::move(sound);
	audioDesc = soundRef->DataDescription();
	if (FAILED(audioDriver->CreateSourceVoice(&audioSourceVoice, &audioDesc))) {
		return false;
	}

	audioBuffer = {};
	audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	audioBuffer.AudioBytes = soundRef->Size();
	audioBuffer.pAudioData = soundRef->Data();

	if (FAILED(audioSourceVoice->FlushSourceBuffers())) {
		return false;
	}
	if (FAILED(audioSourceVoice->SubmitSourceBuffer(&audioBuffer))) {
		return false;
	}
	return true;
}

bool Player::Play()
{
	if (!audioSourceVoice) {
		return false;
	}
	if (FAILED(audioSourceVoice->Start())) {
		return false;
	}

	finished.store(false);
	stopRequested.store(false);
	playThread = std::thread(&Player::PlayBuffer, this);
	return true;
}

bool Player::IsFinished() const
{
	return finished.load();
}

void Player::PlayBuffer()
{
	while (!stopRequested.load()) {
		XAUDIO2_VOICE_STATE state{};
		audioSourceVoice->GetState(&state);
		if (state.BuffersQueued == 0) {
			break;
		}
		Sleep(10);
	}
	finished.store(true);
}
