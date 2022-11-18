#ifndef STAGE_H
#define STAGE_H

#include "window.hpp"
#include "audio.hpp"
#include <fstream>
#include <vector>

/////////////////////////////////////

// Copy a char* into another safely
void copy_chararr(char *a, char *b) {
    int size = 0; while (a[size] != '\0') size++;
    b = new char[size + 1]; b[size] = '\0';
    for (int i = 0; i < size; i++) b[i] = a[i];
    return;
}

/////////////////////////////////////

// Tags for sprite types (used in collision detection, etc)
enum SPRITE_TYPE {
    PLAYER,
    BACKGROUND,
    DECORE,
    FLOOR,
    ROOF,
    WALL,
    BLOCK,
    ENEMY,
    ITEM,
    LABEL
};

/////////////////////////////////////

// An "actor" on a stage. Has references to x, y, and layer 
// positions, a "costume" (surface) and a type.
class Sprite {
public:
    Sprite();

    Sprite(SDL_Surface *s, i32 &xIn, i32 &yIn, u8 &l);
    Sprite(SDL_Surface *s, u8 l);
    ~Sprite();

    SDL_Surface *surface;

    i32 *x, *y;
    u8 *layer;

    SPRITE_TYPE type;
};

// Default constructor for inherited classes (DO NOT USE)
Sprite::Sprite() {
    surface = nullptr;
    x = y = nullptr;
    layer = nullptr;
}

// Construct a Sprite given a surface (costume) and variables
// to be associated with x, y, and layer.
Sprite::Sprite(SDL_Surface *s, i32 &xIn, i32 &yIn, u8 &l) {
    surface = s;
    x = &xIn;
    y = &yIn;
    layer = &l;
}

// Construct a Sprite given a costume and a layer.
// Variable references are generated within.
Sprite::Sprite(SDL_Surface *s, u8 l) {
    surface = s;
    x = new i32(0);
    y = new i32(0);
    layer = new u8(l);
}

// Dealloc associated memory
Sprite::~Sprite() {
    delete x;
    delete y;
    delete layer;
}

/////////////////////////////////////

// A simple wrapper object for easier interprettation of
// the isTouching() function output. Use like: 
// if (Collision(isTouching(a, b)).top()) {...}
class Collision {
public:
    Collision(std::bitset<8> b): col(b) {};
    Collision() {};

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

// Get analytics on the collisions of sprites a and b
// (can be easily interpretted using the collision type)
std::bitset<8> isTouching(Sprite *a, Sprite *b) {
    std::bitset<8> output;

    output[0] = *a->x + a->surface->w >= *b->x && *a->x + a->surface->w <= *b->x + b->surface->w;
    output[1] = *a->y + a->surface->h >= *b->y && *a->y + a->surface->h <= *b->y + b->surface->h;
    output[2] = *a->x >= *b->x && *a->x <= *b->x + b->surface->w;
    output[3] = *a->y >= *b->y && *a->y <= *b->y + b->surface->h;
    output[4] = output[3] && (output[0] || output[2]);
    output[5] = output[0] && (output[1] || output[3]);
    output[6] = output[1] && (output[0] || output[2]);
    output[7] = output[2] && (output[1] || output[3]);

    return output;
}

// Return if a's bounding rectangle collides with b's.
bool isTouchingVague(Sprite *a, Sprite *b) {
    auto result = isTouching(a, b);
    return result[4] || result[5] || result[6] || result[7];
}

/////////////////////////////////////

// Return if the mouse falls within an object's bounding rectangle
bool isMouseWithin(Sprite *a) {
    return (MOUSE_X >= *a->x && MOUSE_X < *a->x + a->surface->w) &&
        (MOUSE_Y >= *a->y && MOUSE_Y < *a->y + a->surface->h);
}

/////////////////////////////////////

// A simple singly-linked-list wrapper for sprites
// To be used within the Stage class for storing sprites by layer
struct SpriteNode {
    Sprite *cur;
    SpriteNode *next;
    char *name;
};

/////////////////////////////////////

// A simple handler for rendering many sprites to the screen.
// Fills with sprites, which are rendered according to their layer
// when update is called.
class Stage {
public:
    Stage(u16 height, u16 width, u8 depth = 1);

    void update(SDL_Surface *frame);

    void addSprite(Sprite *s, char *name = nullptr);
    void removeSprite(Sprite *s);
    void removeSprite(char *spriteName);

    std::vector<Sprite *> getTouching(Sprite *s) const;
    std::vector<Sprite *> getAllTouching(Sprite *s) const;
    std::vector<Sprite *> getOfType(SPRITE_TYPE t) const;

