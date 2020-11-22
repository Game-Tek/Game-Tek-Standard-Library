#include "AAL/Platform/Windows/WindowsAudioDevice.h"

#include "AAL/AudioCore.h"

#include <mmdeviceapi.h>
#include <exception>

#include "GTSL/Assert.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

using namespace GTSL;
using namespace AAL;

void WindowsAudioDevice::Initialize(const CreateInfo& audioDeviceCreateInfo)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	
	ZeroMemory(&pwfx, sizeof(WAVEFORMATEXTENSIBLE));
	pwfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	pwfx.Format.cbSize = 22;
	pwfx.Format.nChannels = 2;
	pwfx.Format.nSamplesPerSec = audioDeviceCreateInfo.Frequency;
	pwfx.Format.wBitsPerSample = pwfx.Samples.wValidBitsPerSample = audioDeviceCreateInfo.BitDepth;
	pwfx.Format.nBlockAlign = 2u * (audioDeviceCreateInfo.BitDepth / 8u);
	if (audioDeviceCreateInfo.BitDepth == 24)
	{
		pwfx.Format.wBitsPerSample = 32;
		pwfx.Format.nBlockAlign = 2 * (32 / 8);
	}
	pwfx.SubFormat = GUID{ STATIC_KSDATAFORMAT_SUBTYPE_PCM };
	pwfx.Format.nAvgBytesPerSec = audioDeviceCreateInfo.Frequency * pwfx.Format.nBlockAlign;
	
	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));

	enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &endPoint);

	endPoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audioClient));

	_AUDCLNT_SHAREMODE win_share_mode{};
	switch (audioDeviceCreateInfo.ShareMode)
	{
	case StreamShareMode::EXCLUSIVE: win_share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE; break;
	case StreamShareMode::SHARED: win_share_mode = AUDCLNT_SHAREMODE_SHARED; break;
	}
	GTSL_ASSERT(audioClient->Initialize(win_share_mode, 0, 0, 0, reinterpret_cast<PWAVEFORMATEX>(&pwfx), nullptr) == S_OK, "Failed to initialize audio client with requested parameters!");

	audioClient->GetBufferSize(&bufferFrameCount);
	
	audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));

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

	CoUninitialize();
}

void WindowsAudioDevice::Start() const { audioClient->Start(); }

void WindowsAudioDevice::GetAvailableBufferFrames(uint64& availableBufferFrames) const
{
	UINT32 num_frames_padding = 0;
	//For a shared-mode rendering stream, the padding value reported by GetCurrentPadding specifies the number of audio frames
	//that are queued up to play in the endpoint buffer. Before writing to the endpoint buffer, the client can calculate the
	//amount of available space in the buffer by subtracting the padding value from the buffer length.
	audioClient->GetCurrentPadding(&num_frames_padding);

	availableBufferFrames = bufferFrameCount - num_frames_padding;
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
