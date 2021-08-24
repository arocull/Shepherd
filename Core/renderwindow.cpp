#include "renderwindow.h"

RenderWindow::RenderWindow() {
    RenderWindow(400, 400, "SDL Render Window");
}
RenderWindow::RenderWindow(int viewportX, int viewportY, const char* windowName) {
    x = viewportX;
    y = viewportY;

    if (SDL_CreateWindowAndRenderer(viewportX, viewportY, SDL_WINDOW_RESIZABLE, &window, &canvas)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window and renderer: %s", SDL_GetError());
        initialized = false;
    } else
        initialized = true;

    if (initialized) {
        UpdateSize();
        SDL_SetWindowTitle(window, windowName);

        SetDialogueText("", 0);

        TEXTURESURFACE_PROGRAM_loadscreen = SDL_LoadBMP("Assets/Textures/Loading.bmp");
        if (TEXTURESURFACE_PROGRAM_loadscreen) {
            TEXTURE_PROGRAM_loadscreen = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_PROGRAM_loadscreen);
            LoadScreen();
        }

        TEXTURESURFACE_PROGRAM_icon = SDL_LoadBMP("Assets/Textures/Icon.bmp");
        if (TEXTURESURFACE_PROGRAM_icon)
            SDL_SetWindowIcon(window, TEXTURESURFACE_PROGRAM_icon);



        TEXTURESURFACE_tree = SDL_LoadBMP("Assets/Textures/Tree.bmp");
        if (TEXTURESURFACE_tree) {
            TEXTURE_tree = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_tree);
        }

        TEXTURESURFACE_rock = SDL_LoadBMP("Assets/Textures/Rock.bmp");
        if (TEXTURESURFACE_rock) {
            TEXTURE_rock = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_rock);
        }

        TEXTURESURFACE_pillar = SDL_LoadBMP("Assets/Textures/Pillar.bmp");
        if (TEXTURESURFACE_pillar) {
            TEXTURE_pillar = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_pillar);
        }

        TEXTURESURFACE_vines = SDL_LoadBMP("Assets/Textures/Vines.bmp");
        if (TEXTURESURFACE_vines) {
            TEXTURE_vines = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_vines);
        }

        TEXTURESURFACE_sunTile = SDL_LoadBMP("Assets/Textures/SunTile.bmp");
        if (TEXTURESURFACE_sunTile) {
            TEXTURE_sunTile = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_sunTile);
        }

        TEXTURESURFACE_crate = SDL_LoadBMP("Assets/Textures/Crate.bmp");
        if (TEXTURESURFACE_crate) {
            TEXTURE_crate = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_crate);
        }

        TEXTURESURFACE_torch = SDL_LoadBMP("Assets/Textures/Torch.bmp");
        if (TEXTURESURFACE_torch) {
            TEXTURE_torch = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_torch);
        }

        TEXTURESURFACE_lever = SDL_LoadBMP("Assets/Textures/Lever.bmp");
        if (TEXTURESURFACE_lever) {
            TEXTURE_lever = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_lever);
        }

        TEXTURESURFACE_sheep = SDL_LoadBMP("Assets/Textures/Sheep.bmp");
        if (TEXTURESURFACE_sheep) {
            TEXTURE_sheep = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_sheep);
        }

        TEXTURESURFACE_wolf = SDL_LoadBMP("Assets/Textures/Wolf.bmp");
        if (TEXTURESURFACE_wolf) {
            TEXTURE_wolf = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_wolf);
        }

        TEXTURESURFACE_shepherd = SDL_LoadBMP("Assets/Textures/Shepherd.bmp");
        if (TEXTURESURFACE_shepherd) {
            TEXTURE_shepherd = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_shepherd);
        }

        TEXTURESURFACE_boss_pyramidgolem = SDL_LoadBMP("Assets/Textures/PyramidGolem.bmp");
        if (TEXTURESURFACE_boss_pyramidgolem) {
            TEXTURE_boss_pyramidgolem = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_boss_pyramidgolem);
        }

        TEXTURESURFACE_alphabet = SDL_LoadBMP("Assets/Textures/Alphabet.bmp");
        if (TEXTURESURFACE_alphabet) {
            TEXTURE_alphabet = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_alphabet);
        }
    }
}


