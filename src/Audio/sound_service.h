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

        float configuredVolume = 1.0f;

        float currentVolume = 1.0f;
        float fadeStartVolume = 1.0f;
        float fadeEndVolume = 1.0f;
        float fadeTimeLength = 0.0f;
        float fadeTime = 0.0f;
        bool fading = false;
        int fadeChannel = -1;

        float musicVolume = 1.0f;
        float musicFadeStartVolume = 1.0f;
        float musicFadeEndVolume = 1.0f;
        float musicFadeTimeLength = 0.0f;
        float musicFadeTime = 0.0f;
        bool musicFading = false;

        Mix_Music* music = nullptr;
        Mix_Music* musicFade = nullptr;
        float musicTimer = 0.0f;

        SoundData* sounds;

        // Internal Play Sound function--do all verification beforehand, otherwise can result in crash
        SoundData* LoadSound(const char* FilePath);

    public:
        // Sets the SDL Mixer volume
        void SetVolume(float volume, int channel = -1);
        // Fades the current volume to the desired volume over the given amount of time
        void FadeVolume(float newVolume, float fadeTime, int channel = -1);

        // Plays given music file for a set or infinite amount of loops
        void PlayMusic(const char* MusicFile, int loops = -1);
        void PlayMusic(Mix_Music* newMusic, int loops = -1);
        // Fades out music
        void FadeOutMusic(float fadeTime);
        // Fades out current music into new music after a set amount of time
        void FadeIntoMusic(float fadeTime, const char* MusicFile);
        // Starts playing the given music file after a set amount of time (used for song intros + looping)
        void QueueMusic(float fadeTime, const char* MusicFile);
        // Sets the SDL Mixer volume of the music
        void SetVolumeMusic(float volume);
        // Fades the music volume to a certain value over a set amount of time
        void FadeVolumeMusic(float newVolume, float fadeTime);

        void ConfiguredVolumeDecrease();
        float ConfiguredVolumeGet();

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