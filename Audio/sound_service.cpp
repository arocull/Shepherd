#include "Audio/sound_service.h"


SoundService::SoundService() {
    if (Mix_Init(MIX_INIT_MP3) == -1)
        printf("Failed to Initiate SDL, %s\n", SDL_GetError());
	else {
        //Initialize SDL_mixer 
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1 ) {
            printf("Failed to Initiate SDL, %s\n", SDL_GetError());
        } else
            Initialized = true;
    }

    if (Initialized)
        SDL_PauseAudio(0);

    Mix_AllocateChannels(MaxAudioChannels);

    sounds = (SoundData*) calloc(MaxAudioChannels, sizeof(SoundData));
    for (int i = 0; i < MaxAudioChannels; i++) {
        sounds[i].sound = nullptr;
        sounds[i].channel = i+1;
        sounds[i].finished = true;
    }
}
void SoundService::CloseSoundService() {
    Mix_HaltMusic();
    StopAllSounds();

    free(sounds);

    if (music) Mix_FreeMusic(music);
    if (musicFade) Mix_FreeMusic(musicFade);

    Mix_CloseAudio();
    Mix_Quit();
}


void SoundService::Tick(float DeltaTime) {
    if (fading) {
        fadeTime += DeltaTime;
        SetVolume(lerp(fadeStartVolume, fadeEndVolume, min(fadeTime / fadeTimeLength, 1.0f)), fadeChannel);

        if (fadeTime >= fadeTimeLength) fading = false;
    }
    if (musicFading) {
        musicFadeTime += DeltaTime;
        SetVolumeMusic(lerp(musicFadeStartVolume, musicFadeEndVolume, min(musicFadeTime / musicFadeTimeLength, 1.0f)));

        if (musicFadeTime >= musicFadeTimeLength) musicFading = false;
    }

    if (musicTimer > 0 && musicFade) {
        musicTimer -= DeltaTime;
        if (musicTimer <= 0) {
            PlayMusic(musicFade);
            musicFade = nullptr;
        }
    }

    for (int i = 0; i < MaxAudioChannels; i++) {
        if (sounds[i].sound) {
            if (Mix_Playing(sounds[i].channel) == 0)
                StopSound(&sounds[i]);
        }
    }
}
void SoundService::SetVolume(float volume, int channel) {
    currentVolume = volume;
    Mix_Volume(channel, (int) (pow(volume * configuredVolume, 2) * 128));
}
void SoundService::FadeVolume(float newVolume, float fadeTime, int channel) {
    fadeChannel = channel;
    fadeStartVolume = currentVolume;
    fadeEndVolume = newVolume;
    fadeTimeLength = fadeTime;
    fadeTime = 0.0f;
    fading = true;
}


void SoundService::PlayMusic(const char* MusicFile, int loops) {
    Mix_HaltMusic();
    if (music) Mix_FreeMusic(music);
    music = Mix_LoadMUS(MusicFile);
    Mix_PlayMusic(music, loops);
}
void SoundService::PlayMusic(Mix_Music* newMusic, int loops) {
    Mix_HaltMusic();
    if (music) Mix_FreeMusic(music);
    music = newMusic;
    Mix_PlayMusic(music, loops);
}
void SoundService::FadeOutMusic(float fadeTime) {
    Mix_FadeOutMusic((int) (fadeTime * 1000));
}
void SoundService::FadeIntoMusic(float fadeTime, const char* MusicFile) {
    if (musicFade) Mix_FreeMusic(musicFade);
    musicFade = Mix_LoadMUS(MusicFile);
    FadeOutMusic(fadeTime);
    musicTimer = fadeTime;
}
void SoundService::QueueMusic(float timer, const char* MusicFile) {
    if (musicFade) Mix_FreeMusic(musicFade);
    musicFade = Mix_LoadMUS(MusicFile);
    musicTimer = timer;
}
void SoundService::SetVolumeMusic(float volume) {
    musicVolume = volume;
    Mix_VolumeMusic((int) (pow(volume * configuredVolume, 2) * 128));
}
void SoundService::FadeVolumeMusic(float newVolume, float fadeTime) {
    musicFadeStartVolume = musicVolume;
    musicFadeEndVolume = newVolume;
    musicFadeTimeLength = fadeTime;
    musicFadeTime = 0.0f;
    musicFading = true;
}


void SoundService::ConfiguredVolumeDecrease() {
    if (configuredVolume <= 0.03) {
        configuredVolume = 1.0f; // Wrap back to top
    } else {
        configuredVolume -= 0.2f;
        if (configuredVolume < 0.03) configuredVolume = 0.0f; // Set volume to zero if negative, with some float threshold
    }

    // TODO: Clean up volume so it is more consistent with fading
    SetVolume(currentVolume);
    SetVolumeMusic(musicVolume);

    #ifdef DEBUG_MODE
        printf("Current configured volume is %f\n", configuredVolume);
    #endif
}
float SoundService::ConfiguredVolumeGet() {
    return configuredVolume;
}


SoundData* SoundService::PlaySound(const char* FilePath) {
    if (!Initialized || DEBUG_AudioDisabled)
        return nullptr;
    
    #ifdef DEBUG_MODE
        printf("Listing currently playing audio:\n");
        for (int i = 0; i < MaxAudioChannels; i++) {
            printf("\tChannel %i is %i\n", sounds[i].channel, !sounds[i].finished);
        }
    #endif

    return LoadSound(FilePath);
}
SoundData* SoundService::LoadSound(const char* FilePath) {
    int sfxIndex = -1;
    for (int i = 0; i < MaxAudioChannels; i++) {
        if (sounds[i].finished == true) {
            sfxIndex = i;
            break;
        }
    }

    if (sfxIndex == -1) {
        printf("No available sound slot for sound %s !\n", FilePath);
        return nullptr;
    }

    SoundData* sfx = &sounds[sfxIndex];
    sfx->finished = false;

    sfx->sound = Mix_LoadWAV(FilePath);
	if (!sfx->sound) {
        printf("Failed to load sound, %s\n", Mix_GetError());
		sfx->finished = true;
    } else
        Mix_PlayChannel(sfx->channel, sfx->sound, 0);

    return sfx;
}
void SoundService::StopSound(SoundData* sound) {
    sound->finished = true;
    if (!sound->sound) return;

    Mix_Pause(sound->channel);
    Mix_FreeChunk(sound->sound);
    sound->sound = nullptr;
}
void SoundService::StopAllSounds() {
    for (int i = 0; i < MaxAudioChannels; i++) {
        StopSound(&sounds[i]);
    }
}