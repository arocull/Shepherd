#pragma once

// Used for determining what tile and collision is drawn and utilized on a given space
enum TileID {   // : int_fast8_t
    ET_Wall = 'W',      // Basic wall that blocks player movement
    ET_Water = 'w',     // Basic liquid that the shepherd can move through, but sheep will not, also freezable
    ET_Magma = 'L',     // Liquid that burns anything that touches it
    ET_Tree = 'T',      // Tree, acts as wall but a little more decorative
    ET_Rock = 'R',      // Rock, acts as wall but a little more decorative
    ET_Pillar = 'P',    // Pillar, acts as wall but a little more decorative
    ET_Empty_Puzzle_Piece = 'e', // Grooves in the ground you can slide boxes in--also used for direction
    ET_Pressure_Plate = 'p',     // Pressure plates can be pressed for unique effects, requires programmed triggers
    ET_Fake_Wall = 'F',   // Fake walls look similar to regular walls, but are darker and non-collideable
    ET_Empty_Tile = 'E', // Empty tiles are blank and considered pitfalls--they are not pathable, but fireballs fly over them
    ET_Door_Vertical = 'D',     // Vertically-closed door that can be used for gates
    ET_Door_Horizontal = 'd',   // Horizontally-closed door that can be used for gates
    ET_Ice = 'i',       // Ice is an empty, pathable tile that can be thawed to water using fire
    ET_Vines = 'v',     // Vines, act as a wall but a little more decorative, and use tiling textures
    ET_Scroll = 's',    // Scrolls are unique triggerables, but still need to be rendered
    ET_Indicator = 'I', // Indicator tile for puzzles, drawn as a sun, but has no functionality

    ET_None = '_',     // Empty space
    ET_Trigger1 = '1', // Triggers
    ET_Trigger2 = '2',
    ET_Trigger3 = '3',
    ET_Trigger4 = '4',  // Trigger 4 has no debounce, it is called repeatedly as long as player is ontop of it
    ET_Fizzler = 'f',   // Unique trigger that fizzles out the player's staff (removes fire and frost)
};

// Used for determining what an entity is--used in casting and trigger/interaction functionality
enum EntityID {     // : uint_fast8_t
    EE_Shepherd = 1,
    EE_Sheep = 2,
    EE_Fireball = 3,
    EE_Wolf = 4,
    EE_Crate = 5,
    EE_Torch = 6,
    EE_Lever = 7,
    EE_Spirit = 8,

    EE_PyramidGolem = 101, // Pyramid Golem Boss

    EE_None = 0,
};

// Used for determining what type of particle is being displayed
enum ParticleID {
    EP_Swing = 1,
    EP_PuzzleSolution = 2,
    EP_PressurePlateClick = 3,
    EP_Fire = 4,
    EP_FireBurst = 5,
    EP_Incinerate = 6,
    EP_Spirit = 7,
    EP_GolemSmash = 8,

    EP_None = 0,
};

// Used for determining what menu is currently being accessed
enum MenuID {
    EM_Pause = 0,
    EM_Map = 1,
    EM_Scrolls = 2,
    EM_Settings = 3
};


// Used for determining what background is used for the map
enum EnvironmentID : char {
    ENV_Cave = 'C',
    ENV_Desert = 'D',
    ENV_Pyramid = 'P',
    ENV_Mountain = 'M',
    ENV_Snowy = 'S',
    ENV_Forest = 'F',

    ENV_None = '\0'
};

// Used for determining what animation frames to use when drawing Entities
enum AnimationID {
    ANIM_Idle = 0,
    ANIM_Walk = 1,
    ANIM_Attack = 2,
    ANIM_Paused = 3
};