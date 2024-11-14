#include <iostream>
#include <csignal>
#include <portaudio.h>
#include "ButterworthFilter.h"

const int SAMPLE_RATE = 44100;       // Sampling rate in Hz
const int FRAMES_PER_BUFFER = 256;   // Number of frames per buffer
const int NUM_CHANNELS = 1;          // Mono audio (1 channel)
const int PA_SAMPLE_TYPE = paFloat32; // Audio sample type (32-bit floating point)

bool keepRunning = true;  // Global flag for controlling main loop

// Signal handler to allow graceful shutdown
void handleSignal(int signal) {
    keepRunning = false;
}

static int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    ButterworthFilter* filter = static_cast<ButterworthFilter*>(userData);
    float* in = (float*)inputBuffer;
    float* out = (float*)outputBuffer;

    // Process each frame of audio
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        float inputSample = in[i];             // Get input sample
        float outputSample = filter->process(inputSample); // Filter the sample
        out[i] = outputSample;                 // Output the processed sample
    }

    return paContinue;
}

int main() {
    // Set up signal handling for graceful shutdown
    signal(SIGINT, handleSignal);

    // Set filter parameters
    double lowCut = 10.0;          // Low cutoff frequency
    double highCut = 20000.0;      // High cutoff frequency
    int order = 2;                 // Filter order
    ButterworthFilter filter(SAMPLE_RATE, lowCut, highCut, order);

    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Open the audio stream for input and output
    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream,
        NUM_CHANNELS,            // Input channels
        NUM_CHANNELS,            // Output channels
        PA_SAMPLE_TYPE,          // Sample type
        SAMPLE_RATE,             // Sample rate
        FRAMES_PER_BUFFER,       // Frames per buffer
        audioCallback,           // Callback function
        &filter);                // User data (Butterworth filter)
    if (err != paNoError) {
        std::cerr << "PortAudio stream opening failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    // Start the audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio stream start failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    std::cout << "Press Ctrl+C to stop the audio processing..." << std::endl;

    // Run the stream until interrupted
    while (keepRunning) {
        Pa_Sleep(100);  // Sleep to prevent busy-waiting
    }

    // Stop the audio stream and clean up
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio stream stop failed: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio stream close failed: " << Pa_GetErrorText(err) << std::endl;
    }

    Pa_Terminate();
    return 0;
}
