#ifndef SMARTSPRITE_H
#define SMARTSPRITE_H

#include "textures.hpp"

/////////////////////////////////////

char *concat(const char a[], char *b) {
    int sizeA = 0; while (a[sizeA] != '\0') sizeA++;
    int sizeB = 0; while (b[sizeB] != '\0') sizeB++;
    
    char *out = new char[sizeA + sizeB + 1];
    for (int i = 0; i < sizeA; i++) out[i] = a[i];
    for (int i = sizeA; i < sizeA + sizeB; i++) out[i] = b[i - sizeA];
    out[sizeA + sizeB] = '\0';

    return out;
}

/////////////////////////////////////

class SmartSprite: public Sprite {
public:
    SmartSprite(Uint8 numF, SDL_Surface **s, int x = 0, int y = 0, Uint8 l = 0, SPRITE_TYPE t = PLAYER);
    SmartSprite(Uint8 numF, char **paths, int x = 0, int y = 0, Uint8 l = 0, Uint8 scale = 1, SPRITE_TYPE t = PLAYER);
    SmartSprite(char *path, int x = 0, int y = 0, Uint8 l = 0, Uint8 scale = 1, SPRITE_TYPE t = PLAYER);
    SmartSprite(Uint8 numF, char *folder, int x = 0, int y = 0, Uint8 l = 0, Uint8 scale = 1, SPRITE_TYPE t = PLAYER);

    ~SmartSprite();

    Uint8 numFrames;
    Uint8 curFrame;
    SDL_Surface **frames;
};

/////////////////////////////////////

SmartSprite::SmartSprite(Uint8 numF, SDL_Surface **s, int x, int y, Uint8 l, SPRITE_TYPE t) {
    numFrames = numF;
    frames = new SDL_Surface*[numFrames];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = s[i];
    }
    curFrame = 0;
    type = t;

    this->x = new int(x);
    this->y = new int(y);
    this->layer = new Uint8(l);
    this->surface = frames[curFrame];
}

SmartSprite::SmartSprite(Uint8 numF, char **paths, int x, int y, Uint8 l, Uint8 scale, SPRITE_TYPE t) {
    numFrames = numF;
    frames = new SDL_Surface*[numFrames];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = loadTexture(paths[i], scale);
    }
    curFrame = 0;
    type = t;

    this->x = new int(x);
    this->y = new int(y);
    this->layer = new Uint8(l);
    this->surface = frames[curFrame];
}

SmartSprite::SmartSprite(char *path, int x, int y, Uint8 l, Uint8 scale, SPRITE_TYPE t) {
    SmartSprite(1, &path, y, x, l, scale, t);
}

SmartSprite::SmartSprite(Uint8 numF, char *folder, int x, int y, Uint8 l, Uint8 scale, SPRITE_TYPE t) {
    char **paths = new char*[numF];

    // ls folder >output_file
    system(concat("ls >toload.temp ", folder));

    // sort output file into paths
    std::ifstream in;
    in.open("toload.temp");
    if (!in.is_open()) throw std::runtime_error("Failed to open indexing file for folder texture loading");
    for (int i = 0; i < numF; i++) {
        paths[i] = new char[64];
        in.getline(paths[i], 64);
    }
    in.close();
    system("rm -f toload.temp");

    numFrames = numF;
    frames = new SDL_Surface*[numF];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = loadTexture(concat(concat(folder, (char*)"/"), paths[i]), scale);
    }
    curFrame = 0;
    type = t;

    this->x = new int(x);
    this->y = new int(y);
    this->layer = new Uint8(l);
    this->surface = frames[curFrame];
}

SmartSprite::~SmartSprite() {
    delete x;
    delete y;
    delete [] frames;
}

/////////////////////////////////////

#endif
