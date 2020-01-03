#pragma once

#include <cstdio>

#include <SDL2/SDL.h>

#include "config.h"

// Note, use this tutorial: https://gist.github.com/armornick/3447121
// Perhaps look here as well https://github.com/jakebesworth/Simple-SDL2-Audio

struct SoundData {
	Uint8* audio_pos;	// global pointer to the audio buffer to be played
	Uint32 audio_len;	// remaining length of the sample we have to play
	Uint8* audio_buffer;
};

class SoundService {
    public:
        SoundService();

        void CloseSoundService();

    private:
        SDL_AudioSpec currentSound;
        bool PlayingAudio = false;

        SDL_AudioSpec LoadSound(char* FilePath);

    public:
        void Tick(float DeltaTime);
        SDL_AudioSpec* PlaySound(char* FilePath);
        void StopSound(SDL_AudioSpec* sound);
};

void SoundService_AudioCallback(void *userdata, unsigned char* stream, int len);