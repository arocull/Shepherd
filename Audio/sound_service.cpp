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
    StopAllSounds();

    free(sounds);

    Mix_CloseAudio();
    Mix_Quit();
}


void SoundService::Tick(float DeltaTime) {
    for (int i = 0; i < MaxAudioChannels; i++) {
        if (sounds[i].sound) {
            if (Mix_Playing(sounds[i].channel) == 0)
                StopSound(&sounds[i]);
        }
    }
}
void SoundService::SetVolume(float volume) {
    Mix_Volume(-1, (int) (volume*128));
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