#ifndef STAGE_H
#define STAGE_H

#include "window.hpp"
#include <fstream>
#include <vector>

/////////////////////////////////////

enum SPRITE_TYPE {
    PLAYER,
    BACKGROUND,
    DECORE,
    FLOOR,
    ROOF,
    WALL,
    ENEMY,
    ITEM
};

/////////////////////////////////////

class Sprite {
public:
    Sprite();

    Sprite(SDL_Surface *s, int &xIn, int &yIn, Uint8 &l);
    Sprite(SDL_Surface *s, Uint8 l);
    ~Sprite();

    SDL_Surface *surface;

    int *x, *y;
    Uint8 *layer;

    SPRITE_TYPE type;
};

Sprite::Sprite() {
    surface = nullptr;
    x = y = nullptr;
    layer = nullptr;
}

Sprite::Sprite(SDL_Surface *s, int &xIn, int &yIn, Uint8 &l) {
    surface = s;
    x = &xIn;
    y = &yIn;
    layer = &l;
}

Sprite::Sprite(SDL_Surface *s, Uint8 l) {
    surface = s;
    x = new int(0);
    y = new int(0);
    layer = new Uint8(l);
}

Sprite::~Sprite() {
    delete x;
    delete y;
    delete layer;
}

/////////////////////////////////////

class Collision {
public:
    Collision(std::bitset<8> b): col(b) {};

    bool upperX() { return col[0]; }
    bool upperY() { return col[1]; }
    bool lowerX() { return col[2]; }
    bool lowerY() { return col[3]; }

    bool top() { return col[4]; }
    bool bottom() { return col[6]; }
    bool left() { return col[7]; }
    bool right() { return col[5]; }
protected:
    std::bitset<8> col;
};

std::bitset<8> isTouching(Sprite *a, Sprite *b) {
    std::bitset<8> output;

    output[0] = *a->x + a->surface->w >= *b->x && *a->x + a->surface->w < *b->x + b->surface->w;
    output[1] = *a->y + a->surface->h >= *b->y && *a->y + a->surface->h < *b->y + b->surface->h;
    output[2] = *a->x >= *b->x && *a->x < *b->x + b->surface->w;
    output[3] = *a->y >= *b->y && *a->y < *b->y + b->surface->h;
    output[4] = output[3] && (output[0] || output[2]);
    output[5] = output[0] && (output[1] || output[3]);
    output[6] = output[1] && (output[0] || output[2]);
    output[7] = output[2] && (output[1] || output[3]);

    return output;
}

bool isTouchingVague(Sprite *a, Sprite *b) {
    auto result = isTouching(a, b);
    return result[4] || result[5] || result[6] || result[7];
}

/////////////////////////////////////

bool isMouseWithin(Sprite *a) {
    return (MOUSE_X >= *a->x && MOUSE_X < *a->x + a->surface->w) &&
        (MOUSE_Y >= *a->y && MOUSE_Y < *a->y + a->surface->h);
}

/////////////////////////////////////

struct SpriteNode {
    Sprite *cur;
    SpriteNode *next;
};

/////////////////////////////////////

class Stage {
public:
    Stage(Uint16 height, Uint16 width, Uint8 depth);

    void update(SDL_Surface *frame);

    void addSprite(Sprite *s);
    void removeSprite(Sprite *s);

    std::vector<Sprite *> getTouching(Sprite *s) const;
    std::vector<Sprite *> getOfType(SPRITE_TYPE t) const;

    SpriteNode *SPRITES;

    // height, width, depth
    Uint16 h, w;
    Uint8 d;

    /////////////Temps////////////
    PIXEL_TYPE *pixels;
    PIXEL_TYPE *sourcePixels;

    Sprite *sprite;
    unsigned int source, destination;
};

/////////////////////////////////////

Stage::Stage(Uint16 height, Uint16 width, Uint8 depth) {
    SPRITES = nullptr;
    h = height;
    w = width;
    d = depth;

    return;
}

void Stage::update(SDL_Surface *frame) {
    SDL_LockSurface(frame);

    pixels = (PIXEL_TYPE*)frame->pixels;

    // Iterate over sprites (from largest to smallest layer)
    for (SpriteNode *current = SPRITES; current != nullptr; current = current->next) {
        sprite = current->cur;

        if (*sprite->x < 0 - sprite->surface->w || *sprite->y < 0 - sprite->surface->h) continue;

        sourcePixels = (PIXEL_TYPE*)sprite->surface->pixels;
        
        // Place pixels in their correct positions
        for (int r = 0; r < sprite->surface->h; r++) {
            for (int c = 0; c < sprite->surface->w; c++) {

                // Determine where from and where to
                source = (r * sprite->surface->w) + c;
                destination = ((r + *sprite->y) * frame->w) + c + *sprite->x;

                if (c + *sprite->x + 1 > frame->w || c + *sprite->x < 0) continue;

                // Ignore if out of bounds
                if (destination > 0 && destination < h * w) {
                    // Place pixel (skipping transparency)
                    if (sourcePixels[source] != 0) pixels[destination] = sourcePixels[source];
                }
            }
        }
    }

    SDL_UnlockSurface(frame);

    return;
}

// Insert at position sorted by layer (largest first)
void Stage::addSprite(Sprite *s) {
    if (SPRITES == nullptr) {
        SPRITES = new SpriteNode();
        SPRITES->cur = s;
        SPRITES->next = nullptr;
    } else {
        SpriteNode *prev = nullptr;
        SpriteNode *cursor = SPRITES;
        while (cursor != nullptr && *cursor->cur->layer > *s->layer) {
            prev = cursor;
            cursor = cursor->next;
        }
        
        if (prev == nullptr) {
            SpriteNode *temp = SPRITES;
            SPRITES = new SpriteNode();
            SPRITES->next = temp;
            SPRITES->cur = s;
        } else {
            SpriteNode *temp = prev->next;
            prev->next = new SpriteNode();
            prev->next->next = temp;
            prev->next->cur = s;
        }
    }
    return;
}

void Stage::removeSprite(Sprite *s) {
    SpriteNode *prev = nullptr;
    SpriteNode *cursor = SPRITES;
    while (cursor != nullptr && cursor->cur != s) {
        prev = cursor;
        cursor = cursor->next;
    }

    if (cursor->cur == s) {
        if (prev == nullptr) {
            SPRITES = cursor->next;
            delete cursor;
        } else {
            prev->next = cursor->next;
            delete cursor;
        }
    }

    return;
}

/////////////////////////////////////

std::vector<Sprite *> Stage::getTouching(Sprite *s) const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        if (cursor->cur->type == BACKGROUND || cursor->cur->type == DECORE) continue;
        if (isTouchingVague(s, cursor->cur)) {
            out.push_back(cursor->cur);
        }
    }
    return out;
}

std::vector<Sprite *> Stage::getOfType(SPRITE_TYPE t) const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        if (cursor->cur->type == t) {
            out.push_back(cursor->cur);
        }
    }
    return out;
}

/////////////////////////////////////

#endif
