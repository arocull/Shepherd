#pragma once

#include <cstdio>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "config.h"
#include "Core/mathutil.h"

// Note, use this tutorial: https://gist.github.com/armornick/3497064
// SDL_Mixer documentation: https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_frame.html

// A structure nested inside SDL_AudioSpec objects to help track information of the sound they're playing
struct SoundData {
	Mix_Chunk* sound;
    int channel;
    bool finished;
};

// Sound Service -- A general tool for playing and managing audio
class SoundService {
    public:
        SoundService();

        // Stops all sounds, de-allocates used memory, and closes any other sound functions
        void CloseSoundService();

    private:
        bool Initialized = false;

        SoundData* sounds;

        // Internal Play Sound function--do all verification beforehand, otherwise can result in crash
        SoundData* LoadSound(const char* FilePath);

    public:
        // Fades the current volume to the desired volume over the given amount of time
        void SetVolume(float volume);

        // Ticks the Sound Service
        // * Automatically stops sounds that have finished and fades volume
        void Tick(float DeltaTime);
        // Plays the given WAV file path and returns the sound object--returns null if could not play
        SoundData* PlaySound(const char* FilePath);
        // Stops the given sound object from playing
        void StopSound(SoundData* sound);
        // Stops all playing sounds
        void StopAllSounds();
};