#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "Sound.h"

// 共享音频设备:所有 Player 共用一个 XAudio2 驱动与主控语音,
// 避免每次播放都创建/销毁驱动实例(每个驱动自带音频线程,反复
// 创建销毁既浪费资源又增加失败点)。
//
// 实现为堆分配且故意不释放:注入型模组在进程退出阶段销毁 XAudio2
// 可能因 loader lock 与音频线程互相等待而死锁,进程退出时由系统回收即可。
class AudioDevice
{
public:
	static AudioDevice& Instance() {
		// 故意泄漏:见类注释
		static AudioDevice* instance = new AudioDevice();
		return *instance;
	}

	bool Available() const { return driver != nullptr && mastering != nullptr; }
	IXAudio2* Driver() const { return driver; }
	IXAudio2MasteringVoice* Mastering() const { return mastering; }

private:
	AudioDevice() {
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(XAudio2Create(&driver, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
			driver = nullptr;
			return;
		}
		if (FAILED(driver->CreateMasteringVoice(&mastering))) {
			mastering = nullptr;
		}
	}

	IXAudio2* driver = nullptr;
	IXAudio2MasteringVoice* mastering = nullptr;
};

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
	// 非拥有指针,由共享 AudioDevice 持有
	IXAudio2* audioDriver = nullptr;
	IXAudio2MasteringVoice* audioMasteringVoice = nullptr;
	IXAudio2SourceVoice* audioSourceVoice = nullptr;
	std::shared_ptr<Sound> soundRef;
	std::atomic_bool stopRequested{ false };
	std::atomic_bool finished{ true };
	std::thread playThread;
};
