#include "shepherd.h"

Shepherd::Shepherd(int spawnX, int spawnY) {
    id = EntityID::EE_Shepherd;

    MaxHealth = ShepherdHealth;
    Health = MaxHealth;

    x = spawnX;
    y = spawnY;
    lastX = spawnX;
    lastY = spawnY;
}


void Shepherd::Tick() {
    if (HasFire)
        animationMetadata = 1;
    else if (HasFrost)
        animationMetadata = 2;
    else
        animationMetadata = 0;
}


void Shepherd::SlingFireball(Entity** entities, Particle* particles, SoundService* soundService) {
    HasFire = false;
    animation = 2; // Fireball Toss animation
    
    Fireball* fireball = new Fireball(x, y, faceX, faceY, 0);
    fireball->enemy = false;
    AppendEntity(entities, fireball);
    soundService->PlaySound("Audio/Resources/FireballSling.wav");
}
void Shepherd::SwingAttack(Entity** entities, Particle* particles, SoundService* soundService) {
    animation = 2;
    ActivateParticle(particles, 1, x, y);

    int sheepFound = 0;
    int pausedSheep = 0;
    Entity* fireballPresent = nullptr;
    bool unlitTorchPresent = false;
    bool litTorchPresent = false;
    bool leverToFlip = false;

    // Tally sheep; find out how many there are and what proportion of them is paused
    for (int w = x-1; w < x+2; w++) {
        for (int z = y-1; z < y+2; z++) {
            Entity* obj = GetEntityAtLocation(entities, w, z);
            if (obj) {
                switch (obj->GetID()) {
                    case EntityID::EE_Sheep: // Tally sheep
                        sheepFound++;
                        if (obj->Paused) pausedSheep++;
                        break;
                    case EntityID::EE_Fireball: // If no fireball has been detected yet, select nearest one
                        if (!fireballPresent) fireballPresent = obj;
                        break;
                    case EntityID::EE_Wolf:
                        obj->TakeDamage(1, this);
                }
                if (obj->GetID() == EntityID::EE_Torch) {
                    Torch* torch = dynamic_cast<Torch*>(obj);
                    if (torch->Extinguishable && !obj->HasFire && !obj->HasFrost) unlitTorchPresent = true;
                    else if (torch->FireUsable && (obj->HasFire || obj->HasFrost)) litTorchPresent = true;
                } else if (obj->GetID() == EntityID::EE_Lever) {
                    Lever* lever = dynamic_cast<Lever*>(obj);
                    if (!lever->IsLocked()) leverToFlip = true;
                }
            }
        }
    }


    bool hasFlame = HasFire || HasFrost;

    /* Do tasks in order of priority
    * If player is holding fire, fire-related tasks take priority
    * 
    * Wolves are always damaged during checks before other functionality is performed
    * 
    * 1 - Toss fireball if staff has fire
    * 2 - If the player is not holding a flame and a fireball is nearby, destroy the first fireball and take its flame
    * 3 - If a lit torch is nearby and the player does not have fire, take fire from the torch (and spread fire if the player has it now)
    * 4 - If an unlit torch is nearby and the player has fire, transfer the fire to that instead
    * 5 - If the player is standing next to an unlocked lever, flip it
    * 6 - Otherwise, rally sheep
    */

    // If the Shepherd has fire or frost and no unlit torches are present, sling a fireball
    if (hasFlame && !unlitTorchPresent) {
        SlingFireball(entities, particles, soundService);

    // If we're not holding any fire and there is a fireball present, catch it
    } else if (!hasFlame && fireballPresent) {
        HasFire = fireballPresent->HasFire;
        HasFrost = fireballPresent->HasFrost;
        fireballPresent->TakeDamage(1, this); // Fireballs with no health are marked for cleanup

    // If we're not holding fire and there is fire we can pick up
    } else if (!hasFlame && litTorchPresent) {
        //printf("doin' the thing\n");
        for (int w = x-1; w < x+2; w++) {
            for (int z = y-1; z < y+2; z++) {
                Entity* obj = GetEntityAtLocation(entities, w, z);
                // Check if it's a torch--don't bother further processing though if we already have a flame unless the torch can override our flame
                if (obj && obj->GetID() == EntityID::EE_Torch && (!hasFlame || (HasFrost && obj->HasFire))) {
                    Torch* t = dynamic_cast<Torch*>(obj);
                    if (t) { // Only grab fire if the fire is useable, the torch has a flame
                    //printf("Considering torch\n");
                        if (t->FireUsable && (obj->HasFire || obj->HasFrost)) {
                            HasFire = obj->HasFire;
                            HasFrost = obj->HasFrost;
                            t->Extinguish();
                            //printf("GRABBING FIRE\n");
                        }
                    }
                }
            }
        }

    // If there is any nearby torches we can light, light all of them
    } else if (hasFlame && unlitTorchPresent) {
        bool torchLit = false;
        for (int w = x-1; w < x+2; w++) {
            for (int z = y-1; z < y+2; z++) {
                Entity* obj = GetEntityAtLocation(entities, w, z);
                if (obj && obj->GetID() == EntityID::EE_Torch && !obj->HasFire && !obj->HasFrost) {
                    Torch* t = dynamic_cast<Torch*>(obj);
                    if (t && t->Extinguishable) {
                        obj->HasFire = HasFire;
                        obj->HasFrost = HasFrost;
                        torchLit = true;
                    }
                }
            }
        }
        if (torchLit) { // If any torches were lit, clear staff flame
            HasFire = false;
            HasFrost = false;
        }

    } else if (leverToFlip) { // Flip levers without interfering with sheep
        for (int w = x-1; w < x+2; w++) {
            for (int z = y-1; z < y+2; z++) {
                Entity* obj = GetEntityAtLocation(entities, w, z);
                if (obj && obj->GetID() == EntityID::EE_Lever) {
                    Lever* lever = dynamic_cast<Lever*>(obj);
                    if (lever) {
                        lever->Flip();
                    }
                }
            }
        }

    } else {
        // Defaultly pause all sheep encountered
        bool NewPause = true;
        if (pausedSheep > sheepFound/2) // ...unless there more paused sheep already than unpaused within the sample 
            NewPause = false;

        // Pause any sheep and attack any wolves within a tile radius of the Shepherd
        for (int w = x-1; w < x+2; w++) {
            for (int z = y-1; z < y+2; z++) {
                Entity* obj = GetEntityAtLocation(entities, w, z);
                if (obj && obj->GetID() == EntityID::EE_Sheep) { //If sheep, toggle pause
                    obj->Paused = NewPause;
                    obj->animation = 1;
                }
            }
        }
    }
}