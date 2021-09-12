#include "Core/game.h"

Game::Game(RenderWindow* gameWindow) {
    window = gameWindow;
    paused = false;
    close = false;

    loadedMapX = 0;
    loadedMapY = 0;
    
    maxMapID = 0;
    tickedThisFrame = false;

    audio = new SoundService();
    menus = new MenuManager(); // Should this be a pointer, or created as a normal base-object like SoundService and Window?
    controller = new Controller(menus, audio); // Player controller input (done as pointer in-case multiple are used in future)
    ai = new AIManager();

    data = new GameData();
}
Game::~Game() {
    Trigger_Free();

    ai->ClearContext();
    menus->Free();
    audio->CloseSoundService();

    delete data;
    delete ai;
    delete controller;
    delete menus;
    delete audio;
}


// LOGIC //
struct InputAction* Game::ProcessInput(SDL_Event* event) {
    struct InputAction* inputAction = new InputAction();
    controller->ProcessInput(event, inputAction, paused, window->InFullscreen());

    paused = inputAction->pause; // Pause / Unpause game
    if (inputAction->fullscreen != window->InFullscreen()) { // Toggle fullscreen
        window->ToggleFullscreen(inputAction->fullscreen);
        menus->ToggleFullscreen(inputAction->fullscreen);
    }

    return inputAction;
}
void Game::Tick() {
    data->ticks++;
    window->LogTick();
    tickedThisFrame = true;

    // Tally sheep
    int sheepLeft = 0;
    for (int i = 0; i < MaxEntities && sheepLeft < MaxSheep; i++) {
        if (data->entities[i] && data->entities[i]->GetID() == EntityID::EE_Sheep) sheepLeft++;
    }
    
    // If we lost a sheep, trigger game over
    if (sheepLeft < MaxSheep) {
        Trigger_GameOver(window, audio, data->map, data->entities);
        data->player->Paused = true;
    }

    // If the player sits still, feed them hints or story
    data->player->ticksIdled++;
    if (data->player->ticksIdled == TicksUntilIdle) {
        Trigger_Idled(window, audio, data->map, data->entities);
    }

    // Do map event tick ahead of player movement (so player does not end up ontop of walls)
    if (data->map->TickEventTimer()) {
        Trigger_LevelEvent(window, audio, data->map, data->entities);
    }


    MovePlayer();
}
bool Game::Step(float deltaTime) {
    TickParticles(data->particles, deltaTime);
    window->TickDeltaTime(deltaTime);
    audio->Tick(deltaTime);

    #ifdef DEBUG_MODE
        if (controller->accelerate) {
            tickTimer += deltaTime * TickRate * TickAcceleration;
        } else {
            tickTimer += deltaTime * TickRate;
        }
    #else
        tickTimer += deltaTime * TickRate;
    #endif
    tickTimer += deltaTime;

    if (tickTimer <= 0.5f && Movement::Move_QueueClear) { // Prevent any 'sliding'
            Movement::ClearQueue();
    }

    return tickTimer >= 1;
}
void Game::Draw(float deltaTime) {
    window->UpdateSize();
    SDL_SetRenderDrawColor(window->canvas, 0, 0, 0, 0);
    SDL_RenderClear(window->canvas);

    Map* lvl = data->map;

    window->FillViewportBackground(lvl->GetMapBiome());

    // Draw tiles first
    int currentTileID = 0;
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            currentTileID = lvl->GetTileID(x,y);
            window->DrawTile(
                x,
                y,
                currentTileID,
                IsTileable(currentTileID) ? GetTilingIndex( // If the tile is tileable, get the tiling index
                    lvl->GetTileIDConstrained(x,y-1) == currentTileID,
                    lvl->GetTileIDConstrained(x,y+1) == currentTileID,
                    lvl->GetTileIDConstrained(x+1,y) == currentTileID,
                    lvl->GetTileIDConstrained(x-1,y) == currentTileID
                ) : 0); // Otherwise, simply return zero
        }
    }

    // Draw all entities aside from Shepherd
    for (int i = 0; i < MaxEntities; i++) {
        Entity* obj = data->entities[i];
        if (nullptr != obj && obj->GetID() != EntityID::EE_Shepherd) {
            window->DrawEntity(obj, tickedThisFrame, deltaTime);
        }
    }

    // Draw active particles - enable blending for transparency
    SDL_BlendMode blend;
    SDL_GetRenderDrawBlendMode(window->canvas, &blend);
    SDL_SetRenderDrawBlendMode(window->canvas, SDL_BLENDMODE_BLEND);
    // TODO: Object-Oriented particles
    for (int i = 0; i < MaxParticles; i++) {
        struct Particle* particle = &(data->particles[i]);
        if (particle && particle->active)
            window->DrawParticle(particle->x, particle->y, particle->id, particle->lifetime/particle->maxLifetime);
    }
    SDL_SetRenderDrawBlendMode(window->canvas, blend);

    // Draw Shepherd last
    window->DrawEntity(data->player, tickedThisFrame, deltaTime);

    // Draw GUI
    // window->canvasDrawStatusBar(data->player->GetHealth(), lvl->PuzzleStatus);
    window->DrawDialogueBox();
    
    tickedThisFrame = false;
    SDL_RenderPresent(window->canvas);
}


