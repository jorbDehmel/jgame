#ifndef SIDESCROLLER_DRIVER_H
#define SIDESCROLLER_DRIVER_H

#include "../jgame.hpp"
#include "scrollDriver.hpp"

///////////////////////////////////////////////////////////

void wallCollide(Sprite *player, Sprite *other)
{
    if (player->rect.x < other->rect.x)
    {
        while (isTouchingVague(player, other))
        {
            player->rect.x--;
        }
    }
    else
    {
        while (isTouchingVague(player, other))
        {
            player->rect.x++;
        }
    }

    return;
}

void roofCollide(Sprite *player, Sprite *other)
{
    while (isTouchingVague(player, other))
    {
        player->rect.y++;
    }

    return;
}

void floorCollide(Sprite *player, Sprite *other)
{
    while (isTouchingVague(player, other))
    {
        player->rect.y--;
    }

    return;
}

void blockCollide(Sprite *player, Sprite *other)
{
    floorCollide(player, other);
    roofCollide(player, other);
    wallCollide(player, other);

    return;
}

///////////////////////////////////////////////////////////

class SideScrollerDriver : public ScrollDriver
{
public:
    SideScrollerDriver(Stage *stageIn, double overScrollRatioX, double overScrollRatioY);
    void handle();

    int gravityConstant;
    set<SPRITE_TYPE> gravityTypes;
    map<Sprite *, int> yVelocities;
};

///////////////////////////////////////////////////////////

SideScrollerDriver::SideScrollerDriver(Stage *stageIn, double overScrollRatioX, double overScrollRatioY) : ScrollDriver(stageIn, overScrollRatioX, overScrollRatioY)
{
    gravityTypes.insert(PLAYER);
    gravityTypes.insert(ENEMY);
    gravityTypes.insert(NPC);

    this->collisionHandlers[WALL] = wallCollide;
    this->collisionHandlers[ROOF] = roofCollide;
    this->collisionHandlers[FLOOR] = floorCollide;
    this->collisionHandlers[BLOCK] = blockCollide;

    gravityConstant = 1;

    return;
}

void SideScrollerDriver::handle()
{
    for (Sprite *s : this->stage->getOfType())
    {
        if (gravityTypes.count(s->type) != 0)
        {
            if (yVelocities.count(s) == 0)
            {
                yVelocities[s] = 0;
            }

            yVelocities[s] += gravityConstant;
            s->rect.y += yVelocities[s];
        }
    }

    this->ScrollDriver::handle();
    return;
}

///////////////////////////////////////////////////////////

#endif