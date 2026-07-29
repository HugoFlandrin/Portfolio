#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "AComponent.h"

class Render : public AComponent
{

	sf::Sprite sprite;
	sf::Texture* texture;

public :

	Render(sf::Texture& _texture, sf::Rect2i _rect, sf::Vec2f _origin = sf::Vec2f(32.f, 32.f));

	sf::Sprite& getSprite();
	void setTexture(sf::Texture& _texture);
	void setFlipX(bool _flip);
	void setFlipY(bool _flip);
	virtual void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;
};
