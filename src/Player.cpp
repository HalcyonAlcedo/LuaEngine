#include "Player.h"

Player::Player()
{
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
	// 驱动与主控语音由共享 AudioDevice 持有(进程生命周期),不在此销毁
}

bool Player::Create()
{
	AudioDevice& device = AudioDevice::Instance();
	audioDriver = device.Driver();
	audioMasteringVoice = device.Mastering();
	return audioDriver != nullptr && audioMasteringVoice != nullptr;
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
