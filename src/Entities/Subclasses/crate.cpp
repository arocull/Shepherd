#include "Entities/Subclasses/crate.h"

Crate::Crate(int xPos, int yPos) {
    id = EntityID::EE_Crate;

    x = xPos;
    y = yPos;
}

void Crate::Incinerate(Particle* particles, Entity* attacker) {
    if (TakeDamage(1, attacker)) {
        Particle* burnEffect = ActivateParticle(particles, ParticleID::EP_Incinerate, x, y);
        if (burnEffect) {
            burnEffect->maxLifetime = 1.0f;
        }
    }
}


std::string* Crate::Ascii() {
    std::string* text = Entity::Ascii();
    strutil::appendChar(text, ',');
    strutil::poolIntegers(text, 2, type, canIncinerate);
    return text;
}
void Crate::LoadAscii(char* str, int* index) {
    Entity::LoadAscii(str, index);
    type = strutil::parseInt(str, index);
    canIncinerate = strutil::parseBool(str, index);
}