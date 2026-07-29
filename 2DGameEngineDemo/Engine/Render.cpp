#include "Render.h"
#include "TransformComponent.h"
#include "Entity.h"
#include <cmath>

Render::Render(sf::Texture& _texture, sf::Rect2i _rect, sf::Vec2f _origin) : sprite{}, texture(&_texture) {
	sprite.textureRect = _rect.toRect2f();
	sprite.origin = _origin;
}

void Render::update(float _deltaTime) {

}

sf::Sprite& Render::getSprite() {
	return sprite;
}

void Render::setTexture(sf::Texture& _texture) {
	texture = &_texture;
}

void Render::setFlipX(bool _flip) {
	sf::Vec2f scale = sprite.scale;
	sprite.scale = { _flip ? -std::abs(scale.x) : std::abs(scale.x), scale.y };
}

void Render::setFlipY(bool _flip) {
	sf::Vec2f scale = sprite.scale;
	sprite.scale = { scale.x, _flip ? -std::abs(scale.y) : std::abs(scale.y) };
}

void Render::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_states.transform *= getParent()->getComponent<TransformComponent>()->getTransform();
	_states.texture = texture;
	_target.draw(sprite, _states);
}
