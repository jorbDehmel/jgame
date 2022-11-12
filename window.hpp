#ifndef WINDOW_H
#define WINDOW_H

#define DEBUG
#define STEADY_FRAMERATE

#include <SDL2/SDL.h>
#include <stdexcept>
#include <bitset>

#ifdef STEADY_FRAMERATE
    #include <chrono>
#endif

/////////////////////////////////////

Uint8 BITDEPTH = 32;
#define PIXEL_TYPE Uint32

#define KEYARRSIZE 332

std::bitset<KEYARRSIZE + (4 - (KEYARRSIZE % 4))> KEYS;
Uint8 MOUSESTATE;
int MOUSE_X, MOUSE_Y;
bool ISRUNNING = true;

#ifdef STEADY_FRAMERATE
    std::chrono::_V2::steady_clock::time_point prev_time, cur_time;
    int toWait;
#endif

/////////////////////////////////////

// A window on the screen. Has event handling and automatic frame updating.
// Optimized for computer use, IE will not work without a keyboard and mouse.
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

// Takes height, width, refresh time in milliseconds, window title, 
// frame updating function by reference, SDL window flag, and bitdepth.
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

// Transfer any occured mouse movements or keyboard presses to 
// their associated metavariables
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

// Call the (given at instantiation) update frame function and delay by 
// the pre-given refresh time.
void Window::runFrame() {
    

    update(SDL_GetWindowSurface(window));
    SDL_UpdateWindowSurface(window);
    
    #ifdef STEADY_FRAMERATE
        cur_time = std::chrono::steady_clock::now();
        toWait = (REFRESH_TIME - (std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - prev_time).count()));
        prev_time = cur_time;

        if (toWait > 0 && toWait < REFRESH_TIME) {
            SDL_Delay(toWait);
        }
    #else
        SDL_Delay(toWait);
    #endif

    return;
}

// Repeatedly handle events and update the screen until the global ISRUNNING
// is false. Then, gracefully exit SDL.
void Window::mainLoop() {
    while (ISRUNNING) {
        scanEvents();
        runFrame();
    }
    kill();
    return;
}

// Shutdown SDL
void Window::kill() {
    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}

/////////////////////////////////////

#endif
