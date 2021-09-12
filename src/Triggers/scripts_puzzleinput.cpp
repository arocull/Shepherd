#include "Triggers/scripts_puzzleinput.h"

Script::PuzzleInputFunc** Script::list_PuzzleInput;

void Script::Init_PuzzleInput(int largestMapID) {
    largestMapID++; // Add one to total, we want to be inclusive

    list_PuzzleInput = (Script::PuzzleInputFunc**) malloc(largestMapID * sizeof(Script::PuzzleInputFunc*));
    
    // Default all maps to do nothing
    for (int i = 0; i < largestMapID; i++) {
        list_PuzzleInput[i] = &Script::PuzzleInput_None;
    }

    // Assign map-specific functions, add functions here using corresponding level ID
    list_PuzzleInput[2] = &Script::PuzzleInput_Map2;
    list_PuzzleInput[3] = &Script::PuzzleInput_Map3;
    list_PuzzleInput[4] = &Script::PuzzleInput_Map4;
    list_PuzzleInput[6] = &Script::PuzzleInput_Map6;
    list_PuzzleInput[7] = &Script::PuzzleInput_Map7;
    list_PuzzleInput[9] = &Script::PuzzleInput_Map9;
    list_PuzzleInput[10] = &Script::PuzzleInput_Map10;
    list_PuzzleInput[11] = &Script::PuzzleInput_GlowFirstTorch;
    list_PuzzleInput[14] = &Script::PuzzleInput_GlowFirstTorch;
    list_PuzzleInput[16] = &Script::PuzzleInput_GlowFirstTorch;
    list_PuzzleInput[17] = &Script::PuzzleInput_Map17;
    list_PuzzleInput[20] = &Script::PuzzleInput_Map20;
    list_PuzzleInput[21] = &Script::PuzzleInput_Map21;
    list_PuzzleInput[22] = &Script::PuzzleInput_GlowFirstTorch;
    list_PuzzleInput[23] = &Script::PuzzleInput_Map23;
    list_PuzzleInput[24] = &Script::PuzzleInput_Map24;
}
void Script::Free_PuzzleInput() {
    free(list_PuzzleInput);
}


// Filler function--does nothing.
void Script::PuzzleInput_None(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) { };

