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

		struct MixFormat
		{
			GTSL::uint8 NumberOfChannels;
			GTSL::uint32 SamplesPerSecond;
			GTSL::uint8 BitsPerSample;
			
			/**
			 * \brief Block alignment, in bytes. The block alignment is the minimum atomic unit of data for the format.
			 * nBlockAlign must be equal to the product of nChannels and * wBitsPerSample divided by 8 (bits per byte).
			 * Software must process a multiple of nBlockAlign bytes of data at a time. Data written to and read from a device must always start at the beginning of a block.
			 * For example, it is illegal to start playback of PCM data in the middle of a sample (that is, on a non-block-aligned boundary).
			 */
			GTSL::uint32 BlockAlign;
		};
		
		struct CreateInfo
		{
		};

	};
}