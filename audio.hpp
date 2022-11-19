#ifndef AUDIO_H
#define AUDIO_H

#include "../jgame/jgame.hpp"

/////////////////////////////////////

SDL_AudioDeviceID deviceId;
bool AUDIO_IS_INIT = false;

struct WAV {
    SDL_AudioSpec wavSpec;
    u32 wavLength;
    u8 *wavBuffer;
    SDL_AudioDeviceID id;
    char *name;
};

/////////////////////////////////////

void initializeAudio() {
    SDL_Init(SDL_INIT_AUDIO);
    AUDIO_IS_INIT = true;
    return;
}

void loadWAV(char *path, WAV *toLoad) {
    if (!AUDIO_IS_INIT) {
        initializeAudio();
    }
    
    int size = 0; while (path[size] != '\0') size++;
    toLoad->name = new char[size + 1]; toLoad->name[size] = '\0';
    for (int i = 0; i < size; i++) toLoad->name[i] = path[i];
    
    SDL_LoadWAV(path, &toLoad->wavSpec, &toLoad->wavBuffer, &toLoad->wavLength);
    return;
}

WAV loadWAV(char *path) {
    WAV toLoad;
    loadWAV(path, &toLoad);
    return toLoad;
}

void playWAV(WAV *toPlay) {
    deviceId = SDL_OpenAudioDevice(NULL, 0, &toPlay->wavSpec, NULL, 0);
    SDL_QueueAudio(deviceId, toPlay->wavBuffer, toPlay->wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
    return;
}

void closeAudio(WAV *toFree = nullptr) {
    SDL_CloseAudioDevice(deviceId);
    if (toFree != nullptr) {
        SDL_FreeWAV(toFree->wavBuffer);
    }
    return;
}

/////////////////////////////////////

#endif