bool RenderWindow::IsInitialized(){
    return initialized;
}
void RenderWindow::ToggleFullscreen(bool setFullscreen) {
    if (setFullscreen) {
        windowedX = x;
        windowedY = y;
        
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); //Enforce borderless fullscreen without screwing up resolution
        fullscreen = true;
    } else {
        SDL_SetWindowFullscreen(window, 0); //Exit fullscreen
        SDL_SetWindowSize(window, windowedX, windowedY);
        fullscreen = false;
    }
}
void RenderWindow::ToggleFullscreen() {
    ToggleFullscreen(!fullscreen);
}
bool RenderWindow::InFullscreen() {
    return fullscreen;
}
void RenderWindow::UpdateSize() {
    SDL_GetWindowSize(window, &x, &y);

    if (dialogueBoxVisible)
        dialogueBoxY = y*DialogueBoxScale;
    else
        dialogueBoxY = 0;
    
    statusBarY = y*StatusBarScale;

    dialogueBoxLineHeight = dialogueBoxY/DialogueBoxLines - dialogueBoxY*DialogueBoxLineSpacing;

    tileRes = fmin(x/MapWidth, (y - dialogueBoxY - statusBarY)/MapHeight);
    offsetXBase = (x - MapWidth*tileRes)/2;
    offsetYBase = (y - MapHeight*tileRes - dialogueBoxY - statusBarY)/2 + statusBarY;
    innerWidth = tileRes*MapWidth;
    innerHeight = tileRes*MapHeight;
    offsetX = offsetXBase;
    offsetY = offsetYBase;
}
void RenderWindow::TickDeltaTime(float DeltaTime) {
    time+=DeltaTime;
    tickAlpha += DeltaTime * TickRate;

    // Impact effect is intense, but drops off fast
    if (screenShakeX < 0) screenShakeX = 0;
    else if (screenShakeX > 0) {
        screenShakeX -= DeltaTime;
        offsetX = offsetXBase + (int) (sinf(time * 10 * PI) * 10 * (powf(5, screenShakeX) - 1)); // 10^0 - 1 = 1 - 1 = 0, should be seamless at zero
    }

    // Rumble effect is more mild, but can persist
    if (screenShakeY < 0) screenShakeY = 0;
    else if (screenShakeY > 0) {
        if (!continuousRumble) screenShakeY -= DeltaTime; // Continuous Rumble in case of cutscenes
        offsetY = offsetYBase + (int) (sinf(time * 3 * PI) * screenShakeY * 22); // Appears much more mild
    }
}
void RenderWindow::LogTick() {
    ticks++;
    tickAlpha = 0.0f;

    // Check if we we have any dialogue text waiting to be cleared
    if (dialogueTicksLeft > 0) {
        dialogueTicksLeft--;
        if (dialogueTicksLeft == 0 && dialogueText && dialogueText[0]) {   //If its time is up, clear it
            dialogueText[0] = '\0'; //Lazy clear, sets first character to a string terminator, but is fast

            //If you wanted to clear the whole buffer, you would do:
            //memset(dialogueText,'\0',strlen(dialogueText));
        }
    }
}



