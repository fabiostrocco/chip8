#pragma once

#include <SDL_audio.h>
#include <limits>

namespace chip8
{
    using SampleType = Sint8;

    class AudioController
    {
      public:
        AudioController();
        ~AudioController();

        void play();
        void stop();

        static constexpr int SampleRate = 44100;
        static constexpr double Frequency = 440.0;
        static constexpr double Amplitude = std::numeric_limits<SampleType>::max() - 1;

      private:
        SDL_AudioDeviceID audioDeviceId;
        SDL_AudioSpec audioDeviceDesiredSpecs;
        SDL_AudioSpec audioDeviceObtainedSpecs;
        size_t sampleNumber;

        // Cannot copy this object as will open audio devices multiple times, which we don't want.
        AudioController(const AudioController&) = delete;
        AudioController& operator=(const AudioController&) = delete;
    };

    void audio_callback(void*, Uint8*, int);
}