#include "jgame.hpp"
#include "audio.hpp"

#include <fstream>
#include <regex>
#include <string>

#include <iostream>

using namespace std;

#ifndef LEVELSTAGE_H
#define LEVELSTAGE_H

/////////////////////////////////////

// Safely extract string data into a char*
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
        for (WAV sound : stage->sounds) {
            SDL_FreeWAV(sound.wavBuffer);
        }
        stage->sounds.clear();

        // Load sounds
        int numSounds = 0;
        string temp;
        WAV wavTemp;
        *loader >> numSounds;
        for (int i = 0; i < numSounds; i++) {
            *loader >> temp;
            loadWAV((char*)temp.c_str(), &wavTemp);
            cout << "Added sound: '" << wavTemp.name << "'\n";
            stage->sounds.push_back(wavTemp);
        }

        // Load sprites
        while (!loader->eof()) {
            string name, spriteTag, frameTag; 
            int layer, numFrames;
            double scaleA, scaleB, x, y;
            
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
                throw runtime_error("Invalid sprite tag '" + spriteTag + "'");
            }

            SmartSprite *toAdd;
            string path;

            // Load from paths
            if (frameTag == "DIRECT") {
                Surface **frames = new Surface*[numFrames];
                for (int i = 0; i < numFrames; i++) {
                    *loader >> path;
                    frames[i] = loadTexture(path.c_str(), stage->w * scaleA);
                }
                toAdd = new SmartSprite(numFrames, frames, stage->w * x, stage->h * y, layer, tag);
            }

            // Load from a folder
            else if (frameTag == "FOLDER") {
                *loader >> path;
                toAdd = new SmartSprite(numFrames, (char*)path.c_str(), stage->w * x, stage->h * y, layer, stage->w * scaleA, tag);
            }

            // Load from 1 path, using tiling
            else if (frameTag == "TILE") {
                *loader >> path;

                Surface *from = loadTexture(path.c_str(), 1);

                Surface *onto = SDL_CreateRGBSurface(0, stage->w * scaleA, stage->w * scaleB, BITDEPTH, 0, 0, 0, 0);
                
                tile(from, onto);
                toAdd = new SmartSprite(1, &onto, stage->w * x, stage->h * y, layer, tag);
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
    char *levelName() const;
    void level(u8 toLoadNumber);
    void level(char *toLoadName);

    vector<Sprite *> getTouching(Sprite *s) const;
    vector<Sprite *> getAllTouching(Sprite *s) const;
    vector<Sprite *> getOfType(SPRITE_TYPE t) const;

    vector<Sprite *> getOfType() const;
    void playSound(char *name) const;
    
    void incLevel(u8 by = 1);
    u16 numLevels;
protected:
    ifstream loader;

    u8 currentLevel;
    char **levelPaths;
    Stage *stage;
};

/////////////////////////////////////

// Construct LevelStage given path to list
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

// Dealloc memory for levelStage
LevelStage::~LevelStage() {
    delete stage;
    delete [] levelPaths;

    return;
}

/////////////////////////////////////

// Render all sprites currently in frame and in memory to the passed frame
void LevelStage::update(Surface *frame) {
    stage->update(frame);
    return;
}

// Add a sprite to frame (doesn't modify file)
void LevelStage::addSprite(Sprite *sprite) {
    stage->addSprite(sprite);
    return;
}

// Remove a sprite form frame by pointer
void LevelStage::removeSprite(Sprite *sprite) {
    stage->removeSprite(sprite);
    return;
}

// Remove a sprite from frame by name
void LevelStage::removeSprite(char *spriteName) {
    stage->removeSprite(spriteName);
    return;
}

/////////////////////////////////////

// Get the current level
u8 LevelStage::level() const {
    return currentLevel;
}

// Get the current level name
char *LevelStage::levelName() const {
    return levelPaths[currentLevel];
}

// Load a given level by number
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

// Load a given level by name
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

/////////////////////////////////////

// Get all sprites s touches
vector<Sprite *> LevelStage::getTouching(Sprite *s) const {
    return stage->getTouching(s);
}

// Get all sprites s touches AND that touch s
vector<Sprite *> LevelStage::getAllTouching(Sprite *s) const {
    return stage->getAllTouching(s);
}

// Get all sprites of a given type
vector<Sprite *> LevelStage::getOfType(SPRITE_TYPE t) const {
    return stage->getOfType(t);
}

// Get all sprites
vector<Sprite *> LevelStage::getOfType() const {
    return stage->getOfType();
}

/////////////////////////////////////

void LevelStage::playSound(char *name) const {
    stage->playSound(name);
    return;
}

/////////////////////////////////////

// Increment the current level by a number
void LevelStage::incLevel(u8 by) {
    if (by > 0) {
        currentLevel += by;
        currentLevel %= numLevels;

        assert(!loader.is_open());
        assert(currentLevel < numLevels); // idk how this would fail but just in case

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
