#ifndef STAGE_H
#define STAGE_H

#include "jgame.hpp"

/////////////////////////////////////

// Copy a char* into another safely
void copy_chararr(char *a, char *b) {
    if (a == nullptr) {
        b = nullptr;
        return;
    }
    if (b != nullptr) delete [] b;
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
    LABEL,
    NPC
};

/////////////////////////////////////

// An "actor" on a stage. Has references to x, y, and layer 
// positions, a "costume" (surface) and a type.
class Sprite {
public:
    Sprite();
    Sprite(SDL_Surface *s, i32 x = 0, i32 y = 0, u8 l = 0);
    ~Sprite();

    SDL_Surface *surface;

    SDL_Rect rect;
    u8 layer;

    SPRITE_TYPE type;
};

// Default constructor for inherited classes (DO NOT USE)
Sprite::Sprite() {
    surface = nullptr;
    layer = 0;
}

// Construct a Sprite given a surface (costume) and variables
// to be associated with x, y, and layer.
Sprite::Sprite(SDL_Surface *s, i32 x, i32 y, u8 l) {
    surface = s;
    layer = l;
    
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    return;
}

// Dealloc associated memory
Sprite::~Sprite() {
    SDL_FreeSurface(surface);
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
Collision isTouching(Sprite *a, Sprite *b) {
    std::bitset<8> output;

    output[0] = a->rect.x + a->surface->w >= b->rect.x && a->rect.x + a->surface->w <= b->rect.x + b->surface->w;
    output[1] = a->rect.y + a->surface->h >= b->rect.y && a->rect.y + a->surface->h <= b->rect.y + b->surface->h;
    output[2] = a->rect.x >= b->rect.x && a->rect.x <= b->rect.x + b->surface->w;
    output[3] = a->rect.y >= b->rect.y && a->rect.y <= b->rect.y + b->surface->h;
    output[4] = output[3] && (output[0] || output[2]);
    output[5] = output[0] && (output[1] || output[3]);
    output[6] = output[1] && (output[0] || output[2]);
    output[7] = output[2] && (output[1] || output[3]);

    return Collision(output);
}

// Return if a's bounding rectangle collides with b's.
bool isTouchingVague(Sprite *a, Sprite *b) {
    auto result = isTouching(a, b);
    return result.top() || result.bottom() || result.right() || result.left();
}

/////////////////////////////////////

// Return if the mouse falls within an object's bounding rectangle
bool isMouseWithin(Sprite *a) {
    return (MOUSE_X >= a->rect.x && MOUSE_X < a->rect.x + a->surface->w) &&
        (MOUSE_Y >= a->rect.y && MOUSE_Y < a->rect.y + a->surface->h);
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
    Stage(u16 height, u16 width);

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
    
    u16 h;
    u16 w;
};

/////////////////////////////////////

// Create an empty stage.
Stage::Stage(u16 height, u16 width) {
    SPRITES = nullptr;
    sounds.clear();
    h = height;
    w = width;

    return;
}

// Update a passed frame so that it contains the stage's image
// (with all sprites rendered from furthest away to closest, layer 0 through infinity)
void Stage::update(SDL_Surface *frame) {
    // Iterate over sprites (from largest to smallest layer)
    Sprite *sprite;
    for (SpriteNode *current = SPRITES; current != nullptr; current = current->next) {
        sprite = current->cur;

        if (sprite->rect.x < -sprite->surface->w || sprite->rect.y < -sprite->surface->h) continue;

        SDL_BlitSurface(sprite->surface, NULL, frame, &sprite->rect);
    }

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
        while (cursor != nullptr && cursor->cur->layer < s->layer) {
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
