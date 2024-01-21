#include <CoreFoundation/CoreFoundation.h>
#include <AudioUnit/AudioUnit.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define FREQUENCY 220  // Lower frequency for a soothing tone
#define DURATION 10      // Duration of the soothing tone in seconds

// Function to generate audio samples for a soothing tone with fade-in and fade-out effects
void generateSoothingTone(float *samples, UInt32 numFrames) {
    double phaseIncrement = 2.0 * M_PI * FREQUENCY / SAMPLE_RATE;
    double phase = 0.0;

    // Calculate fade-in and fade-out duration in frames
    UInt32 fadeDurationFrames = SAMPLE_RATE * 0.1; // 10% of the tone duration

    for (UInt32 i = 0; i < numFrames; i++) {
        // Apply fade-in effect for the first few frames
        float volume = (i < fadeDurationFrames) ? (float)i / fadeDurationFrames : 1.0;

        // Apply fade-out effect for the last few frames
        if (i >= numFrames - fadeDurationFrames) {
            volume = (float)(numFrames - i) / fadeDurationFrames;
        }

        samples[i] = volume * sin(phase);

        // Fill the same sample for the right channel (both channels have the same value)
        samples[i + numFrames] = samples[i];

        phase += phaseIncrement;
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }
}

// Audio output callback function that uses pre-calculated soothing tone
static OSStatus AudioOutputCallback(void *inRefCon,
                                    AudioUnitRenderActionFlags *ioActionFlags,
                                    const AudioTimeStamp *inTimeStamp,
                                    UInt32 inBusNumber,
                                    UInt32 inNumberFrames,
                                    AudioBufferList *ioData) {
    float *outBufferLeft = (float *)ioData->mBuffers[0].mData;
    float *outBufferRight = (float *)ioData->mBuffers[1].mData;

    generateSoothingTone(outBufferLeft, inNumberFrames);
    generateSoothingTone(outBufferRight, inNumberFrames);

    return noErr;
}

int main() {
    // Create an AudioComponentDescription for the output audio unit
    AudioComponentDescription outputDesc;
    outputDesc.componentType = kAudioUnitType_Output;
    outputDesc.componentSubType = kAudioUnitSubType_DefaultOutput;
    outputDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    outputDesc.componentFlags = 0;
    outputDesc.componentFlagsMask = 0;

    // Find the default output audio unit
    AudioComponent outputComponent = AudioComponentFindNext(NULL, &outputDesc);
    if (outputComponent == NULL) {
        printf("Error: Unable to find the default output audio unit.\n");
        return 1;
    }

    // Create the output audio unit instance
    AudioUnit outputUnit;
    OSStatus status = AudioComponentInstanceNew(outputComponent, &outputUnit);
    if (status != noErr) {
        printf("Error: Unable to create the output audio unit instance.\n");
        return 1;
    }

    // Register the output audio callback function
    AURenderCallbackStruct input;
    input.inputProc = AudioOutputCallback;
    input.inputProcRefCon = NULL;

    status = AudioUnitSetProperty(outputUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &input, sizeof(input));
    if (status != noErr) {
        printf("Error: Unable to set the audio output callback.\n");
        return 1;
    }

    // Set the sample rate for the audio unit
    Float64 sampleRate = SAMPLE_RATE;
    status = AudioUnitSetProperty(outputUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, 0, &sampleRate, sizeof(sampleRate));
    if (status != noErr) {
        printf("Error: Unable to set the audio unit sample rate.\n");
        return 1;
    }

    // Initialize the output audio unit
    status = AudioUnitInitialize(outputUnit);
    if (status != noErr) {
        printf("Error: Unable to initialize the audio output unit.\n");
        return 1;
    }

    // Start the audio output process
    status = AudioOutputUnitStart(outputUnit);
    if (status != noErr) {
        printf("Error: Unable to start the audio output.\n");
        return 1;
    }

    // Wait for the soothing tone to play for the specified duration
    sleep(DURATION);

    // Stop the audio output process
    status = AudioOutputUnitStop(outputUnit);
    if (status != noErr) {
        printf("Error: Unable to stop the audio output.\n");
        return 1;
    }

    // Cleanup
    AudioUnitUninitialize(outputUnit);
    AudioComponentInstanceDispose(outputUnit);

    return 0;
}
