#ifndef TEXTURES_H
#define TEXTURES_H

#include "jgame.hpp"

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
        
        if (out == nullptr) {
            throw std::runtime_error("Unable to load texture");
        }
        
        upScale(out, scale);
        s->surface = out;

        s->rect.w = s->surface->w;
        s->rect.h = s->surface->h;
    } else {
        throw std::runtime_error("Unrecognized filetype; cannot load texture");
    }
    return;
}

/////////////////////////////////////

SDL_Surface *loadTexture(const char *path, int scale = 1) {
    if (std::regex_match(path, std::regex(".*\\.bmp"))) {
        SDL_Surface *out = SDL_LoadBMP(path);
        if (out == nullptr) throw std::runtime_error("Unable to load texture");
        upScale(out, scale);
        return out;
    } else {
        throw std::runtime_error("Unrecognized filetype; cannot load texture");
    }
}

/////////////////////////////////////

// Tile a surface <into> with the contents of surface <from>
// such that <from>'s pattern is repeated across <into>
void tile(SDL_Surface *from, SDL_Surface *into) {
    if (from == nullptr || into == nullptr) throw std::runtime_error("Cannot tile nullptrs");
    
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
            destination = ((r + sprite->rect.y) * onto->w) + c + sprite->rect.x;

            if (c + sprite->rect.x + 1 > onto->w || c + sprite->rect.x < 0) continue;

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
        for (int x = 0; x <= (s->w - x); x++) {
            temp = pixels[(y * s->w) + x];
            pixels[(y * s->w) + x] = pixels[(y * s->w) + (s->w - x)];
            pixels[(y * s->w) + (s->w - x)] = temp;
        }
    }
    return;
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
    return;
}

/////////////////////////////////////

#endif
