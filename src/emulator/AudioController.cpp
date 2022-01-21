#include "AudioController.hpp"

#include <numbers>

#include "AudioException.hpp"

void chip8::audio_callback(void* userData, Uint8* rawBuffer, int bufferLength)
{
    SampleType* buffer = reinterpret_cast<SampleType*>(rawBuffer);
    int numberOfSamples = bufferLength / sizeof(SampleType); // Number of samples = buffer length / sample size
    size_t& sampleNumber = *reinterpret_cast<size_t*>(userData);

    for (size_t i = 0; i < numberOfSamples; i++, sampleNumber++)
    {
        const double time = static_cast<double>(sampleNumber) / static_cast<double>(AudioController::SampleRate);
        buffer[i] = static_cast<SampleType>(AudioController::Amplitude * sin(2.0f * std::numbers::pi * AudioController::Frequency * time));
    }
}

chip8::AudioController::AudioController()
    : sampleNumber(0)
{
    audioDeviceDesiredSpecs.format = AUDIO_S8;
    audioDeviceDesiredSpecs.freq = AudioController::SampleRate; // Sampling frequency
    audioDeviceDesiredSpecs.channels = 1;
    audioDeviceDesiredSpecs.samples = 4096; // Audio buffer size = number of frames * number of channels * size of frame (U8 is 8-bit in this case).
    audioDeviceDesiredSpecs.callback = audio_callback;
    audioDeviceDesiredSpecs.userdata = &sampleNumber;

    audioDeviceId = SDL_OpenAudioDevice(nullptr, false, &audioDeviceDesiredSpecs, &audioDeviceObtainedSpecs, 0);
    if (audioDeviceId == 0)
    {
        throw chip8::AudioException("Cannot open default audio device " + std::string(SDL_GetError()));
    }
}

chip8::AudioController::~AudioController()
{
    SDL_CloseAudioDevice(audioDeviceId);
}

void chip8::AudioController::play()
{
    SDL_PauseAudioDevice(audioDeviceId, false);
}

void chip8::AudioController::stop()
{
    sampleNumber = 0;
    SDL_PauseAudioDevice(audioDeviceId, true);
}
