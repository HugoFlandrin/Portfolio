#include "AComponent.h"
#include "Entity.h"

Entity* AComponent::getParent() const {
    return parent;
}

void AComponent::draw(sf::RenderTarget& _target, sf::RenderStates _states) const
{
}