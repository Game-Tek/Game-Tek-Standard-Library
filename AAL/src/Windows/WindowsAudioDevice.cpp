#include "AAL/Platform/Windows/WindowsAudioDevice.h"

#include "AAL/AudioCore.h"

#include "GTSL/Assert.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

using namespace GTSL;
using namespace AAL;

void WindowsAudioDevice::Initialize(const CreateInfo& audioDeviceCreateInfo)
{
	CoInitializeEx(nullptr, 0);
	
	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));

	enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &endPoint);

	endPoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audioClient));
}

void WindowsAudioDevice::CreateAudioStream(StreamShareMode shareMode, MixFormat mixFormat)
{
	WAVEFORMATEXTENSIBLE pwfx;
	ZeroMemory(&pwfx, sizeof(WAVEFORMATEXTENSIBLE));
	pwfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	pwfx.Format.cbSize = 22;
	pwfx.Format.nChannels = 2;
	pwfx.Format.nSamplesPerSec = mixFormat.SamplesPerSecond;
	pwfx.Format.wBitsPerSample = pwfx.Samples.wValidBitsPerSample = mixFormat.BitsPerSample;
	pwfx.Format.nBlockAlign = mixFormat.GetFrameSize();
	if (mixFormat.BitsPerSample == 24) // Most devices using WASAPI prefer 24 bits padded to 32 bits per sample.
	{
		mixFormat.BitsPerSample = 32;
		pwfx.Format.nBlockAlign = mixFormat.GetFrameSize();
	}
	pwfx.SubFormat = GUID{ STATIC_KSDATAFORMAT_SUBTYPE_PCM };
	pwfx.Format.nAvgBytesPerSec = mixFormat.SamplesPerSecond * pwfx.Format.nBlockAlign;

	frameSize = pwfx.Format.nBlockAlign;
	
	_AUDCLNT_SHAREMODE win_share_mode{};
	switch (shareMode)
	{
	case StreamShareMode::EXCLUSIVE: win_share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE; break;
	case StreamShareMode::SHARED: win_share_mode = AUDCLNT_SHAREMODE_SHARED; break;
	}
	GTSL_ASSERT(audioClient->Initialize(win_share_mode, 0, 0, 0, reinterpret_cast<PWAVEFORMATEX>(&pwfx), nullptr) == S_OK, "Failed to initialize audio client with requested parameters!");

	audioClient->GetBufferSize(&bufferFrameCount);

	audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));

	//switch (pwfx.Format.nChannels)
	//{
	//case 1: channelCount = AudioChannelCount::CHANNELS_MONO; break;
	//case 2: channelCount = AudioChannelCount::CHANNELS_STEREO; break;
	//case 6: channelCount = AudioChannelCount::CHANNELS_5_1; break;
	//case 8: channelCount = AudioChannelCount::CHANNELS_7_1; break;
	//default:;
	//}
	//
	//switch (pwfx.Format.nSamplesPerSec)
	//{
	//case 44100: sampleRate = AudioSampleRate::KHZ_44_1; break;
	//case 48000: sampleRate = AudioSampleRate::KHZ_48; break;
	//case 96000: sampleRate = AudioSampleRate::KHZ_96; break;
	//default:;
	//}
	//
	//switch (pwfx.Format.wBitsPerSample)
	//{
	//case 8:  bitDepth = AudioBitDepth::BIT_DEPTH_8; break;
	//case 16: bitDepth = AudioBitDepth::BIT_DEPTH_16; break;
	//case 24: bitDepth = AudioBitDepth::BIT_DEPTH_24; break;
	//case 32: bitDepth = AudioBitDepth::BIT_DEPTH_32; break;
	//default:;
	//}
}

void WindowsAudioDevice::Destroy()
{
	SAFE_RELEASE(renderClient)
	SAFE_RELEASE(audioClient)
	SAFE_RELEASE(endPoint)
	SAFE_RELEASE(enumerator)

	CoUninitialize();
}

AudioDevice::MixFormat WindowsAudioDevice::GetMixFormat() const
{
	WAVEFORMATEXTENSIBLE* waveformatex;
	audioClient->GetMixFormat(reinterpret_cast<WAVEFORMATEX**>(&waveformatex));

	AudioDevice::MixFormat mixFormat;
	
	GTSL_ASSERT(waveformatex->Format.wFormatTag == WAVE_FORMAT_PCM, "Format mismatch!");
	
	mixFormat.NumberOfChannels = waveformatex->Format.nChannels;
	mixFormat.SamplesPerSecond = waveformatex->Format.nSamplesPerSec;
	mixFormat.BitsPerSample = waveformatex->Format.wBitsPerSample == 24 ? 32 : waveformatex->Format.wBitsPerSample; // Most devices using WASAPI prefer 24 bits padded to 32 bits per sample.

	CoTaskMemFree(waveformatex);

	return mixFormat;
}

bool WindowsAudioDevice::IsMixFormatSupported(StreamShareMode shareMode, AudioDevice::MixFormat mixFormat) const
{
	WAVEFORMATEX waveformatex; WAVEFORMATEXTENSIBLE* closestMatch;

	waveformatex.wFormatTag = WAVE_FORMAT_PCM;
	waveformatex.cbSize = 0; //extra data size if using WAVEFORMATEXTENSIBLE, this parameter is ignored since format is PCM but for correctness we set it to 0
	waveformatex.nBlockAlign = mixFormat.GetFrameSize();
	waveformatex.nChannels = mixFormat.NumberOfChannels;
	waveformatex.nSamplesPerSec = mixFormat.SamplesPerSecond;
	waveformatex.wBitsPerSample = mixFormat.BitsPerSample;
	waveformatex.nAvgBytesPerSec = waveformatex.nSamplesPerSec * waveformatex.nBlockAlign;

	bool result = false;
	
	switch (shareMode)
	{
	case StreamShareMode::SHARED:
		result = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &waveformatex, reinterpret_cast<WAVEFORMATEX**>(&closestMatch)) == S_OK;
		CoTaskMemFree(closestMatch);
		break;
		
	case StreamShareMode::EXCLUSIVE:
		result = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, &waveformatex, nullptr) == S_OK;
		break;
	}

	return result;
}

void WindowsAudioDevice::Start() const { audioClient->Start(); }

void WindowsAudioDevice::GetAvailableBufferFrames(uint32& availableBufferFrames) const
{
	UINT32 numFramesAvailable = 0;
	//For a shared-mode rendering stream, the padding value reported by GetCurrentPadding specifies the number of audio frames
	//that are queued up to play in the endpoint buffer. Before writing to the endpoint buffer, the client can calculate the
	//amount of available space in the buffer by subtracting the padding value from the buffer length.
	audioClient->GetCurrentPadding(&numFramesAvailable);

	availableBufferFrames = bufferFrameCount - numFramesAvailable;
}

void WindowsAudioDevice::GetBufferFrameCount(uint32& totalBufferFrames) const
{
	audioClient->GetBufferSize(&totalBufferFrames);
}

GTSL::Result<void*> WindowsAudioDevice::getBuffer(uint32 pushedSamples) const
{
	BYTE* bufferAddress = nullptr;
	auto result = renderClient->GetBuffer(pushedSamples, &bufferAddress);
	return GTSL::Result<void*>(GTSL::MoveRef((void*)bufferAddress), result == S_OK);
}

bool WindowsAudioDevice::realeaseBuffer(uint32 pushedSamples) const
{
	return renderClient->ReleaseBuffer(pushedSamples, 0) == S_OK;
}

void WindowsAudioDevice::Stop() const { audioClient->Stop(); }