void Game::DrawPauseMenu(float deltaTime) {
    SDL_SetRenderDrawColor(window->canvas, 0, 0, 0, 0);
    SDL_RenderClear(window->canvas);
    window->UpdateSize();

    window->DrawStatusBar(data->player->GetHealth(), data->map->PuzzleStatus);
    window->DrawDialogueBox(menus->activeMenu->optionDesc[menus->activeMenu->optionIndex]);
    window->DrawMenuBackground();
    window->DrawMenu(menus->pauseMenu->getNumOptions(), menus->pauseMenu->optionNames, menus->pauseMenu->optionIndex, menus->inSubmenu());

    if (menus->inSubmenu()) { // Draw submenu as well if it is active
        window->DrawMenu(menus->activeMenu->getNumOptions(), menus->activeMenu->optionNames, menus->activeMenu->optionIndex, false);
    }

    audio->Tick(deltaTime);
    SDL_RenderPresent(window->canvas);
}



// SETUP AND PREPARATION //
void Game::LoadScrolls() {
    int numScrolls = 0; // Total number of scrolls, for scroll data setup
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            if (data->world[x][y]->HasScroll()) numScrolls++;
        }
    }

    menus->InitScrolls(numScrolls); // Initialize scroll list
    int currentScroll = 0;

    // Fill in scroll data
    for (int x = 0; x < WorldWidth && currentScroll < numScrolls; x++) {
        for (int y = 0; y < WorldHeight && currentScroll < numScrolls; y++) {
            if (data->world[x][y]->HasScroll()) {
                data->world[x][y]->SetScrollIndex(currentScroll);
                currentScroll++;
            }
        }
    }
}

