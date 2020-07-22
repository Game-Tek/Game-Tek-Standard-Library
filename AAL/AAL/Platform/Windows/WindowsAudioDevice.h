#pragma once

#include "AAL/AudioDevice.h"

#include <mmdeviceapi.h>
#include <Audioclient.h>

namespace AAL
{
	class WindowsAudioDevice final : public AudioDevice
	{
	public:
		WindowsAudioDevice() = default;
		WindowsAudioDevice(const CreateInfo& audioDeviceCreateInfo);
		~WindowsAudioDevice() = default;

		void Initialize(const CreateInfo& audioDeviceCreateInfo);
		void Destroy();
		
		/**
		* \brief Initializes the audio device to start receiving audio. Must be called before any other function.
		*/
		void Start() const;
		
		/**
		* \brief Sets the passed variable as the available size in the allocated buffer.
		* Should be called to query the available size before filling the audio buffer size it may have some space still occupied since the audio driver may not have consumed it.
		* \param availableBufferSize Pointer to a variable to set as the available buffer size.
		*/
		void GetAvailableBufferSize(GTSL::uint64& availableBufferSize) const;

		/**
		* \brief Sets the passed variable as the size of the allocated buffer.
		* \param totalBufferSize Pointer to to variable for storing the size of the allocated buffer.
		*/
		void GetBufferSize(GTSL::uint32& totalBufferSize) const;
		
		/**
		* \brief Pushes the audio data found in the passed in buffer for the amount of specified samples to the audio buffer, making such data available for the next request from the driver to retrieve audio data.
		* \param data Pointer to the buffer containing the audio data to be used for filling the audio buffer.
		* \param pushedSamples Number of audio samples to copy from the passed pointer to the audio buffer.
		*/
		void PushAudioData(void* data, GTSL::uint32 pushedSamples) const;
		
		/**
		 * \brief Shutdowns and destroys the audio device resources. Must be called before destroying the audio device, no other functions shall be called after this.
		*/
		void Stop() const;

	private:
		/**
		 * \brief The IMMDeviceEnumerator interface provides methods for enumerating multimedia device resources.
		 * In the current implementation of the MMDevice API, the only device resources that this interface can enumerate are audio endpoint devices.
		 * A client obtains a reference to an IMMDeviceEnumerator interface by calling the CoCreateInstance function, as described previously (see MMDevice API).
		 */
		IMMDeviceEnumerator* enumerator = nullptr;
		
		/**
		 * \brief The IMMDevice interface encapsulates the generic features of a multimedia device resource.
		 * In the current implementation of the MMDevice API, the only type of device resource that an IMMDevice interface can represent is an audio endpoint device.
		 */
		IMMDevice* endPoint = nullptr;
		
		/**
		 * \brief The IAudioClient interface enables a client to create and initialize an audio stream between an audio application and the audio engine
		 * (for a shared-mode stream) or the hardware buffer of an audio endpoint device (for an exclusive-mode stream).
		 */
		IAudioClient* audioClient = nullptr;
		/**
		 * \brief The IAudioRenderClient interface enables a client to write output data to a rendering endpoint buffer.
		 * The client obtains a reference to the IAudioRenderClient interface of a stream object by calling the IAudioClient::GetService method
		 * with parameter riid set to REFIID IID_IAudioRenderClient.
		 */
		IAudioRenderClient* renderClient = nullptr;
		WAVEFORMATEXTENSIBLE pwfx;

		GTSL::uint32 bufferFrameCount = 0;
	};
}