void RenderWindow::Close() {
    if (!initialized) return;

    free(dialogueText);

    SDL_FreeSurface(TEXTURESURFACE_PROGRAM_loadscreen);
    SDL_DestroyTexture(TEXTURE_PROGRAM_loadscreen);
    SDL_FreeSurface(TEXTURESURFACE_PROGRAM_icon);

    SDL_FreeSurface(TEXTURESURFACE_tree);
    SDL_DestroyTexture(TEXTURE_tree);
    SDL_FreeSurface(TEXTURESURFACE_rock);
    SDL_DestroyTexture(TEXTURE_rock);
    SDL_FreeSurface(TEXTURESURFACE_pillar);
    SDL_DestroyTexture(TEXTURE_pillar);
    SDL_FreeSurface(TEXTURESURFACE_vines);
    SDL_DestroyTexture(TEXTURE_vines);
    SDL_FreeSurface(TEXTURESURFACE_sunTile);
    SDL_DestroyTexture(TEXTURE_sunTile);
    SDL_FreeSurface(TEXTURESURFACE_crate);
    SDL_DestroyTexture(TEXTURE_crate);
    SDL_FreeSurface(TEXTURESURFACE_torch);
    SDL_DestroyTexture(TEXTURE_torch);
    SDL_FreeSurface(TEXTURESURFACE_lever);
    SDL_DestroyTexture(TEXTURE_lever);
    SDL_FreeSurface(TEXTURESURFACE_sheep);
    SDL_DestroyTexture(TEXTURE_sheep);
    SDL_FreeSurface(TEXTURESURFACE_wolf);
    SDL_DestroyTexture(TEXTURE_wolf);
    SDL_FreeSurface(TEXTURESURFACE_shepherd);
    SDL_DestroyTexture(TEXTURE_shepherd);
    SDL_FreeSurface(TEXTURESURFACE_boss_pyramidgolem);
    SDL_DestroyTexture(TEXTURE_boss_pyramidgolem);
    SDL_FreeSurface(TEXTURESURFACE_alphabet);
    SDL_DestroyTexture(TEXTURE_alphabet);

    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
}



