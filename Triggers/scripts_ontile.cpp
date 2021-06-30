#include "Triggers/scripts_ontile.h"

Script::OnTileFunc** Script::list_OnTile;

void Script::Init_OnTile(int largestMapID) {
    largestMapID++; // Add one to total, we want to be inclusive

    list_OnTile = (Script::OnTileFunc**) malloc(largestMapID * sizeof(Script::OnTileFunc*));
    
    // Default all maps to do nothing
    for (int i = 0; i < largestMapID; i++) {
        list_OnTile[i] = &Script::OnTile_None;
    }

    // Assign map-specific functions
    list_OnTile[1] = &Script::OnTile_Map1;
    list_OnTile[5] = &Script::OnTile_Map5;
    list_OnTile[6] = &Script::OnTile_Map6;
    list_OnTile[8] = &Script::OnTile_Map8;
    list_OnTile[13] = &Script::OnTile_Map13;
    list_OnTile[17] = &Script::OnTile_Map17;
    list_OnTile[29] = &Script::OnTile_Map29;
}
void Script::Free_OnTile() {
    free(list_OnTile);
}


void Script::OnTile_None(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) { };
void Script::OnTile_Map1(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    window->SetDialogueText("Remember to have all your sheep gathered nearby before you leave.", 0);
}
void Script::OnTile_Map5(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    window->SetDialogueText("You peer over the pitfall into the ravine. You don't remember falling in.", 100);
}
void Script::OnTile_Map6(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    window->SetDialogueText("The lever is covered in the sandy dust of ages. Perhaps a good swing of your staff will brush it off...", 0);
}
void Script::OnTile_Map8(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    switch (triggerID) {
        case 1:
            window->SetDialogueText("You sigh with relief as you climb out of the sandstone ravine. The sheep may survive after all.", 100);
            break;
        case 2:
            window->SetDialogueText("A large pyramid looms overhead. The gate is covered in strange glyphs.", 100);
            break;
        case 3:
            window->SetDialogueText("The torches are identified with unique glyphs. Stone moulding runs into the sand below them, disappearing from view.", 120);
        default:
            break;
    }
}
void Script::OnTile_Map13(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    if (triggerID == 1) {
        window->SetDialogueText("The air is dry, but cool. Your footsteps echo around the walls of the corridor as you march your flock along.", 100);
        soundService->FadeVolumeMusic(0.0f, 1.0f);
    }
}
void Script::OnTile_Map17(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    window->SetDialogueText("Fire from torches can be picked up with a swing of your staff.", 0);
}
void Script::OnTile_Map29(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    if (triggerID == 1) { // Warning
        window->SetDialogueText("It's quiet and warm in this room, but you feel chilled and uneasy...", 75);
        soundService->FadeOutMusic(0.5f);
    } else if (triggerID == 2) { // Trap player
        window->AddScreenShake(0, 1.0f);
        window->SetDialogueText("", 0);
        map->SetEventTimer(10, 0);
        map->FillRectangle(0, 11, 1, 14, TileID::ET_Door_Vertical);
        map->FillRectangle(39, 11, 40, 14, TileID::ET_Door_Vertical);
    }
}