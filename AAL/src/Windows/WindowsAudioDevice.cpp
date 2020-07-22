#include "AAL/Platform/Windows/WindowsAudioDevice.h"

#include "AAL/AudioCore.h"

#include <mmdeviceapi.h>
#include <exception>

#include "GTSL/Assert.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

using namespace GTSL;
using namespace AAL;

WindowsAudioDevice::WindowsAudioDevice(const CreateInfo& audioDeviceCreateInfo)
{
	const auto CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const auto IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const auto IID_IAudioClient = __uuidof(IAudioClient);
	const auto IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

	CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&enumerator));

	//IMMDeviceCollection* audio_endpoints = nullptr;
	//enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &audio_endpoints);

	enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &endPoint);

	endPoint->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient));

	_AUDCLNT_SHAREMODE win_share_mode{};
	switch (audioDeviceCreateInfo.ShareMode)
	{
	case StreamShareMode::EXCLUSIVE: win_share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE; break;
	case StreamShareMode::SHARED: win_share_mode = AUDCLNT_SHAREMODE_SHARED; break;
	}
	audioClient->Initialize(win_share_mode, 0, 0, 0, reinterpret_cast<PWAVEFORMATEX>(&pwfx), nullptr);

	audioClient->GetService(IID_IAudioRenderClient, reinterpret_cast<void**>(&renderClient));

	audioClient->GetBufferSize(&bufferFrameCount);

	switch (pwfx.Format.nChannels)
	{
	case 1: channelCount = AudioChannelCount::CHANNELS_MONO; break;
	case 2: channelCount = AudioChannelCount::CHANNELS_STEREO; break;
	case 6: channelCount = AudioChannelCount::CHANNELS_5_1; break;
	case 8: channelCount = AudioChannelCount::CHANNELS_7_1; break;
	default:;
	}

	switch (pwfx.Format.nSamplesPerSec)
	{
	case 44100: sampleRate = AudioSampleRate::KHZ_44_1; break;
	case 48000: sampleRate = AudioSampleRate::KHZ_48; break;
	case 96000: sampleRate = AudioSampleRate::KHZ_96; break;
	default:;
	}

	switch (pwfx.Format.wBitsPerSample)
	{
	case 8: bitDepth = AudioBitDepth::BIT_DEPTH_8; break;
	case 16:bitDepth = AudioBitDepth::BIT_DEPTH_16; break;
	case 24:bitDepth = AudioBitDepth::BIT_DEPTH_24; break;
	default:;
	}
}

void WindowsAudioDevice::Initialize(const CreateInfo& audioDeviceCreateInfo)
{
	constexpr auto CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	constexpr auto IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	constexpr auto IID_IAudioClient = __uuidof(IAudioClient);
	constexpr auto IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

	CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&enumerator));

	enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &endPoint);

	endPoint->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient));

	_AUDCLNT_SHAREMODE win_share_mode{};
	switch (audioDeviceCreateInfo.ShareMode)
	{
	case StreamShareMode::EXCLUSIVE: win_share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE; break;
	case StreamShareMode::SHARED: win_share_mode = AUDCLNT_SHAREMODE_SHARED; break;
	}
	audioClient->Initialize(win_share_mode, 0, 0, 0, reinterpret_cast<PWAVEFORMATEX>(&pwfx), nullptr);

	audioClient->GetService(IID_IAudioRenderClient, reinterpret_cast<void**>(&renderClient));

	audioClient->GetBufferSize(&bufferFrameCount);

	switch (pwfx.Format.nChannels)
	{
	case 1: channelCount = AudioChannelCount::CHANNELS_MONO; break;
	case 2: channelCount = AudioChannelCount::CHANNELS_STEREO; break;
	case 6: channelCount = AudioChannelCount::CHANNELS_5_1; break;
	case 8: channelCount = AudioChannelCount::CHANNELS_7_1; break;
	default:;
	}

	switch (pwfx.Format.nSamplesPerSec)
	{
	case 44100: sampleRate = AudioSampleRate::KHZ_44_1; break;
	case 48000: sampleRate = AudioSampleRate::KHZ_48; break;
	case 96000: sampleRate = AudioSampleRate::KHZ_96; break;
	default:;
	}

	switch (pwfx.Format.wBitsPerSample)
	{
	case 8: bitDepth = AudioBitDepth::BIT_DEPTH_8; break;
	case 16:bitDepth = AudioBitDepth::BIT_DEPTH_16; break;
	case 24:bitDepth = AudioBitDepth::BIT_DEPTH_24; break;
	default:;
	}
}

void WindowsAudioDevice::Destroy()
{
	SAFE_RELEASE(renderClient)
	SAFE_RELEASE(audioClient)
	SAFE_RELEASE(endPoint)
	SAFE_RELEASE(enumerator)
}

void WindowsAudioDevice::Start() const { audioClient->Start(); }

void WindowsAudioDevice::GetAvailableBufferSize(uint64& availableBufferSize) const
{
	UINT32 num_frames_padding = 0;
	//For a shared-mode rendering stream, the padding value reported by GetCurrentPadding specifies the number of audio frames
	//that are queued up to play in the endpoint buffer. Before writing to the endpoint buffer, the client can calculate the
	//amount of available space in the buffer by subtracting the padding value from the buffer length.
	audioClient->GetCurrentPadding(&num_frames_padding);

	availableBufferSize = bufferFrameCount - num_frames_padding;
}

void WindowsAudioDevice::GetBufferSize(uint32& totalBufferSize) const
{
	audioClient->GetBufferSize(&totalBufferSize);
}

void WindowsAudioDevice::PushAudioData(void* data, const uint32 pushedSamples) const
{
	//Block alignment, in bytes.The block alignment is the minimum atomic unit of data for the wFormatTag format type.
	//If wFormatTag is WAVE_FORMAT_PCM or WAVE_FORMAT_EXTENSIBLE, nBlockAlign must be equal to the product of nChannels
	//and wBitsPerSample divided by 8 (bits per byte).For non - PCM formats, this member must be computed according to the
	//manufacturer's specification of the format tag.
	//Software must process a multiple of nBlockAlign bytes of data at a time.Data written to and read from a device
	//must always start at the beginning of a block.For example, it is illegal to start playback of PCM data in the middle
	//of a sample(that is, on a non - block - aligned boundary).

	// block_alignment = nChannels * (wBitsPerSample / 8);
	
	BYTE* buffer_address = nullptr;
	GTSL_ASSERT(renderClient->GetBuffer(pushedSamples, &buffer_address) == S_OK, "IAudioRenderClient::GetBuffer failed");
	memcpy(buffer_address, data, static_cast<uint64>(pushedSamples) * pwfx.Format.nBlockAlign);
	GTSL_ASSERT(renderClient->ReleaseBuffer(pushedSamples, 0) == S_OK, "IAudioRenderClient::ReleaseBuffer failed");
}

void WindowsAudioDevice::Stop() const { audioClient->Stop(); }
