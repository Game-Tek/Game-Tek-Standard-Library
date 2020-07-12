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

		struct AudioDeviceCreateInfo
		{
			StreamShareMode ShareMode;
		};

	protected:
		AudioBitDepth bitDepth;
		AudioSampleRate sampleRate;
		AudioChannelCount channelCount;
		AudioOutputDeviceType outputDevice;

	};
}