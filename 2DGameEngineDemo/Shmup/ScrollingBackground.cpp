#include "ScrollingBackground.h"
#include "Entity.h"

void ScrollingBackground::init(float _tileHeight, float _playAreaHeight) {
	transformComp = getParent()->getComponent<TransformComponent>();
	tileHeight = _tileHeight;
	playAreaHeight = _playAreaHeight;
	// Fraction of the play area's height scrolled per second. Was 0.047
	// (a ~27s loop) - bumped up so the terrain visibly advances at a pace
	// that matches the rest of the game's speed instead of crawling by.
	scrollSpeed = playAreaHeight * 0.09f;
}

void ScrollingBackground::update(float _deltaTime) {
	sf::Vec2f position = transformComp->getPosition();
	position.y += scrollSpeed * _deltaTime;

	// Once this tile has scrolled a full tileHeight past the bottom, its
	// partner tile (offset by exactly tileHeight) has taken its place at the
	// top - jump back above the visible area to keep the pair looping.
	if (position.y >= playAreaHeight + tileHeight / 2.f) {
		position.y -= 2.f * tileHeight;
	}

	transformComp->setPosition(position);
}
