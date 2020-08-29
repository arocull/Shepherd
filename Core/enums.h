#pragma once

// Used for determining what tile and collision is drawn and utilized on a given space
enum TileID {   // : int_fast8_t
    ET_Wall = 1,
    ET_Water = 2,
    ET_Magma = 3,
    ET_Tree = 4,
    ET_Rock = 5,
    ET_Pillar = 6,
    ET_Empty_Puzzle_Piece = 7,
    ET_Pressure_Plate = 8,
    ET_Fake_Wall = 9,
    ET_Empty_Tile = 10,
    ET_Door_Vertical = 11,
    ET_Door_Horizontal = 12,
    ET_Ice = 13,
    ET_Vines = 14,
    ET_Scroll = 15,    // Scrolls are unique triggerables, but still need to be rendered

    ET_None = 0,       // Empty space
    ET_Trigger1 = -1,  // Triggers
    ET_Trigger2 = -2,
    ET_Trigger3 = -3,
    ET_Trigger4 = -4,  // Trigger 4 has no debounce, it is called repeatedly as long as player is ontop of it
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

    EE_None = 0,
};

// Used for determining what menu is currently being accessed
enum MenuID {
    EM_Pause = 0,
    EM_Map = 1,
    EM_Scrolls = 2,
    EM_Settings = 3
};