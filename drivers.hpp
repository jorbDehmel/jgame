#ifndef DRIVERS_H
#define DRIVERS_H

//////////////////////////////////////////////////

#include "jgame.hpp"

//////////////////////////////////////////////////

// Default collision handlers
void hitWall(Sprite *player, Sprite *wall) {}
void hitRoof(Sprite *player, Sprite *roof) {}
void hitFloor(Sprite *player, Sprite *floor) {}
void hitBlock(Sprite *player, Sprite *block) {}
void hitEnemy(Sprite *player, Sprite *enemy) {}
void hitItem(Sprite *player, Sprite *item) {}

//////////////////////////////////////////////////

// A driver for top-down RPG-style games
class ScrollDriver {
public:
    ScrollDriver(Stage *stageIn, double overScrollRatioX = 0, double overScrollRatioY = 0);

    void handle();
    void associate(SPRITE_TYPE t, void (*func)(Sprite*, Sprite*));

    i16 renderH, renderW;
    i16 defaultX, defaultY;
    i16 maxX, maxY, minX, minY;
    SmartSprite *player;
    i8 xInc, yInc;
    i16 xOffset, yOffset;

    Stage *stage;

    map<SPRITE_TYPE, void (*)(Sprite*, Sprite*)> collisionHandlers;
    set<SPRITE_TYPE> exemptTypes;
};

//////////////////////////////////////////////////

// Initialize driver
ScrollDriver::ScrollDriver(Stage *stageIn, double overScrollRatioX, double overScrollRatioY) {
    stage = stageIn;
    player = (SmartSprite*)stage->getOfType(PLAYER)[0];
    assert(player);

    collisionHandlers.clear();
    exemptTypes.clear();

    renderH = stageIn->h;
    renderW = stageIn->w;

    defaultX = (renderW / 2) - (player->w() / 2);
    defaultY = (renderH / 2) - (player->h() / 2);

    maxX = 0;
    maxY = 0;
    minX = renderW * -overScrollRatioX;
    minY = renderH * -overScrollRatioY;

    xInc = yInc = 1;
    xOffset = yOffset = 0;
    
    collisionHandlers[WALL] = hitWall;
    collisionHandlers[ROOF] = hitRoof;
    collisionHandlers[FLOOR] = hitFloor;
    collisionHandlers[BLOCK] = hitBlock;
    collisionHandlers[ENEMY] = hitEnemy;
    collisionHandlers[ITEM] = hitItem;

    exemptTypes.insert(PLAYER);
    exemptTypes.insert(LABEL);

    return;
}

// Update positions
void ScrollDriver::handle() {
    i8 xChange = 0, yChange = 0;
    
    // Handle movement events
    if (KEYS['w'] || KEYS[keys::upArrow]) {
        if (player->y() > defaultY) player->yInc(-yInc);
        else if (yOffset < maxY) yChange += yInc;
        else if (player->y() > 0) player->yInc(-yInc);
    } else if (KEYS['s'] || KEYS[keys::downArrow]) {
        if (player->y() < defaultY) player->yInc(yInc);
        else if (yOffset > minY) yChange -= yInc;
        else if (player->y() + player->h() < renderH) player->yInc(yInc);
    }

    if (KEYS['d'] || KEYS[keys::rightArrow]) {
        if (player->x() < defaultX) player->xInc(xInc);
        else if (xOffset > minX) xChange -= xInc;
        else if (player->x() + player->w() < renderW) player->xInc(xInc);
    } else if (KEYS['a'] || KEYS[keys::leftArrow]) {
        if (player->x() > defaultX) player->xInc(-xInc);
        else if (xOffset < maxX) xChange += xInc;
        else if (player->x() > 0) player->xInc(-xInc);
    }

    for (auto c : stage->getAllTouching(player)) {
        if (collisionHandlers.count(c->type) != 0) {
            (collisionHandlers[c->type])(player, c);
        }
    }

    if (xChange == 0 && yChange == 0) return;

    // Increment all positions
    xOffset += xChange;
    yOffset += yChange;
    for (auto i: stage->getOfType()) {
        if (exemptTypes.find(i->type) == exemptTypes.end()) {
            i->rect.x += xChange;
            i->rect.y += yChange;
        }
    }

    return;
}

// Associate a given function with a sprite type. This function will be called
// upon player collision with a sprite of type t.
void ScrollDriver::associate(SPRITE_TYPE t, void (*func)(Sprite*, Sprite*)) {
    collisionHandlers[t] = func;
    return;
}

//////////////////////////////////////////////////

#endif
