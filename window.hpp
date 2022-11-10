#ifndef WINDOW_H
#define WINDOW_H

#define DEBUG

#include <SDL2/SDL.h>
#include <stdexcept>
#include <bitset>

/////////////////////////////////////

Uint8 BITDEPTH = 32;
#define PIXEL_TYPE Uint32

#define KEYARRSIZE 332

std::bitset<KEYARRSIZE + (4 - (KEYARRSIZE % 4))> KEYS;
Uint8 MOUSESTATE;
int MOUSE_X, MOUSE_Y;
bool ISRUNNING = true;

/////////////////////////////////////

class Window {
public:
    Window(Uint16 h, Uint16 w, Uint16 rt, char *t, void (*update)(SDL_Surface *frame),
        SDL_WindowFlags windowFlag, Uint8 bd);
    
    void scanEvents();
    void runFrame();
    void mainLoop();
    void kill();

    void (*update)(SDL_Surface *frame);
    
    Uint16 HEIGHT;
    Uint16 WIDTH;
    Uint16 REFRESH_TIME;

    char *TITLE;

    SDL_Window *window;
    SDL_Event event;
};

/////////////////////////////////////

Window::Window(Uint16 h, Uint16 w, Uint16 rt, char *t, void (*updateFunc)(SDL_Surface *frame),
        SDL_WindowFlags windowFlag = SDL_WINDOW_OPENGL, Uint8 bd = BITDEPTH) {
    
    HEIGHT = h;
    WIDTH = w;
    REFRESH_TIME = rt;
    TITLE = t;
    update = updateFunc;

    BITDEPTH = bd;

    // Initialize globals
    KEYS = 0;
    MOUSESTATE = '\0';
    MOUSE_X = MOUSE_Y = 1;

    ISRUNNING = true;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        windowFlag
    );

    if (window == nullptr) {
        throw std::runtime_error("Could not create window");
    }

    SDL_SetSurfaceRLE(SDL_GetWindowSurface(window), 1);

    return;
}

void Window::scanEvents() {
    // Event handling
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym < KEYARRSIZE) {
                    KEYS[event.key.keysym.sym] = true;
                }
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym < KEYARRSIZE) {
                    KEYS[event.key.keysym.sym] = false;
                }
                break;

            case SDL_MOUSEMOTION:
                MOUSE_X = event.motion.x;
                MOUSE_Y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                MOUSESTATE = SDL_GetMouseState(NULL, NULL);
                break;
            case SDL_MOUSEBUTTONUP:
                MOUSESTATE = '\0';
                break;

            case SDL_QUIT:
                ISRUNNING = false;
                break;
        }
    }

    return;
}

void Window::runFrame() {
    update(SDL_GetWindowSurface(window));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(REFRESH_TIME);
    
    return;
}

void Window::mainLoop() {
    while (ISRUNNING) {
        scanEvents();
        runFrame();
    }
    kill();
    return;
}

void Window::kill() {
    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}

/////////////////////////////////////

#endif
