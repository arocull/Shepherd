#include "Audio/sound_service.h"


int AudioVolume = 128;


SoundService::SoundService() {
    
}
void SoundService::CloseSoundService() {
    if (PlayingAudio)
        StopSound(&currentSound);
}


void SoundService::Tick(float DeltaTime) {
    if (TimeToDesiredVolume > 0) {
        TimeToDesiredVolume-=DeltaTime;
        float percent = 1.0f - (TimeToDesiredVolume / TimeToDesiredVolumeStart);
        Volume = VolumeStart*(1.0f-percent) + VolumeDesired*percent;
        AudioVolume = max(min((int) (SDL_MIX_MAXVOLUME*log(SDL_MIX_MAXVOLUME*Volume + 1.0f)/4.87f), SDL_MIX_MAXVOLUME), 0);
    }

    if (PlayingAudio) {
        if (((struct SoundData*)currentSound.userdata)->audio_len <= 0) {
            StopSound(&currentSound);
        }
    }
}
void SoundService::FadeVolume(float desired, float time) {
    VolumeDesired = desired;
    VolumeStart = Volume;
    TimeToDesiredVolume = time;
    TimeToDesiredVolumeStart = time;
}



SDL_AudioSpec* SoundService::PlaySound(char* FilePath) {
    if (PlayingAudio == true)
        return nullptr;
    else {
        currentSound = LoadSound(FilePath);
        PlayingAudio = true;

        return &currentSound;
    }
}
SDL_AudioSpec SoundService::LoadSound(char* FilePath) {
    SDL_AudioSpec wav_specifications;
	static Uint32 wav_length;
	static Uint8* wav_buffer;

	if (SDL_LoadWAV(FilePath, &wav_specifications, &wav_buffer, &wav_length) == NULL)
		exit(1);
	
	wav_specifications.callback = SoundService_AudioCallback;
	struct SoundData* data = (struct SoundData*) calloc(1, sizeof(struct SoundData));
	wav_specifications.userdata = data;
	data->audio_pos = wav_buffer;
	data->audio_buffer = wav_buffer;
	data->audio_len = wav_length;

	if (SDL_OpenAudio(&wav_specifications, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

    SDL_PauseAudio(0);

	return wav_specifications;
}
void SoundService::StopSound(SDL_AudioSpec* sound) {
    SDL_CloseAudio();

	struct SoundData* data = (struct SoundData*) sound->userdata;
	SDL_FreeWAV(data->audio_buffer);
	free(data);

    PlayingAudio = false;
}




void SoundService_AudioCallback(void *userdata, unsigned char* stream, int len) {
    struct SoundData* data = (struct SoundData*) userdata;

	if (data->audio_len <= 0)
		return;
	
	len = ( len > data->audio_len ? data->audio_len : len );
	SDL_memcpy(stream, data->audio_pos, len); 						// simply copy from one buffer into the other
	SDL_MixAudio(stream, data->audio_pos, len, AudioVolume);	// mix from one buffer into another

	data->audio_pos += len;
	data->audio_len -= len;
}