    std::vector<Sprite *> getOfType() const;
    void playSound(char *name);

    SpriteNode *SPRITES;
    std::vector<WAV> sounds;

    // height, width, depth
    u16 h, w;

    /////////////Temps////////////
    PIXEL_TYPE *pixels;
    PIXEL_TYPE *sourcePixels;

    Sprite *sprite;
    unsigned int source, destination;
};

/////////////////////////////////////

// Create an empty stage with the given height, width and depth (depth is depreciated).
Stage::Stage(u16 height, u16 width, u8 depth) {
    SPRITES = nullptr;
    h = height;
    w = width;

    return;
}

// Update a passed frame so that it contains the stage's image
// (with all sprites rendered from furthest away to closest, layer 0 through infinity)
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

                //if (!sprite->GO_OFFSCREEN) destination %= h * w;

                // Ignore if out of bounds
                if (destination >= 0 && destination < h * w) {
                    // Place pixel (skipping transparency)
                    if (sourcePixels[source] != 0) pixels[destination] = sourcePixels[source];
                }
            }
        }
    }

    SDL_UnlockSurface(frame);

    return;
}

// Insert at position sorted by layer (smallest first)
void Stage::addSprite(Sprite *s, char *name) {
    if (SPRITES == nullptr) {
        SPRITES = new SpriteNode();

        copy_chararr(name, SPRITES->name);
        
        SPRITES->cur = s;
        SPRITES->next = nullptr;
    } else {
        SpriteNode *prev = nullptr;
        SpriteNode *cursor = SPRITES;
        while (cursor != nullptr && *cursor->cur->layer < *s->layer) {
            prev = cursor;
            cursor = cursor->next;
        }
        
        if (prev == nullptr) {
            SpriteNode *temp = SPRITES;
            SPRITES = new SpriteNode();
            copy_chararr(name, SPRITES->name);
            SPRITES->next = temp;
            SPRITES->cur = s;
        } else {
            SpriteNode *temp = prev->next;
            prev->next = new SpriteNode();
            copy_chararr(name, prev->next->name);
            prev->next->next = temp;
            prev->next->cur = s;
        }
    }
    return;
}

// Remove a sprite such that it is no longer rendered
// on calling update(frame).
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

// Remove a sprite by name
void Stage::removeSprite(char *spriteName) {
    SpriteNode *prev = nullptr;
    SpriteNode *cursor = SPRITES;
    while (cursor != nullptr && cursor->name != spriteName) {
        prev = cursor;
        cursor = cursor->next;
    }

    if (cursor->name == spriteName) {
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

// Get all sprites stored in this stage which touches s
std::vector<Sprite *> Stage::getTouching(Sprite *s) const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        if (cursor->cur->type == BACKGROUND || cursor->cur->type == DECORE) {
            continue;
        }
        
        else if (isTouchingVague(s, cursor->cur)) {
            out.push_back(cursor->cur);
        }
    }

    if (out.empty()) out.push_back(nullptr);

    return out;
}

// Get all sprites where either isTouchingVague(sprite, s) or isTouchingVague(s, sprite) is true
std::vector<Sprite *> Stage::getAllTouching(Sprite *s) const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        if (cursor->cur->type == BACKGROUND || cursor->cur->type == DECORE) {
            continue;
        }

        else if (isTouchingVague(s, cursor->cur) || isTouchingVague(cursor->cur, s)) {
            out.push_back(cursor->cur);
        }
    }

    if (out.empty()) out.push_back(nullptr);

    return out;
}

// Get all sprites storing in this stage which have the type t
std::vector<Sprite *> Stage::getOfType(SPRITE_TYPE t) const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        if (cursor->cur->type == t) {
            out.push_back(cursor->cur);
        }
    }

    if (out.empty()) out.push_back(nullptr);

    return out;
}

// Get all sprites
std::vector<Sprite *> Stage::getOfType() const {
    std::vector<Sprite *> out;
    for (SpriteNode *cursor = SPRITES; cursor != nullptr; cursor = cursor->next) {
        out.push_back(cursor->cur);
    }

    if (out.empty()) out.push_back(nullptr);

    return out;
}

// Play a wav which has been loaded into memory
void Stage::playSound(char *name) {
    for (WAV sound : sounds) {
        for (int i = 0; name[i] != '\0' && name[i] == sound.name[i]; i++) {
            playWAV(&sound);
            return;
        }
    }
    throw std::runtime_error("Invalid sound name in stage");
    return;
}

/////////////////////////////////////

#endif
