#include <CoreAudio/CoreAudio.h>
#include <assert.h>
#include <stdio.h>

void printCoreAudioError(OSStatus status) {
    char fourCC[16];
    *(UInt32 *)(fourCC + 1) = CFSwapInt32HostToBig(status);
    if (isprint(fourCC[1]) && isprint(fourCC[2]) && isprint(fourCC[3]) && isprint(fourCC[4])) {
        fourCC[0] = fourCC[5] = fourCC[6] = fourCC[7] = ' ';
        fourCC[8] = '\0';
    } else {
        // Print the integer value if not printable
        sprintf(fourCC, "%d", (int)status);
    }
}


int main()
{
    AudioObjectID defaultOutputDeviceID;
    UInt32 size = sizeof(defaultOutputDeviceID);
    AudioObjectPropertyAddress prop_default_output_device = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };

    // Get the default output device ID
    assert(kAudioHardwareNoError == AudioObjectGetPropertyData(kAudioObjectSystemObject, &prop_default_output_device, 0, NULL, &size, &defaultOutputDeviceID));

    printf("Device ID: %u\n", defaultOutputDeviceID);

    // Get the sample rate
    Float64 sampleRate;
    size = sizeof(sampleRate);
    AudioObjectPropertyAddress prop_sample_rate = { kAudioDevicePropertyNominalSampleRate, kAudioDevicePropertyScopeOutput, 1 };

    assert(kAudioHardwareNoError == AudioObjectGetPropertyData(defaultOutputDeviceID, &prop_sample_rate, 0, NULL, &size, &sampleRate));

    printf("Sample Rate: %f\n", sampleRate);

        // Get the volume of master channel
        Float32 CenterVolume;
        size = sizeof(CenterVolume);
        AudioObjectPropertyAddress prop_center_channel_volume = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 0 };
        assert(kAudioHardwareNoError == AudioObjectGetPropertyData(defaultOutputDeviceID, &prop_center_channel_volume, 0, NULL, &size, &CenterVolume));
        int centerint = CenterVolume*100;
        printf("Channel Volume: %d\n", centerint);

        // Get the volume for the left channel
        Float32 leftVolume;
        size = sizeof(leftVolume);
        AudioObjectPropertyAddress prop_left_channel_volume = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 2 };
        assert(kAudioHardwareNoError == AudioObjectGetPropertyData(defaultOutputDeviceID, &prop_left_channel_volume, 0, NULL, &size, &leftVolume));
        int leftint = leftVolume*100;
        printf("Left Channel Volume: %d\n", leftint);

        // Get the volume for the right channel
        Float32 rightVolume;
        size = sizeof(rightVolume);
        AudioObjectPropertyAddress prop_right_channel_volume = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 }; 
        assert(kAudioHardwareNoError == AudioObjectGetPropertyData(defaultOutputDeviceID, &prop_right_channel_volume, 0, NULL, &size, &rightVolume));
        int rightint = rightVolume*100;
        printf("Right Channel Volume: %d\n", rightint);






    return 0;
}