void RenderWindow::FillViewportBackground(EnvironmentID environment) {
    SDL_Rect box;
    box.x = offsetX;
    box.y = offsetY;
    box.w = tileRes*MapWidth;
    box.h = tileRes*MapHeight;

    switch (environment) {
        case EnvironmentID::ENV_Cave:   // Cave / Ravine
            SDL_SetRenderDrawColor(canvas, 120, 100, 80, 0);
            break;
        case EnvironmentID::ENV_Desert:   // Desert
            SDL_SetRenderDrawColor(canvas, 210, 200, 80, 0);
            break;
        case EnvironmentID::ENV_Pyramid:   // Pyramid
            SDL_SetRenderDrawColor(canvas, 100, 80, 65, 0);
            break;
        case EnvironmentID::ENV_Mountain:   // Mountain
            SDL_SetRenderDrawColor(canvas, 20, 20, 20, 0);
            break;
        case EnvironmentID::ENV_Snowy:   // Snowy
            SDL_SetRenderDrawColor(canvas, 200, 210, 220, 0);
            break;
        case EnvironmentID::ENV_Forest:
        default:    // Default / Forest
            SDL_SetRenderDrawColor(canvas, 10, 60, 20, 0);
    }
    SDL_RenderFillRect(canvas, &box);
    return;
}
void RenderWindow::DrawTile(int tileX, int tileY, int tileID, int tilingIndex) {
    if (tileID == TileID::ET_None || (tileID >= TileID::ET_Trigger1 && tileID <= TileID::ET_Trigger4) || tileID == TileID::ET_Fizzler) return;

    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = tileX*tileRes + offsetX;
    tile.y = tileY*tileRes + offsetY;
    
    float z = sqrt(pow(tileX,2) + pow(tileY,2));

    bool requiresSource = (tileID == TileID::ET_Tree || tileID == TileID::ET_Vines);
    bool defaultDraw = true;
    
    switch (tileID) {
        case TileID::ET_Wall:
            if ((tileX % 2 == 1 && tileY % 2 == 0) || (tileX % 2 == 0 && tileY % 2 == 1))
                SDL_SetRenderDrawColor(canvas, 215, 215, 215, 0);
            else
                SDL_SetRenderDrawColor(canvas, 235, 235, 235, 0);
            break;
        case TileID::ET_Water:
            SDL_SetRenderDrawColor(canvas, 0, 30, 200 + (int) (sin(time + z) * 20), 0);
            break;
        case TileID::ET_Magma:
            SDL_SetRenderDrawColor(canvas, 220 + (int) (sin(time/2 + z) * 30), 80 + (int) (sin(time/3 + z) * 20), 20, 0);
            break;
        case TileID::ET_Rock:
            SDL_RenderCopy(canvas, TEXTURE_rock, NULL, &tile);
            defaultDraw = false;
            break;
        case TileID::ET_Pillar:
            SDL_RenderCopy(canvas, TEXTURE_pillar, NULL, &tile);
            defaultDraw = false;
            break;
        case TileID::ET_Empty_Puzzle_Piece:
            SDL_SetRenderDrawColor(canvas, 90, 90, 90, 0);
            break;
        case TileID::ET_Pressure_Plate:
            tile.w = 4*tileRes/5;
            tile.h = tile.w;
            tile.x+=tileRes/10;
            tile.y+=tileRes/10;
            SDL_SetRenderDrawColor(canvas, 130, 130, 130, 0);
            break;
        case TileID::ET_Fake_Wall:
            if ((tileX % 2 == 1 && tileY % 2 == 0) || (tileX % 2 == 0 && tileY % 2 == 1))
                SDL_SetRenderDrawColor(canvas, 205, 205, 205, 0);
            else
                SDL_SetRenderDrawColor(canvas, 225, 225, 225, 0);
            break;
        case TileID::ET_Empty_Tile:
            SDL_SetRenderDrawColor(canvas, 0, 0, 0, 0);
            break;
        case TileID::ET_Door_Vertical:
            SDL_SetRenderDrawColor(canvas, 100, 60, 50, 0);
            tile.x += tileRes*.1;
            tile.w *= .8;
            break;
        case TileID::ET_Door_Horizontal:
            SDL_SetRenderDrawColor(canvas, 100, 60, 50, 0);
            tile.y += tileRes*.1;
            tile.h *= .8;
            break;
        case TileID::ET_Ice:
            SDL_SetRenderDrawColor(canvas, 220, 225, 240, 0);
            break;
        
        case TileID::ET_Scroll:
            SDL_SetRenderDrawColor(canvas, 230, 220, 0, 0);
            tile.x += tileRes*.15;
            tile.w *= 0.7;
            break;
        case TileID::ET_Indicator:
            SDL_RenderCopy(canvas, TEXTURE_sunTile, NULL, &tile);
            defaultDraw = false;
            break;
        default:
            defaultDraw = false;
            break;
    }
    if (!requiresSource && defaultDraw) SDL_RenderFillRect(canvas, &tile);

    if (requiresSource) {
        SDL_Rect src;
        src.w = 32;
        src.h = 32;
        src.x = 0;
        src.y = 0;

        if (tilingIndex > 0) { // Tileables
            switch (tilingIndex) {
                case 1: src.x = 32; break;
                case 14: src.x = 64; break;
                case 12: src.x = 96; break;
                case 15: src.x = 128; break;

                case 8: src.y = 32; break;
                case 6: src.y = 32; src.x = 32; break;
                case 9: src.y = 32; src.x = 64; break;
                case 13: src.y = 32; src.x = 96; break;

                case 4: src.y = 64; break;
                case 5: src.y = 64; src.x = 32; break;
                case 7: src.y = 64; src.x = 64; break;
                case 3: src.y = 64; src.x = 96; break;

                case 10: src.y = 96; break;
                case 2: src.y = 96; src.x = 32; break;
                case 11: src.y = 96; src.x = 64; break;
            }
        }

        switch (tileID) {     //Tree
            case TileID::ET_Tree:
                src.x = ((tileX + tileY)%3) * 32;
                SDL_RenderCopyEx(canvas, TEXTURE_tree, &src, &tile, 0, NULL, SDL_FLIP_NONE);
                break;
            case TileID::ET_Vines:
                SDL_RenderCopyEx(canvas, TEXTURE_vines, &src, &tile, 0, NULL, SDL_FLIP_NONE);
            default:
                break;
        }
    }
}
void RenderWindow::DrawEntity(Entity* entity, bool tickedThisFrame, float delta) {
    int lastX = entity->lastX;
    int lastY = entity->lastY;
    int posX = entity->x;
    int posY = entity->y;
    
    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    if (lastX != posX) tile.x = lerp(lastX * tileRes, posX * tileRes, tickAlpha) + offsetX;
    else tile.x = posX * tileRes + offsetX;
    
    if (lastY != posY) tile.y = lerp(lastY * tileRes, posY * tileRes, tickAlpha) + offsetY;
    else tile.y = posY * tileRes + offsetY;

    SDL_Texture* texturePtr = nullptr;

    // TODO: Convert this to a Map
    switch (entity->GetID()) {
        case EntityID::EE_Shepherd: texturePtr = TEXTURE_shepherd; break;
        case EntityID::EE_Sheep: texturePtr = TEXTURE_sheep; break;
        case EntityID::EE_Wolf: texturePtr = TEXTURE_wolf; break;
        case EntityID::EE_Crate: texturePtr = TEXTURE_crate; break;
        case EntityID::EE_Torch: texturePtr = TEXTURE_torch; break;
        case EntityID::EE_Lever: texturePtr = TEXTURE_lever; break;

        case EntityID::EE_PyramidGolem: texturePtr = TEXTURE_boss_pyramidgolem; break;

        default: // Other entities do not have a texture
            break;
    }

    if (tickedThisFrame) entity->animationTimerTicks++;
    entity->Draw(canvas, texturePtr, &tile, delta);
}
void RenderWindow::DrawParticle(float posX, float posY, int id, float percentage) {
    SDL_Rect base;
    base.w = tileRes;
    base.h = tileRes;

    if (id == ParticleID::EP_Swing) {              // Swing
        SDL_SetRenderDrawColor(canvas, 255, 255, 255, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        base.w = (int) (((float) tileRes*3) * (1 - powf(percentage, 3)));
        base.h = base.w;
    } else if (id == ParticleID::EP_PuzzleSolution) {       // Puzzle Solved Click
        SDL_SetRenderDrawColor(canvas, 190, 255, 200, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        base.w = tileRes*3*sqrt(percentage);
        base.h = base.w;
    } else if (id == ParticleID::EP_PressurePlateClick) {       // Pressure Plate Click
        SDL_SetRenderDrawColor(canvas, 255, 255, 100, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        base.w = tileRes*1.5*percentage;
        base.h = base.w;
    } else if (id == ParticleID::EP_Fire) {       // Fire
        SDL_SetRenderDrawColor(canvas, 220 + (int) (sin(time/2) * 30), 80 + (int) (sin(time/3) * 20), 20, 200);
        base.w/=3;
        base.h = base.w;
    } else if (id == ParticleID::EP_FireBurst) {       // Fire Burst
        SDL_SetRenderDrawColor(canvas, 220 + (int) (sin(time/2) * 30), 80 + (int) (sin(time/3) * 20), 20, (int) 200*(1.0f-percentage));
        base.w = (int) tileRes*2.75*(sin(PI*percentage)+1.0)/2;
        base.h = base.w;
    } else if (id == ParticleID::EP_Incinerate) {
        SDL_SetRenderDrawColor(canvas, 80, 80, 40, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        base.w = (int) tileRes*((sin(PI*percentage) + 1.0)/2);
        base.h = base.w;
        base.y -= tileRes*((sin(PI*percentage) + 1.0)/2);
    } else if (id == ParticleID::EP_Spirit) {
        SDL_SetRenderDrawColor(canvas, 100, 150, 255, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        base.w = (int) tileRes * percentage;
        base.h = base.w * 2;
    } else if (id == ParticleID::EP_GolemSmash) {
        SDL_SetRenderDrawColor(canvas, 255, 255, 255, (int) SDL_ALPHA_OPAQUE*(1.0f-percentage));
        if (percentage > 0.63) percentage = powf(percentage, 3); // Expand outward
        else percentage = 0.5 - powf(percentage, 3); // Shrink
        base.w = (int) (((float) tileRes*(MapWidth/2)) * percentage);
        base.h = base.w;
    }

    base.x = (int) ((posX + 0.5)*tileRes - base.w/2 + offsetX);
    base.y = (int) ((posY + 0.5)*tileRes - base.h/2 + offsetY);

    SDL_RenderFillRect(canvas, &base);
}




// Draws a given letter at the given position with the given size
void RenderWindow::DrawLetter(int xPos, int yPos, int sizeX, int sizeY, char letter) {
    if (letter == ' ') return;

    SDL_Rect let;
    let.w = sizeX;
    let.h = sizeY;
    let.x = xPos;
    let.y = yPos;

    int sx = LetterMap[letter];
    int sy = 0;

    // Constrain source X to be within 0-25; adds to source Y for each 26-character row
    for (; sx >= 26; sx-=26)
        sy++;

    SDL_Rect sample;
    sample.w = 5;
    sample.h = 8;
    sample.x = 5*sx;
    sample.y = 8*sy;

    switch (letter) { // Letters that need to be shifted down hlaf a line
        case 'g':
        case 'j':
        case 'p':
        case 'q':
        case 'y':
            let.y+=sizeY/2;
    }

    SDL_RenderCopy(canvas, TEXTURE_alphabet, &sample, &let);
}
/*Writes the given text to fit the set boundaries

Returns the index of whatever letter it was cut off on (the next letter that would not fit in the box)

Start and end describe what sections of the string to scan, defaults to 0 and -2
Set 'end' to -2 for no end, -1 for auto cut-off */
int RenderWindow::WriteText(int leftX, int topY, int rightX, int bottomY, char* text, int start, int end) {
    int sizeY = bottomY-topY;
    int sizeX = (int) sizeY * (5.0f/8.0f);

    int currentIndex = start;
    int lastWordEnd = start;
    int leftXSim = leftX;
    // First, estimate how much space the text will take up so we don't end up splitting up a new word
    while (text[currentIndex] && ((end == -1 || currentIndex < end) && leftXSim <= rightX-sizeX) || end <= -2) {
        currentIndex++;

        char l = text[currentIndex];
        if (l == '\n' || l == '\0') { //If it's a new line, move on (automatically gets pushed on)
            lastWordEnd = currentIndex-1;
            break;
        } else if (l == ' ') //If it's a space or terminating character, count it as the end of a word
            lastWordEnd = currentIndex-1;

        leftXSim+=sizeX + sizeX*LetterSpacing;
    }


    currentIndex = start;   // Reset index and actually draw the text (same method)
    if (text[currentIndex] == ' ')
        currentIndex++;
    while (text[currentIndex] && (((end == -1 || currentIndex < end) && leftX <= rightX-sizeX && currentIndex <= lastWordEnd) || end <= -2)) {
        if (text[currentIndex] == '\n' || text[currentIndex] == '\0') { //If it's a new line, move on (automatically gets pushed on)
            currentIndex++;
            break;
        }

        DrawLetter(leftX, topY, sizeX, sizeY, text[currentIndex]);

        currentIndex++;
        leftX+=sizeX + sizeX*LetterSpacing;
    }
    
    return currentIndex;
}


/*Displays given text within the dialogue box

Set the amount of ticks the dialogue remains in the box, defaults to 50 (5 seconds with a tick rate of 10)
Set 'ticks' to 0 for the text to last indefinitely or until overwritten*/
void RenderWindow::SetDialogueText(const char* text, int ticks) {
    if (dialogueText != NULL)
        free(dialogueText);
    dialogueText = strdup(text);
    dialogueTicksLeft = ticks;
}
// Returns whatever is currently supposed to be inside the dialogue box (exclused custom text)
char* RenderWindow::GetDialogueText() {
    return dialogueText;
}
// Draws the dialogue box and fills it with the given text
// Text defaults to a nullptrs, at which point it will draw the given dialogue text instead
void RenderWindow::DrawDialogueBox(char* text) {
    // Top pixel of dialogue box
    int dboxtop = innerHeight + offsetY;
    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    SDL_RenderDrawLine(canvas,0,dboxtop,x,innerHeight+offsetY);
    
    if (text == nullptr) text = dialogueText;

    // Only render text if it exists
    if (text && text[0]) {
        // Add in half a line-space to center text
        dboxtop+=dialogueBoxY*DialogueBoxLineSpacing/2;

        int shave = 0;
        for (int i = 0; i < DialogueBoxLines && text[shave]; i++) {
            int top = dboxtop + i*dialogueBoxLineHeight + i*dialogueBoxY*DialogueBoxLineSpacing;
            shave = WriteText(0, top, x, top + dialogueBoxLineHeight, text, shave, -1);
        }

        // Remove the offset to keep frame bottom consistent
        dboxtop-=dialogueBoxY*DialogueBoxLineSpacing/2;
    }

    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    SDL_RenderDrawLine(canvas,0,dboxtop+dialogueBoxY,x,dboxtop+dialogueBoxY);
}


// Adds screen shake, impact is the more intense horizontal shaking, while rumble is slight vertical movement
// * Impact is good for projectiles and collapses to add chaos and intensity
// * Rumble is good for light puzzle feedback to grab the player's attention when a door opens or path appears (makes effect more satisfying)
void RenderWindow::AddScreenShake(float impact, float rumble) {
    screenShakeX += impact;
    screenShakeY += rumble;
}
void RenderWindow::ToggleContinuousRumble(bool enabled) {
    continuousRumble = enabled;
}

// Draws the status bar, informing player of their health and whether or not all the puzzles in the current level have been completed or not
void RenderWindow::DrawStatusBar(int HP, bool PuzzleCompleted) {
    if (!statusBarVisible) return;

    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    int top = offsetY-statusBarY;
    SDL_RenderDrawLine(canvas,0,top,x,top);
    SDL_RenderDrawLine(canvas,0,offsetY,x,offsetY);

    int pulse = (int) 20 * abs(sin(PI*time));
    SDL_SetRenderDrawColor(canvas, 50 + pulse, 180 + pulse, 60 + pulse, 0);
    SDL_Rect healthRect;
    healthRect.w = (int) statusBarY * 3 * (float) (HP / 3.0f);
    healthRect.h = statusBarY;
    healthRect.x = 0;
    healthRect.y = top;
    SDL_RenderFillRect(canvas, &healthRect);

    if (PuzzleCompleted) {
        SDL_SetRenderDrawColor(canvas, 50, 180, 60, 0);
        SDL_Rect completionRect;
        completionRect.w = statusBarY;
        completionRect.h = statusBarY;
        completionRect.x = x-statusBarY;
        completionRect.y = top;
        SDL_RenderFillRect(canvas, &completionRect);
    }
}
// Enables or disables the Status Bar depending on input
void RenderWindow::ToggleStatusBar(bool toggle) {
    statusBarVisible = toggle;
    UpdateSize();
}





// Draws a blank menu background
void RenderWindow::DrawMenuBackground() {
    SDL_SetRenderDrawColor(canvas, 20, 20, 20, 0);
    
    SDL_Rect back;
    back.w = tileRes * MapWidth; 
    back.h = tileRes * MapHeight;
    back.x = offsetX;
    back.y = offsetY;

    SDL_RenderFillRect(canvas, &back);
}
// Draws a menu and highlights selected item
void RenderWindow::DrawMenu(int menuSize, char** menuText, int itemSelected, bool shrunk) {
    SDL_Rect back;
    back.w = tileRes * MapWidth; 
    back.h = tileRes * MapHeight;
    back.x = offsetX;
    back.y = offsetY;

    int left = back.w * 0.25 + back.x;
    int right = back.w * 0.75;

    int yScale = min(back.h / (menuSize + 1), back.h / 5);
    int yScaleInbetween = min((back.h - yScale * menuSize) / (menuSize + 1), yScale / 2);

    if (shrunk) { // Shifts menu to top left corner
        left *= 0.2;
        right *= 0.2;
        yScale *= 0.2;
        yScaleInbetween *= 0.2;
    }

    int posY = back.y + yScaleInbetween;
    for (int i = 0; i < menuSize; i++) {
        if (i == itemSelected) { // Draw the item with a little scale boost if it is currently selected
            WriteText(left * 0.9, posY - yScaleInbetween / 2, right * 1.1, posY + yScale + yScaleInbetween / 2, menuText[i]);
        } else
            WriteText(left, posY, right, posY + yScale, menuText[i]);
        
        posY += yScale + yScaleInbetween;
    }
}
// Draws a pop-up that covers the whole screen if something is expected to take a while to process
void RenderWindow::LoadScreen() {
    SDL_RenderCopy(canvas, TEXTURE_PROGRAM_loadscreen, NULL, NULL);
    SDL_RenderPresent(canvas);
}