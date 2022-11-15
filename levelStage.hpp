#include "jgame.hpp"

#include <fstream>
#include <regex>
#include <string>
#include <iostream>

using namespace std;

#ifndef LEVELSTAGE_H
#define LEVELSTAGE_H

/////////////////////////////////////

ifstream &operator >> (ifstream& stream, char *&into) {
    string temp = "#";
    
    while (temp[0] == '#' || temp.empty()) {
        stream >> temp;
    }

    // Convert string temp to new char* in into
    into = new char[temp.size() + 1];
    for (int i = 0; i < temp.size(); i++) {
        into[i] = temp[i];
    } into[temp.size()] = '\0';

    return stream;
}

/* Level file structure:
name x y layer spriteTag scale numFrames frameTag
<frames/folder>
*/
void loadFileToStage(ifstream *loader, Stage *stage) {
    if (loader->is_open()) {
        // Empty old data
        while (stage->SPRITES != nullptr) {
            stage->removeSprite(stage->SPRITES->cur);
        }

        // Load new data
        while (!loader->eof()) {
            string name, spriteTag, frameTag;
            int x, y, layer, scaleA, scaleB, numFrames;
            
            *loader >> name >> x >> y >> layer >> spriteTag >> 
                scaleA >> scaleB >> numFrames >> frameTag;

            SPRITE_TYPE tag;
            if (spriteTag == "BACKGROUND") {
                tag = BACKGROUND;
            } else if (spriteTag == "BLOCK") {
                tag = BLOCK;
            } else if (spriteTag == "DECORE") {
                tag = DECORE;
            } else if (spriteTag == "ENEMY") {
                tag = ENEMY;
            } else if (spriteTag == "FLOOR") {
                tag = FLOOR;
            }  else if (spriteTag == "ITEM") {
                tag = ITEM;
            } else if (spriteTag == "LABEL") {
                tag = LABEL;
            } else if (spriteTag == "PLAYER") {
                tag = PLAYER;
            } else if (spriteTag == "ROOF") {
                tag = ROOF;
            } else if (spriteTag == "WALL") {
                tag = WALL;
            } else {
                throw runtime_error("Invalid sprite tag");
            }

            SmartSprite *toAdd;
            string path;

            // Load from paths
            if (frameTag == "DIRECT") {
                Surface **frames = new Surface*[numFrames];
                for (int i = 0; i < numFrames; i++) {
                    *loader >> path;
                    frames[i] = loadTexture(path.c_str(), scaleA);
                }
                toAdd = new SmartSprite(numFrames, frames, x, y, layer, tag);
            }

            // Load from a folder
            else if (frameTag == "FOLDER") {
                *loader >> path;
                toAdd = new SmartSprite(numFrames, (char*)path.c_str(), x, y, layer, scaleA, tag);
            }

            // Load from 1 path, using tiling
            else if (frameTag == "TILE") {
                *loader >> path;

                Surface *from = loadTexture(path.c_str(), 1);

                Surface *onto = SDL_CreateRGBSurface(0, scaleA, scaleB, BITDEPTH, 0, 0, 0, 0);
                
                tile(from, onto);
                toAdd = new SmartSprite(1, &onto, x, y, layer, tag);
            }
            
            else {
                throw runtime_error("Invalid frame tag (must be DIRECT or FOLDER)");
            }

            stage->addSprite(toAdd, (char*)name.c_str());
        }
    } else {
        throw runtime_error("Invalid level path");
    }

    return;
}

/////////////////////////////////////

// stage with load-from-file ability
class LevelStage {
public:
    LevelStage(u16 height, u16 width, char *levelListFile);
    ~LevelStage();

    void update(SDL_Surface *frame);

    void addSprite(Sprite *sprite);
    void removeSprite(Sprite *sprite);
    void removeSprite(char *spriteName);

    u8 level() const;
    void level(u8 toLoadNumber);
    void level(char *toLoadName);
    
    void incLevel(u8 by = 1);
    u16 numLevels;
protected:
    ifstream loader;

    u8 currentLevel;
    char **levelPaths;
    Stage *stage;
};

/////////////////////////////////////

/* Level list file structure:
numLevels
level
level
level
*/

LevelStage::LevelStage(u16 height, u16 width, char *levelListFile) {

    // Load from levelListFile file
    loader.open(levelListFile);
    if (loader.is_open()) {
        loader >> numLevels;
        levelPaths = new char*[numLevels];
        for (int i = 0; i < numLevels; i++) {
            loader >> levelPaths[i];
        }
    } else {
        throw runtime_error("Could not open levelListFile");
    }
    loader.close();

    // Finish initialization
    stage = new Stage(height, width);
    currentLevel = 0;

    level(currentLevel);

    return;
}

LevelStage::~LevelStage() {
    delete stage;
    delete [] levelPaths;

    return;
}

/////////////////////////////////////

void LevelStage::update(Surface *frame) {
    stage->update(frame);
    return;
}

void LevelStage::addSprite(Sprite *sprite) {
    stage->addSprite(sprite);
    return;
}

void LevelStage::removeSprite(Sprite *sprite) {
    stage->removeSprite(sprite);
    return;
}

void LevelStage::removeSprite(char *spriteName) {
    stage->removeSprite(spriteName);
    return;
}

/////////////////////////////////////

u8 LevelStage::level() const {
    return currentLevel;
}

void LevelStage::level(u8 toLoadNumber) {
    if (toLoadNumber < numLevels) {
        loader.open(levelPaths[toLoadNumber]);
        loadFileToStage(&loader, stage);
        loader.close();
    } else {
        throw runtime_error("Invalid level index");
    }
    return;
}

void LevelStage::level(char *toLoadName) {
    for (int i = 0; i < numLevels; i++) {
        if (levelPaths[i] == toLoadName) {
            loader.open(levelPaths[i]);
            loadFileToStage(&loader, stage);
            loader.close();
            return;
        }
    }
    throw runtime_error("Invalid level name");
    
    return;
}

void LevelStage::incLevel(u8 by) {
    if (by > 0) {
        currentLevel += by;
        currentLevel %= numLevels;
        
        std::cout << levelPaths[currentLevel] << '\n';
        assert(!loader.is_open());

        loader.open(levelPaths[currentLevel]);
        loadFileToStage(&loader, stage);
        loader.close();
        return;
    } else {
        throw runtime_error("Invalid frame incrementation");
    }
    return;
}

/////////////////////////////////////

#endif
