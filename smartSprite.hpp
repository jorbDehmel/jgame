#ifndef SMARTSPRITE_H
#define SMARTSPRITE_H

#include "textures.hpp"

/////////////////////////////////////

// Concatenate two string literals
char *concat(const char a[], char *b) {
    int sizeA = 0; while (a[sizeA] != '\0') sizeA++;
    int sizeB = 0; while (b[sizeB] != '\0') sizeB++;
    
    char *out = new char[sizeA + sizeB + 1];
    for (int i = 0; i < sizeA; i++) out[i] = a[i];
    for (int i = sizeA; i < sizeA + sizeB; i++) out[i] = b[i - sizeA];
    out[sizeA + sizeB] = '\0';

    return out;
}

// Concatenate two string literals
char *concat(char a, char *b) {
    int size = 0; while (b[size] != '\0') size++;
    char *out = new char[size + 2];
    out[0] = a; for (int i = 1; i < size + 1; i++) out[i] = b[i - 1];
    out[size + 1] = '\0';
    return out;
}

// Compare two chararr literals (basic bool)
bool cmp_chararrs(char *a, char *b) {
    int sizeA = 0, sizeB = 0;
    while (a[sizeA] != '\0') sizeA++;
    while (b[sizeB] != '\0') sizeB++;
    if (sizeA != sizeB) {
        return false;
    }
    for (int i = 0; i < sizeA; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

/////////////////////////////////////

// For holding in a sprite to indicate current costume direction
enum DIRECTION_TYPE {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LEFTUP,
    LEFTDOWN,
    RIGHTUP,
    RIGHTDOWN,
    TOWARDS,
    AWAY
};

/////////////////////////////////////

// A slightly improved version of the sprite object
// Holds its own x, y, and layer rather than holding a pointer.
// Also able to hold multiple frames and cycle through them.
class SmartSprite: public Sprite {
public:
    SmartSprite(u8 numF, Surface **s, int x = 0, int y = 0, u8 l = 0, SPRITE_TYPE t = PLAYER);
    SmartSprite(u8 numF, char *folder, int x = 0, int y = 0, u8 l = 0, u8 scale = 1, SPRITE_TYPE t = PLAYER);

    ~SmartSprite();

    inline int X() const { return *x; }
    inline int Y() const { return *y; }
    inline void X(int t) { *x = t; }
    inline void Y(int t) { *y = t; }
    inline u8 L() const { return *layer; }
    inline void L(int t) { if (t > 0) *layer = t; }

    inline int W() const { return surface->w; }
    inline int H() const { return surface->h; }

    inline void xInc(int t = 1) { (*x) += t; }
    inline void yInc(int t = 1) { (*y) += t; }
    inline void lInc(int t = 1) { (*layer) += t; }

    inline int frame() const { return curFrame; };
    inline void frame(int t) {
        if (t >= 0 && t < numFrames) {
            curFrame = t;
            surface = frames[curFrame];
        } else {
            throw std::runtime_error("Invalid frame position!");
        }
        return;
    }
    inline void frame(char *name) {
        for (int i = 0; i < numFrames; i++) {
            if (cmp_chararrs(frameNames[i], name)) {
                curFrame = i;
                surface = frames[curFrame];
                return;
            }
        }
        throw std::runtime_error("Invalid frame name!");
    }

    inline void incFrame(int t = 1) {
        curFrame = ((int)curFrame + t) % numFrames;
        surface = frames[curFrame];
    }

    DIRECTION_TYPE direction;

    u8 numFrames;
    u8 curFrame;
    Surface **frames;
    char **frameNames;
};

/////////////////////////////////////

// Initialize a smartSprite from a list of Surface * frames.
SmartSprite::SmartSprite(u8 numF, Surface **s, int x, int y, u8 l, SPRITE_TYPE t) {
    numFrames = numF;
    frameNames = nullptr;
    frames = new Surface*[numFrames];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = s[i];
    }
    curFrame = 0;
    type = t;

    this->x = new int(x);
    this->y = new int(y);
    this->layer = new u8(l);
    this->surface = frames[curFrame];
}

// Initialize a smartSprite from a folder path and a number of frames to load.
SmartSprite::SmartSprite(u8 numF, char *folder, int x, int y, u8 l, u8 scale, SPRITE_TYPE t) {
    frameNames = new char*[numF];

    system(concat("ls >toload.temp ", folder));

    // sort output file into paths
    std::ifstream in;
    in.open("toload.temp");
    if (!in.is_open()) throw std::runtime_error("Failed to open indexing file for folder texture loading");
    for (int i = 0; i < numF; i++) {
        frameNames[i] = new char[64];
        for (int j = 0; j < 64; j++) frameNames[i][j] = '\0';
        in.getline(frameNames[i], 63);
    }
    in.close();

    system("rm -f toload.temp");

    numFrames = numF;
    frames = new Surface*[numF];
    for (int i = 0; i < numF; i++) {
        frames[i] = SDL_LoadBMP(concat(concat(folder, (char*)"/"), frameNames[i]));    //loadTexture(concat(concat(folder, (char*)"/"), paths[i]), scale);
        upScale(frames[i], scale);
    }
    curFrame = 0;
    type = t;

    this->x = new int(x);
    this->y = new int(y);
    this->layer = new u8(l);
    this->surface = frames[curFrame];
}

// Gracefully deallocate all associated memory
SmartSprite::~SmartSprite() {
    delete x;
    delete y;
    delete [] frames;
}

/////////////////////////////////////

#endif
