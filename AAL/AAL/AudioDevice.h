#pragma once

#include "AudioCore.h"

namespace AAL
{
	/**
	 * \brief Interface for an audio device. Creates and manages an audio device, endpoint and buffer.
	 */
	class AudioDevice
	{
	public:
		AudioDevice() = default;
		~AudioDevice() = default;

		struct CreateInfo
		{
			StreamShareMode ShareMode;
			GTSL::uint32 BitDepth, Frequency;
		};

		AudioBitDepth GetBitDepth() const { return bitDepth; }
		AudioSampleRate GetSampleRate() const { return sampleRate; }
		AudioChannelCount GetChannelCount() const { return channelCount; }
		AudioOutputDeviceType GetOutputDeviceType() const { return outputDevice; }
		
	protected:
		AudioBitDepth bitDepth;
		AudioSampleRate sampleRate;
		AudioChannelCount channelCount;
		AudioOutputDeviceType outputDevice;

	};
}