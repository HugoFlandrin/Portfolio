#include "TransformComponent.h"

void TransformComponent::update(float _deltaTime) {
	
}

void TransformComponent::init(sf::Vec2f _position, sf::Vec2f _scale, sf::Angle _rotation)
{
	setPosition(_position);
	setScale(_scale);
	setRotation(_rotation);
}

