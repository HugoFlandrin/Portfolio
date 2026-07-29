#pragma once
#include <map>
#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

class ResourceManager
{
	std::map<std::string, sf::Texture*> textureCache;
	std::map<std::string, sf::Font*> fontCache;
	static ResourceManager* m_instance;

public:
	sf::Texture* loadTexture(std::string _path);
	void unloadTexture(std::string _path);

	sf::Font* loadFont(std::string _path);
	void unloadFont(std::string _path);

	static ResourceManager* instance();
};
