#ifndef JGAME_H
#define JGAME_H

// Dependancies
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <cmath>
#include <string>
#include <regex>

#include <SDL2/SDL.h>
using namespace std;

// Add this to your compilation command:
// `sdl2-config --cflags --libs`

#include "window.hpp"
#include "audio.hpp"
#include "keys.hpp"

#include "stage.hpp"
#include "textures.hpp"
#include "smartSprite.hpp"
#include "levelStage.hpp"

#include "font.hpp"
#include "drivers.hpp"

#endif
