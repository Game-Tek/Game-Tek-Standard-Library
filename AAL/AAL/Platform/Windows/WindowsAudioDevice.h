#pragma once

#include "AAL/AudioDevice.h"

#include <mmdeviceapi.h>
#include <Audioclient.h>

namespace AAL
{
	class WindowsAudioDevice final : public AudioDevice
	{
		IMMDeviceEnumerator* enumerator = nullptr;
		IMMDevice* endPoint = nullptr;
		IAudioClient* audioClient = nullptr;
		IAudioRenderClient* renderClient = nullptr;
		PWAVEFORMATEXTENSIBLE pwfx = nullptr;

		GTSL::uint32 bufferFrameCount = 0;
		void* data = nullptr;
	public:
		WindowsAudioDevice(const AudioDeviceCreateInfo& audioDeviceCreateInfo);
		virtual ~WindowsAudioDevice();

		void Start() override;
		void GetAvailableBufferSize(GTSL::uint64* availableBufferSize) override;
		void GetBufferSize(GTSL::uint32* totalBufferSize) override;
		void PushAudioData(void* data_, GTSL::uint64 pushedSamples) override;
		void Stop() override;
	};
}