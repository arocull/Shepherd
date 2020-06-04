# Shepherd

A small game about a shepherd and his flock of sheep.

This is heavily a work-in-progress and something I do only on my own time when I have it.

## Packages

This game uses SDL2 and SDL2_Mixer as dependencies. To install these packages on Debian, run
`$ apt install libsdl2-dev libsdl2-mixer-dev`

The game compiles uses a [Makefile](Makefile) to compile by executing g++ commands. Run the make file by typing `$ make`

Once the game is compiled, run it with `$ make run`

Commands `$ make clean` and `$ make debug` are also available for cleaning the build and checking memory management respectively.

## Gameplay

There is a tutorial in place that should give you the basic controls and instruct you on how to play.

Press F11 to toggle between fullscreen and windowed mode--aspect ratio is adjusted automatically.
Pressing Escape closes the game.

Note the game is still in its early, bare-bones phases, meaning that there is not much gameplay currently aside from the tutorial puzzles.




## Editing

### Level Editing

Level files can be located within [Map > Maps](Map/Maps), and are merely comprised of text files.
The translations of char to tile/entity can be found within [Map > map_loading.cpp](Map/map_loading.cpp) in the bottom function.

The number and letter below the actual map tiles itself are used for level identification and biome.
Biomes merely affect the background color, whereas the identification is used within [Core > triggers.cpp](Core/triggers.cpp) to tell what level is what.

Below the level identification section is lines that can be used for **scrolls**. Scrolls are special collectibles that can contain various, meaningful text.
The first line after map identification is the name of the scroll. This is what you see when browsing discovered scrolls in the pause menu.
The line below the scroll name is the text the scroll contains when read. It extends the remaining length of the file, just note it has a max length of 256 characters (set in [config.h](Core/config.h)) and all the text must be able to fit in the dialogue box at once.

If you want to set up and configure a puzzle, simply edit the function [Trigger_SetupPuzzles](Core/triggers.cpp) and add a section for your added level.
If the level requires a unique set of inputs, you can add conditions in [Trigger_PuzzleInput](Core/triggers.cpp) for more specific purposes.