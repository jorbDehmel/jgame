------------------------------------------------------

jgame

(Jordan "Jorb" Dehmel, 2022, jdehmel@outlook.com)

A simple game/graphical interface built on top of SDL.

------------------------------------------------------

Using the LevelStage object:

    This object works similarly to a stage, except 
that it can hold multiple "levels". It stores paths to
"level".txt files (explained later), which are loaded 
into memory when they are needed.

Initializing

    the LevelStage object takes a path to a "levelList.txt"
file upon initialization. This file should be structured
as follows:

-------begin example-------
# Comments must start their lines with pound signs
<int number of levels n>
<path to level 1>
#et cetra
<path to level n>
-------end example-------

    As a concrete example, an instance with one level stored
at ../levels/level.txt looks as follows:

-------begin example-------
1
../levels/level.txt
-------end example-------

Writing level.txt files

    Level .txt files must take the following form:

-------begin example-------
# Sprite 1's data follows:
name x y layer sprite_tag scale/w NULL/h numFrames loading_tag
path[1]
# any number of frame paths here
path[numFrames]
-------end example-------

    As a concrete example:

-------begin example-------
bg 0 0 0 BACKGROUND 64 32 1 TILE
textures/bgtile.bmp

player 0 0 1 PLAYER 8 0 1 DIRECT
textures/player.bmp

font 0 0 2 LABEL 1 0 49 FOLDER
../jgame/font
-------end example-------

    The sprite_tag works as in a stage object. the
loading_tag represents how to interpret the paths given
afterwords. It can be either TILE (tile from this
texture path, with the result being w by h), DIRECT
(load from these one or more direct paths to textures)
or FOLDER (load frameNum textures from the following
single folder).