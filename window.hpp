#ifndef WINDOW_H
#define WINDOW_H

#include "jgame.hpp"

/////////////////////////////////////

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned long long u128;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;
typedef long long i128;

typedef float f32;
typedef double f64;
typedef long double f128;

typedef SDL_Surface Surface;
typedef SDL_Window RawWindow;

/////////////////////////////////////

std::set<int> KEYS_SET;
class KeyArrType {
public:
    inline bool operator[] (int key) {
        return (KEYS_SET.find(key) != KEYS_SET.end());
    }

    inline bool empty() {
        return KEYS_SET.empty();
    }

    inline bool any() {
        return !KEYS_SET.empty();
    }

    inline void clear() {
        KEYS_SET.clear();
        return;
    }

    inline void insert(int key) {
        KEYS_SET.insert(key);
        return;
    }

    inline void erase(int key) {
        KEYS_SET.erase(key);
        return;
    }

    inline int size() {
        return KEYS_SET.size();
    }

    std::set<int>::iterator begin() {
        return KEYS_SET.begin();
    }

    std::set<int>::iterator end() {
        return KEYS_SET.end();
    }
};
KeyArrType KEYS;

/////////////////////////////////////

u8 BITDEPTH = 32;
#define PIXEL_TYPE u32

u8 MOUSESTATE;
i32 MOUSE_X, MOUSE_Y;
bool ISRUNNING = true;

/////////////////////////////////////

// A window on the screen. Has event handling and automatic frame updating.
// Optimized for computer use, IE will not work without a keyboard and mouse.
class Window {
public:
    Window(u16 h, u16 w, u16 rt, char *t, void (*update)(SDL_Surface *frame),
        SDL_WindowFlags windowFlag, u8 bd);
    
    void scanEvents();
    void runFrame();
    void mainLoop();
    void kill();

    void (*update)(SDL_Surface *frame);
    
    u16 HEIGHT;
    u16 WIDTH;
    u16 REFRESH_TIME;

    char *TITLE;

    SDL_Window *window;
    SDL_Event event;

    u32 prevTicks;
    i16 toWait;
};

/////////////////////////////////////

// Takes height, width, refresh time in milliseconds, window title, 
// frame updating function by reference, SDL window flag, and bitdepth.
Window::Window(u16 h, u16 w, u16 rt, char *t, void (*updateFunc)(SDL_Surface *frame),
        SDL_WindowFlags windowFlag = SDL_WINDOW_OPENGL, u8 bd = BITDEPTH) {
    
    HEIGHT = h;
    WIDTH = w;
    REFRESH_TIME = rt;
    TITLE = t;
    update = updateFunc;

    BITDEPTH = bd;

    // Initialize globals
    KEYS.clear();
    MOUSESTATE = '\0';
    MOUSE_X = MOUSE_Y = 1;

    ISRUNNING = true;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);

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

    return;
}

// Transfer any occured mouse movements or keyboard presses to 
// their associated metavariables
void Window::scanEvents() {
    // Event handling
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                KEYS.insert((int)event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                KEYS.erase(event.key.keysym.sym);
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
    prevTicks = SDL_GetTicks();

    update(SDL_GetWindowSurface(window));
    SDL_UpdateWindowSurface(window);

    toWait = SDL_GetTicks() - prevTicks;
    if (REFRESH_TIME - toWait > 0) {
        SDL_Delay(REFRESH_TIME - toWait);
    }

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
