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

        TEXTURESURFACE_tree = SDL_LoadBMP("Textures/Tree.bmp");
        if (TEXTURESURFACE_tree) {
            SDL_SetWindowIcon(window, TEXTURESURFACE_tree);
            TEXTURE_tree = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_tree);
        }

        TEXTURESURFACE_sheep = SDL_LoadBMP("Textures/Sheep.bmp");
        if (TEXTURESURFACE_sheep) {
            TEXTURE_sheep = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_sheep);
        }

        TEXTURESURFACE_shepherd = SDL_LoadBMP("Textures/Shepherd.bmp");
        if (TEXTURESURFACE_shepherd) {
            TEXTURE_shepherd = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_shepherd);
        }

        TEXTURESURFACE_alphabet = SDL_LoadBMP("Textures/Alphabet.bmp");
        if (TEXTURESURFACE_alphabet) {
            TEXTURE_alphabet = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_alphabet);
        }
    }
}


bool RenderWindow::IsInitialized(){
    return initialized;
}
void RenderWindow::ToggleFullscreen() {
    if (!fullscreen) {
        windowedX = x;
        windowedY = y;
        
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); //Enforce borderless fullscreen without screwing up resolution
        fullscreen = true;
    } else {
        SDL_SetWindowFullscreen(window, 0); //Exit fullscreen
        //SDL_SetWindowBordered(window, SDL_TRUE);
        SDL_SetWindowSize(window, windowedX, windowedY);
        fullscreen = false;
    }
}
void RenderWindow::UpdateSize() {
    SDL_GetWindowSize(window, &x, &y);

    if (dialogueBoxVisible)
        dialogueBoxY = y*DialogueBoxScale;
    else
        dialogueBoxY = 0;

    dialogueBoxLineHeight = dialogueBoxY/DialogueBoxLines - dialogueBoxY*DialogueBoxLineSpacing;

    tileRes = fmin(x/MapWidth, (y - dialogueBoxY)/MapHeight);
    offsetX = (x - MapWidth*tileRes)/2;
    offsetY = (y - MapHeight*tileRes - dialogueBoxY)/2;
    innerWidth = tileRes*MapWidth;
    innerHeight = tileRes*MapHeight;
}
void RenderWindow::TickDeltaTime(float DeltaTime) {
    time+=DeltaTime;
}
void RenderWindow::LogTick() {
    ticks++;

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

    SDL_FreeSurface(TEXTURESURFACE_tree);
    SDL_DestroyTexture(TEXTURE_tree);
    SDL_FreeSurface(TEXTURESURFACE_sheep);
    SDL_DestroyTexture(TEXTURE_sheep);
    SDL_FreeSurface(TEXTURESURFACE_shepherd);
    SDL_DestroyTexture(TEXTURE_shepherd);
    SDL_FreeSurface(TEXTURESURFACE_alphabet);
    SDL_DestroyTexture(TEXTURE_alphabet);

    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
}



