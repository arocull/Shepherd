#include "Entities/Subclasses/torch.h"

Torch::Torch(int xPos, int yPos) {
    id = 6;

    x = xPos;
    y = yPos;

    Solid = true;
    HasFire = true;
    UpdateAnimationData();
}

void Torch::UpdateAnimationData() {
    if (HasFire)
        animationMetadata = 1;
    else if (HasFrost)
        animationMetadata = 2;
    else
        animationMetadata = 0;
    
    if (glow)
        animation = 1;
    else
        animation = 0;
}

void Torch::Extinguish(bool Override) {
    if (Extinguishable || Override) {
        HasFire = false;
        HasFrost = false;
    }
}