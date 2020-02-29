#include "Audio/sound_service.h"


SoundService::SoundService() {
    if (Mix_Init(MIX_INIT_MP3) == -1)
        printf("Failed to Initiate SDL, %s\n", SDL_GetError());
	else {
        //Initialize SDL_mixer 
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 4096) == -1 ) {
            printf("Failed to Initiate SDL, %s\n", SDL_GetError());
        } else
            Initialized = true;
    }

    if (Initialized)
        SDL_PauseAudio(0);

    //Mix_AllocateChannels(MaxAudioChannels);

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
    if (TimeToDesiredVolume > 0) {
        TimeToDesiredVolume-=DeltaTime;
        float percent = 1.0f - (TimeToDesiredVolume / TimeToDesiredVolumeStart);
        Volume = VolumeStart*(1.0f-percent) + VolumeDesired*percent;
    }

    for (int i = 0; i < MaxAudioChannels; i++) {
        if (sounds[i].sound) {
            if (Mix_Playing(sounds[i].channel) == 0)
                StopSound(&sounds[i]);
        }
    }
}
void SoundService::FadeVolume(float desired, float time) {
    VolumeDesired = desired;
    VolumeStart = Volume;
    TimeToDesiredVolume = time;
    TimeToDesiredVolumeStart = time;
}



SoundData* SoundService::PlaySound(const char* FilePath) {
    if (!Initialized || DEBUG_AudioDisabled)
        return nullptr;
    else {
        SoundData* sfx = LoadSound(FilePath);

        return sfx;
    }
}
SoundData* SoundService::LoadSound(const char* FilePath) {
    int sfxIndex = 0;
    for (int i = 0; i < MaxAudioChannels; i++) {
        if (sounds[i].finished == true) {
            sfxIndex = i;
            break;
        }
    }

    SoundData sfx = sounds[sfxIndex];
    sfx.finished = false;

    sfx.sound = Mix_LoadWAV(FilePath);
	if (!sfx.sound) {
        printf("Failed to load sound, %s\n", Mix_GetError());
		sfx.finished = true;
    } else
        Mix_PlayChannel(sfx.channel, sfx.sound, 0);

    return &(sounds[sfxIndex]);
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