void RenderWindow::FillViewportBackground(int r, int g, int b) {
    SDL_Rect box;
    box.x = offsetX;
    box.y = offsetY;
    box.w = tileRes*MapWidth;
    box.h = tileRes*MapHeight;

    SDL_SetRenderDrawColor(canvas, r, g, b, 0);
    SDL_RenderFillRect(canvas, &box);
    return;
}
void RenderWindow::DrawTile(int tileX, int tileY, int tileID) {
    if (tileID == 0) return;

    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = tileX*tileRes + offsetX;
    tile.y = tileY*tileRes + offsetY;

    if (tileID == 1) {      // Wall
        //if ((time - round(time)) <= .5 && )
        if ((tileX % 2 == 1 && tileY % 2 == 0) || (tileX % 2 == 0 && tileY % 2 == 1))
            SDL_SetRenderDrawColor(canvas, 215, 215, 215, 0);
        else
            SDL_SetRenderDrawColor(canvas, 235, 235, 235, 0);
        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 2) {   //Water
        SDL_SetRenderDrawColor(canvas, 0, 30, 200 + (int) (sin(time + (sqrt(pow(tileX,2) + pow(tileY,2)))) * 20), 0);

        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 3) {   //Magma
        float z = sqrt(pow(tileX,2) + pow(tileY,2));
        SDL_SetRenderDrawColor(canvas, 220 + (int) (sin(time/2 + z) * 30), 80 + (int) (sin(time/3 + z) * 20), 20, 0);
        
        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 4) {   //Tree
        SDL_RenderCopy(canvas, TEXTURE_tree, NULL, &tile);
    }
}
void RenderWindow::DrawEntity(int posX, int posY, int id, bool flip, int anim) {
    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = posX*tileRes + offsetX;
    tile.y = posY*tileRes + offsetY;

    SDL_RendererFlip flipStyle = SDL_FLIP_NONE;
    if (flip)
        flipStyle = SDL_FLIP_HORIZONTAL;

    double angle = 0.0;
    if (id == 1) {
        int step = ticks/TickRate % 2;

        switch (anim) {
            case 2:
                tile.x += tile.w/6;
                tile.w *= (float) 2 / (float) 3;
                break;
            default:
                tile.x += tile.w/3;
                tile.w /= 3;
        }

        SDL_Rect src;
        src.h = 32;
        src.y = 0;

        if (anim == 1) {
            src.w = 12;
            src.x = 20;
            step = ticks % 2;
            angle = (((double) step) - 0.5)*5;
        } else if (anim == 2) {
            src.w = 23;
            src.x = 32; 
        } else {
            src.w = 10;
            if (step == 1)
                src.x = 10;
            else
                src.x = 0;
        }
        

        SDL_RenderCopyEx(canvas, TEXTURE_shepherd, &src, &tile, angle, NULL, flipStyle);
    } else if (id == 2) {  // Player or Sheep
        int step = ticks/2 % 4;
        if (step == 2)
            angle = -3.0;
        else if (step == 1 || step == 3)
            angle = 0.0;
        else
            angle = 3.0;

        SDL_RenderCopyEx(canvas, TEXTURE_sheep, NULL, &tile, angle, NULL, flipStyle);
    } else if (id == 3) {   // Fireball
        int sha = (int) 20 * (sin(time*20) + 1);
        SDL_SetRenderDrawColor(canvas, 210 + sha, 100 + sha, 0, 0);

        tile.x += tile.w*.25;
        tile.y += tile.h*.25;
        tile.w *= .5;
        tile.h *= .5;

        SDL_RenderFillRect(canvas, &tile);
    }    
}





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

    switch (letter) {
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

Start and end describe what sections of the string to scan, defaults to 0 and -1
Set 'end' to -1 for no end*/
int RenderWindow::WriteText(int leftX, int topY, int rightX, int bottomY, char* text, int start, int end) {
    int sizeY = bottomY-topY;
    int sizeX = (int) sizeY * (5.0f/8.0f);

    int currentIndex = start;
    while (text[currentIndex] && (end <= -1 || currentIndex < end) && leftX <= rightX-sizeX) {
        if (text[currentIndex] == '\n') { //If it's a new line, move on (automatically gets pushed on)
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
void RenderWindow::SetDialogueText(char* text, int ticks) {
    dialogueText = strdup(text);//strcpy(dialogueText, text);
    dialogueTicksLeft = ticks;
}
char* RenderWindow::GetDialogueText() {
    return dialogueText;
}
void RenderWindow::DrawDialogueBox() {
    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    SDL_RenderDrawLine(canvas,0,offsetY,x,offsetY);

    // Top pixel of dialogue box
    int dboxtop = innerHeight + offsetY;
    SDL_RenderDrawLine(canvas,0,dboxtop,x,innerHeight+offsetY);
    

    // Only render text if it exists
    if (dialogueText && dialogueText[0]) {
        // Add in half a line-space to center text
        dboxtop+=dialogueBoxY*DialogueBoxLineSpacing/2;

        int shave = 0;
        for (int i = 0; i < DialogueBoxLines && dialogueText[shave]; i++) {
            int top = dboxtop + i*dialogueBoxLineHeight + i*dialogueBoxY*DialogueBoxLineSpacing;
            shave = WriteText(0, top, x, top + dialogueBoxLineHeight, dialogueText, shave);
        }

        // Remove the offset to keep frame bottom consistent
        dboxtop-=dialogueBoxY*DialogueBoxLineSpacing/2;
    }

    SDL_RenderDrawLine(canvas,0,dboxtop+dialogueBoxY,x,dboxtop+dialogueBoxY);
}