// Causes the first torch found in the entity list to ignite, glow, and become unusable.
void Script::PuzzleInput_GlowFirstTorch(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EE_Torch, 1, MaxEntities), map->PuzzleStatus);
}
void Script::PuzzleInput_Map2(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    #ifdef DEBUG_MODE
        if ((map->PressurePlatesPressed == 1 && !map->PuzzleStatus) || DEBUG_SkipGates >= 1) {
    #else
        if (map->PressurePlatesPressed == 1 && !map->PuzzleStatus) {
    #endif
        map->PuzzleStatus = true;
        map->FillRectangle(37, 5, 38, 10, TileID::ET_None);
        window->AddScreenShake(0, 0.3f);
        window->SetDialogueText("As the pressure plate clicks, the wall slides away into the shifting sands below.", 100);
    }
}
void Script::PuzzleInput_Map3(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    #ifdef DEBUG_MODE
        if (map->PressurePlatesPressed == 1 || DEBUG_SkipGates >= 1) {
    #else
        if (map->PressurePlatesPressed == 1) {
    #endif
        window->AddScreenShake(0, 0.3f);
        map->FillRectangle(37, 5, 38, 10, TileID::ET_None);
    } else {
        window->AddScreenShake(0, 0.3f);
        map->FillRectangle(37, 5, 38, 10, TileID::ET_Door_Vertical);
    }
}
void Script::PuzzleInput_Map4(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    #ifdef DEBUG_MODE
        if (map->PressurePlatesPressed == 2 || DEBUG_SkipGates >= 1) {
    #else
        if (map->PressurePlatesPressed == 2) {
    #endif
        window->AddScreenShake(0, 0.3f);
        map->FillRectangle(27, 6, 28, 9, TileID::ET_None);
        map->Triggers[0] = true;
    } else if (map->PressurePlatesPressed == 1 && !map->Triggers[0]) {
        window->SetDialogueText("Perhaps its time to put the sheep to work. Rest or rally them with spacebar.", 0);
    }
}
void Script::PuzzleInput_Map6(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
}
void Script::PuzzleInput_Map7(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->PressurePlatesPressed == 3) {
        Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), true);
        map->PuzzleStatus = true;
    }
}
void Script::PuzzleInput_Map9(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
    
    Entity* obj = EntityTools::GetEntityAtLocation(entities, 25, 7, MaxEntities); // Get entity on pressure plate (if there is one)
    if (obj && obj->OnPressurePlate) { // If an entity is standing on the pressure plate, lower the wall
        map->FillRectangle(20, 6, 21, 9, TileID::ET_Empty_Puzzle_Piece);
    } else { // Otherwise raise it
        map->FillRectangle(20, 6, 21, 9, TileID::ET_Wall);
    }
}
void Script::PuzzleInput_Map10(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->PressurePlatesPressed == 6) {
        Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), true);
        map->PuzzleStatus = true;
    }
}
void Script::PuzzleInput_Map17(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Trigger_Internal_DisplayPuzzleStatus_Torch(map->Puzzles->entities[0], map->PuzzleStatus);
    Trigger_Internal_DisplayPuzzleStatus_Torch(map->Puzzles->entities[1], map->PuzzleStatus);

    if (map->PuzzleStatus) { // Once puzzle is completed, pull up bridge
        window->AddScreenShake(0, 0.5f);
        map->FillRectangle(11, 6, 31, 9, TileID::ET_Empty_Puzzle_Piece);
        window->SetDialogueText("As the earth shakes, a bridge rises from the depths, agitating the dust of ages.", 100);
    }
}
void Script::PuzzleInput_Map20(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Trigger_Internal_TorchSetup(EntityTools::GetEntityOccurence(entities, EE_Torch, 1, MaxEntities), false, true, map->PuzzleStatus, true, false);
}
void Script::PuzzleInput_Map21(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    Entity* obj1 = EntityTools::GetEntityAtLocation(entities, 20, 6, MaxEntities);
    if (obj1 && obj1->OnPressurePlate) {
        map->FillRectangle(14, 4, 15, 9, TileID::ET_None);
    } else {
        map->FillRectangle(14, 4, 15, 9, TileID::ET_Door_Vertical);
    }

    Entity* obj2 = EntityTools::GetEntityAtLocation(entities, 20, 11, MaxEntities);
    if (obj2 && obj2->OnPressurePlate) {
        map->FillRectangle(26, 4, 27, 9, TileID::ET_None);
    } else {
        map->FillRectangle(26, 4, 27, 9, TileID::ET_Door_Vertical);
    }
}
void Script::PuzzleInput_Map23(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->PressurePlatesPressed == 1 && !map->PuzzleStatus) {
        Entity* spawnTorch = EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities);
        Fireball* fireball = new Fireball(spawnTorch->x, spawnTorch->y + 1, 0, -1, 1);
        fireball->enemy = false;
        EntityTools::AppendEntity(entities, fireball);
        window->SetDialogueText("Fireballs can be caught with a deft swing of your staff.\n", 70);
    }
    if (map->PuzzleStatus && !map->Triggers[0]) {
        map->FillRectangle(9, 6, 31, 9, TileID::ET_Empty_Puzzle_Piece);
        Trigger_Internal_DisplayPuzzleStatus_Torch(EntityTools::GetEntityOccurence(entities, EntityID::EE_Torch, 2, MaxEntities), true);
        window->AddScreenShake(0.0f, 0.5f);
        map->Triggers[0] = true; // Debounce so screenshake doesn't stack
    }
}
void Script::PuzzleInput_Map24(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->PuzzleStatus && !map->Triggers[0]) {
        map->Triggers[0] = true;
        window->AddScreenShake(0.0f, 0.2f);
        map->FillRectangle(39, 1, 40, 4, TileID::ET_Fizzler);
    }
}