void Game::NewGame() {
    using SaveLoad::LoadMapFile;
    Map*** world = data->world;

    // LOAD WORLD
    world[0][2] = LoadMapFile("Assets/Maps/Ravine/Start1");
    world[1][2] = LoadMapFile("Assets/Maps/Ravine/Start2");
    world[2][2] = LoadMapFile("Assets/Maps/Ravine/Start3");
    world[3][2] = LoadMapFile("Assets/Maps/Ravine/Start4");

    world[0][1] = LoadMapFile("Assets/Maps/Desert/Desert1");
    world[1][1] = LoadMapFile("Assets/Maps/Desert/Desert2");
    world[2][1] = LoadMapFile("Assets/Maps/Desert/Desert3");
    world[3][1] = LoadMapFile("Assets/Maps/Desert/GateArea");

    world[0][0] = LoadMapFile("Assets/Maps/Desert/Desert4");
    world[1][0] = LoadMapFile("Assets/Maps/Desert/Desert5");
    world[2][0] = LoadMapFile("Assets/Maps/Desert/Desert6");
    world[3][0] = LoadMapFile("Assets/Maps/Desert/Desert7");

    world[4][2] = LoadMapFile("Assets/Maps/Empty");
    world[4][1] = LoadMapFile("Assets/Maps/Pyramid/PyramidHall1");
    world[4][0] = LoadMapFile("Assets/Maps/Desert/DesertScroll");

    world[5][2] = LoadMapFile("Assets/Maps/Pyramid/Pyramid1");
    world[6][2] = LoadMapFile("Assets/Maps/Pyramid/Pyramid2");
    world[7][2] = LoadMapFile("Assets/Maps/Pyramid/Pyramid3");

    world[5][1] = LoadMapFile("Assets/Maps/Pyramid/Pyramid4");
    world[6][1] = LoadMapFile("Assets/Maps/Pyramid/Pyramid5");
    world[7][1] = LoadMapFile("Assets/Maps/Pyramid/Pyramid6");

    world[5][0] = LoadMapFile("Assets/Maps/Pyramid/Pyramid7");
    world[6][0] = LoadMapFile("Assets/Maps/Pyramid/Pyramid8");
    world[7][0] = LoadMapFile("Assets/Maps/Pyramid/Pyramid9");

    world[8][2] = LoadMapFile("Assets/Maps/Pyramid/PyramidFireballs2");
    world[8][1] = LoadMapFile("Assets/Maps/Pyramid/PyramidFireballs1");
    world[8][0] = LoadMapFile("Assets/Maps/Pyramid/PyramidScroll");

    world[9][2] = LoadMapFile("Assets/Maps/Pyramid/PyramidFireballs3");
    world[9][1] = LoadMapFile("Assets/Maps/Pyramid/PyramidFireballs4");
    world[9][0] = LoadMapFile("Assets/Maps/Pyramid/PyramidFireballs5");

    world[10][2] = LoadMapFile("Assets/Maps/Empty");
    world[10][1] = LoadMapFile("Assets/Maps/Empty");
    world[10][0] = LoadMapFile("Assets/Maps/Pyramid/PyramidBoss");

    // Initialize puzzles
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            Trigger_SetupPuzzles(world[x][y]);
            if (world[x][y]->GetMapID() > maxMapID) { maxMapID = world[x][y]->GetMapID(); }
        }
    }
    Trigger_Init(maxMapID); // Initialize triggers

    // Set up world position
    data->worldX = 0;
    data->worldY = 2;
    #ifdef DEBUG_MODE
        if (DEBUG_SkipGates == 2) {
            data->worldX = 4;
            data->worldY = 1;
        } else if (DEBUG_SkipGates == 3) {
            data->worldX = 7;
            data->worldY = 1;
        } else if (DEBUG_SkipGates >= 4) {
            data->worldX = 9;
            data->worldY = 0;
        }
    #endif

    // Player Setup
    data->player = new Shepherd(20, 8);
    data->entities[0] = data->player;

    // Load level into memory
    LoadLevel(data->world, NULL, data->entities, data->worldX, data->worldY, data->player->x, data->player->y);
    loadedMapX = data->worldX;
    loadedMapY = data->worldY;
    // Start game with intro cutscene
    Trigger_GameStart(window, audio, data->map, data->entities);
}


// LOGIC //

// Move Player - Moves player and loads new maps if necessary
void Game::MovePlayer() {
    Shepherd* player = data->player;

    using namespace Movement;

    if (!player->Paused) {      //If they player is not paused, let them move if input is given
        player->animation = AnimationID::ANIM_Idle;

        player->lastX = player->x;
        player->lastY = player->y;

        if (MoveUp) {
            ShiftPlayer(data, 0, 1);
        } else if (MoveDown) {
            ShiftPlayer(data, 0, -1);
        } else if (MoveRight) {
            ShiftPlayer(data, 1, 0);
        } else if (MoveLeft) {
            ShiftPlayer(data, -1, 0);
        }
    }

    if (Movement::Move_QueueClear) {
        Movement::ClearQueue();
    }


    // Limit world coordinates to actual world
    if (data->worldX >= WorldWidth) {
        data->worldX = WorldWidth-1;
    } else if (data->worldX < 0) {
        data->worldX = 0;
    }
    if (data->worldY >= WorldHeight) {
        data->worldY = WorldHeight-1;
    } else if (data->worldY < 0) {
        data->worldY = 0;
    }

    // If world coordinate has changed, load a new level
    if (data->worldX != loadedMapX || data->worldY != loadedMapY) {
        loadedMapX = data->worldX;
        loadedMapY = data->worldY;
        StopParticles(data->particles);
        data->map = LoadLevel(data->world, data->map, data->entities, data->worldX, data->worldY, player->x, player->y);
        Trigger_LevelLoaded(window, audio, data->world, data->map, data->entities);
        player->lastX = player->x; // Don't draw player teleporting
        player->lastY = player->y;
    }
}