#include "AAL/AudioDevice.h"

#include "AAL/Platform/Windows/WindowsAudioDevice.h"

using namespace GTSL;
using namespace AAL;

AudioDevice::AudioDevice()
{
}

AudioDevice* AudioDevice::CreateAudioDevice(const AudioDeviceCreateInfo& audioDeviceCreateinfo)
{
#if (_WIN32)
	return new WindowsAudioDevice(audioDeviceCreateinfo);
#endif
}