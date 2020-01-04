#pragma once

#include <cstdio>
#include <cmath>

#include <SDL2/SDL.h>

#include "config.h"
#include "Core/mathutil.h"

// Note, use this tutorial: https://gist.github.com/armornick/3447121
// Perhaps look here as well https://github.com/jakebesworth/Simple-SDL2-Audio

// A structure nested inside SDL_AudioSpec objects to help track information of the sound they're playing
struct SoundData {
	Uint8* audio_pos;	// global pointer to the audio buffer to be played
	Uint32 audio_len;	// remaining length of the sample we have to play
	Uint8* audio_buffer;
};

// Sound Service -- A general tool for playing and managing audio
class SoundService {
    public:
        SoundService();

        // Stops all sounds, de-allocates used memory, and closes any other sound functions
        void CloseSoundService();

    private:
        SDL_AudioSpec currentSound;
        bool PlayingAudio = false;

        // Internal Play Sound function--do all verification beforehand, otherwise can result in crash
        SDL_AudioSpec LoadSound(char* FilePath);

        float Volume = 1.0f;
        float VolumeStart = 1.0f;
        float VolumeDesired = 1.0f;
        float TimeToDesiredVolume = 0.0f;
        float TimeToDesiredVolumeStart = 0.0f;

    public:
        // Fades the current volume to the desired volume over the given amount of time
        void FadeVolume(float desired, float time);

        // Ticks the Sound Service
        // * Automatically stops sounds that have finished and fades volume
        void Tick(float DeltaTime);
        // Plays the given WAV file path and returns the sound object--returns null if could not play
        SDL_AudioSpec* PlaySound(char* FilePath);
        // Stops the given sound object from playing
        void StopSound(SDL_AudioSpec* sound);
        // Stops all playing sounds
        void StopAllSounds();
};

void SoundService_AudioCallback(void *userdata, unsigned char* stream, int len);