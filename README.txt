jgame

(Jordan "Jorb" Dehmel, 2022, jdehmel@outlook.com)
A simple game/graphical interface built on top of SDL.



Using the Window object:

    Starts and maintains a SDL_Window, but with easier
access and abstracted management. Also maintains a list
of pressed keys and other user input events. Takes a
function at declaration that is called to update the
screen every time it is updated. This function takes the
form void update(Surface *frameToUpdate). It also takes
width, height, and refresh time (in milliseconds to wait
per frame).



Using sprites and smartSprites
    
    Sprites hold references to variables defining their
x, y, and layer position in a stage. They also hold a
SPRITE_TYPE object representing their designation and a
Surface reference representing their texture to render.

    Sprite types can be one of the following: PLAYER,
BACKGROUND, DECORE, FLOOR, ROOF, WALL, BLOCK (meaning a
combination floor, wall, and roof), ENEMY, ITEM, LABEL,
and NPC.

    SmartSprites hold all that sprites hold, as well as
a direction indicator and the capacity for multiple frames.
Additionally, they contain their own x, y, and layer
positions rather than holding pointers to external variables.
They can be initialized with either an array of pre-loaded
Surface* objects or with a path to a folder full of textures.
All textures are loaded via textures.hpp, and thus must
use the bitmap format.



Textures

    The textures.hpp header file provides a light wrapper
for SDL's loading of BMP images. It has functions for
upscaling Surface* frames, for loading bmp files from
paths to Surface* objects, for tiling a surface from
another or from a file, for stamping a sprite onto a surface,
and for inverting a surface's x or y.



Loading and playing WAV files

    First, call initializeAudio() (this is technically 
optional, as loadWAV called this function). Second, create
a WAV object. You can then use the loadWAV function to
load from a filepath. Following this, the playWAV function
can be called on said WAV object, which will play the sound.

Example:
    
    initializeAudio();
    WAV exampleSound;

    loadWAV("examplepath.wav", exampleSound);
    playWAV(exampleSound);
    SDL_Delay(5000);

    closeAudio();
    SDL_Quit();


Game drivers

    Game drivers link to your main file and connect with
loaded stage objects (which can be obtained from
initialized levelStage objects) to drive a specific game
style.



ScrollDriver

    This drives a top-down RPG-style game via the WASD
keys. It checks for collisions with the player and calls 
a function depending on what type the other sprite is.
These functions are given via the associate(type, function)
method.



Using fonts

    The font.hpp file provides the write() function,
which takes a properly set up font SmartSprite, a message
to write to the screen, a surface to write to, and a 
x value at which to perform a carriage return. A font
texture folder is including in jgame, which can be used
to load a SmartSprite. If you want to write a font sprite
that is compatible with this header file, you must follow
the naming conventions of this included font folder.

    Honestly, it might be easier to write your own
write() function, as this one is highly specific to the
way this font folder has been set up.



Events:

    Events are stored in global variables and are
maintained by the current open Window object (if you try
to run multiple windows, there are no guarentees events
will act accordingly).

    The current state of any pressed key is stored in
KEYS[<keyname>], where keyname is its associated entry
in keys::<name>. For instance, KEYS[keys::space] or
KEYS[keys::esc]. For alphanumeric (lowercase) keys, you
can also use KEYS['<lettername>'] (IE KEYS['a']).

    The current mouse state is stored in MOUSESTATE.
This can be decoded using the SDL_BUTTON(MOUSESTATE)
to get the associated mouse button. The mouse x and y
are maintained in MOUSE_X and MOUSE_Y respectively.



Using the Stage object:

    The stage holds a list of Sprites (holding a frame,
position, and other metadata about an object in the game)
which are rendered to a Surface when update() is called.
These sprites can have their positions changed and update()
will render them accordingly.



Using the LevelStage object:

    This object works similarly to a stage, except 
that it can hold multiple "levels". It stores paths to
"level".txt files (explained later), which are loaded 
into memory when they are needed.

Initializing

    the LevelStage object takes a path to a "levelList.txt"
file upon initialization. This file should be structured
as follows:

    # Comments must start their lines with pound signs
    <int number of levels n>
    <path to level 1>
    #et cetra
    <path to level n>

    As a concrete example, an instance with one level stored
at ../levels/level.txt looks as follows:

    3
    ../levels/level.txt
    ../levels/level2.txt
    ./level3ThatIsInAWeirdPlace.txt



Writing level.txt files

    Level .txt files must take the following form
in any .txt file:

    # Sprite 1's data follows:
    name x y layer sprite_tag scale/w NULL/h numFrames loading_tag
    path[1]
    # any number of frame paths here
    path[numFrames]

    Take the following level.txt file containing a
background, a player, and a font:

    bg 0 0 0 BACKGROUND 64 32 1 TILE
    textures/bgtile.bmp

    player 0 0 1 PLAYER 8 0 1 DIRECT
    textures/player.bmp

    font 0 0 2 LABEL 1 0 49 FOLDER
    ../jgame/font

    The sprite_tag works as in a stage object. the
loading_tag represents how to interpret the paths given
afterwords. It can be either TILE (tile from this
texture path, with the result being w by h), DIRECT
(load from these one or more direct paths to textures)
or FOLDER (load frameNum textures from the following
single folder).



License

Copyright © 2022 Jordan Dehmel

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the “Software”), to deal in the 
Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions 
of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF 
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.