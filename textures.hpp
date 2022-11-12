#ifndef TEXTURES_H
#define TEXTURES_H

#include <iostream>
#include <cassert>
#include "stage.hpp"
#include <regex>

/////////////////////////////////////

// Upscale a surface s such that its pointer now points to a surface
// scale times larger along each axis
void upScale(SDL_Surface *&s, Uint8 scale) {
    SDL_Surface *hi = SDL_CreateRGBSurface(0, s->w * scale, s->h * scale, BITDEPTH, 0, 0, 0, 0);
    SDL_BlitScaled(s, NULL, hi, NULL);
    SDL_FreeSurface(s);
    s = hi;
    return;
}

// Load a texture from a file to a sprite in memory
void loadTexture(Sprite *s, const char *path, int scale = 1) {
    if (std::regex_match(path, std::regex(".*\\.bmp"))) {
        SDL_Surface *out = SDL_LoadBMP(path);
        upScale(out, scale);
        s->surface = out;
    } else {
        throw std::runtime_error("Unrecognized filetype; cannot load texture");
    }
    return;
}

/////////////////////////////////////

SDL_Surface *loadTexture(const char *path, int scale = 1) {
    if (std::regex_match(path, std::regex(".*\\.bmp"))) {
        SDL_Surface *out = SDL_LoadBMP(path);
        upScale(out, scale);
        return out;
    } else {
        throw std::runtime_error("Unrecognized filetype; cannot load texture");
    }

    /*std::ifstream in;

    in.open(path);
    if (!in.is_open()) {
        in.close();
        throw std::runtime_error("Failed to open texture");
    }
    
    int format, h, w, bd;
    PIXEL_TYPE temp;
    //in >> format >> w >> h >> bd;
    in.read(reinterpret_cast<char*>(&format), sizeof(format));
    in.read(reinterpret_cast<char*>(&w), sizeof(w));
    in.read(reinterpret_cast<char*>(&h), sizeof(h));
    in.read(reinterpret_cast<char*>(&bd), sizeof(bd));


    if (bd != BITDEPTH) {
        throw std::runtime_error("Invalid texture!");
    }

    auto s = SDL_CreateRGBSurface(0, w * scale, h * scale, BITDEPTH, 0, 0, 0, 0);
    PIXEL_TYPE *pixels = (PIXEL_TYPE*)s->pixels;

    int num;
    switch (format) {
    case 0: // compressionless
        // rgba rgba rgba etc
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                //in >> temp;
                in.read(reinterpret_cast<char*>(&temp), sizeof(temp));
                for (int j = 0; j < scale; j++) {
                    for (int k = 0; k < scale; k++) {
                        pixels[((r * scale) * (w * scale)) + (j * w * scale) + (c * scale) + k] = temp;
                    }
                }
            }
        }
        break;
    case 1: // basic compression
        // num rgba num rgba etc
        num = 0;
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                if (num <= 0) {
                    //in >> num >> temp;
                    in.read(reinterpret_cast<char*>(&num), sizeof(num));
                    in.read(reinterpret_cast<char*>(&temp), sizeof(temp));
                }
                for (int j = 0; j < scale; j++) {
                    for (int k = 0; k < scale; k++) {
                        pixels[((r * scale) * (w * scale)) + (j * w * scale) + (c * scale) + k] = temp;
                    }
                }
                num--;
            }
        }
        
        break;
    default:
        throw std::runtime_error("Unrecognized filetype");
        break;
    };

    in.close();

    return s;*/
}

/////////////////////////////////////

// Tile a surface <into> with the contents of surface <from>
// such that <from>'s pattern is repeated across <into>
void tile(SDL_Surface *from, SDL_Surface *into) {
    PIXEL_TYPE *sourcePixels = (PIXEL_TYPE*)from->pixels;
    PIXEL_TYPE *pixels = (PIXEL_TYPE*)into->pixels;
    int source, destination;

    for (int x = 0; x < into->w; x += from->w) {
        for (int y = 0; y < into->h; y += from->h) {
            for (int r = 0; r < from->h; r++) {
                for (int c = 0; c < from->w; c++) {
                    // Determine where from and where to
                    source = (r * from->w) + c;
                    destination = ((r + y) * into->w) + c + x;

                    // Ignore if out of bounds
                    if (destination > 0 && destination < into->h * into->w) {
                        pixels[destination] = sourcePixels[source];
                    }
                }
            }
        }
    }

    return;
}

// Tile with a scaling aspect on the <from> surface
void tile(SDL_Surface *into, char *path, int scale) {
    SDL_Surface *from = loadTexture(path, scale);
    tile(from, into);
    return;
}

// Tile from a path, returning a newly created surface
SDL_Surface *tile(int w, int h, char *path, int scale = 1, Uint8 bd = BITDEPTH) {
    SDL_Surface *temp = SDL_CreateRGBSurface(0, w, h, bd, 0, 0, 0, 0);
    tile(temp, path, scale);
    return temp;
}

/////////////////////////////////////

// Stamp a suface <onto> with <sprite> according to its current location
void stamp(Sprite *sprite, SDL_Surface *onto) {
    int source, destination;

    // Place pixels in their correct positions
    for (int r = 0; r < sprite->surface->h; r++) {
        for (int c = 0; c < sprite->surface->w; c++) {

            // Determine where from and where to
            source = (r * sprite->surface->w) + c;
            destination = ((r + *sprite->y) * onto->w) + c + *sprite->x;

            if (c + *sprite->x + 1 > onto->w || c + *sprite->x < 0) continue;

            // Ignore if out of bounds
            if (destination > 0 && destination < onto->h * onto->w) {
                // Place pixel (skipping transparency)
                if (((PIXEL_TYPE*)sprite->surface->pixels)[source] != 0) ((PIXEL_TYPE*)onto->pixels)[destination] = ((PIXEL_TYPE*)sprite->surface->pixels)[source];
            }
        }
    }
    return;
}

/////////////////////////////////////

// Invert a surface's pixels such that its left is now its right
void invertX(SDL_Surface *s) {
    PIXEL_TYPE temp, *pixels;
    pixels = (PIXEL_TYPE*)s->pixels;
    for (int y = 0; y < s->h; y++) {
        for (int x = 0; x < (s->w - x); x++) {
            temp = pixels[(y * s->w) + x];
            pixels[(y * s->w) + x] = pixels[(y * s->w) + (s->w - x)];
            pixels[(y * s->w) + (s->w - x)] = temp;
        }
    }
}

// Invert a surface's pixels such that its top is now its bottom
void invertY(SDL_Surface *s) {
    PIXEL_TYPE temp, *pixels;
    pixels = (PIXEL_TYPE*)s->pixels;
    for (int x = 0; x < s->h; x++) {
        for (int y = 0; y < (s->h - y); y++) {
            temp = pixels[(y * s->w) + x];
            pixels[(y * s->w) + x] = pixels[((s->h - y) * s->w) + x];
            pixels[((s->h - y) * s->w) + x] = temp;
        }
    }
}

/////////////////////////////////////

#endif
