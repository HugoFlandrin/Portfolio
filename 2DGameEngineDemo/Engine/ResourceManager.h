#pragma once
#include <map>
#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class ResourceManager
{
	std::map<std::string, sf::Texture*> textureCache;
	std::map<std::string, sf::Font*> fontCache;
	std::map<std::string, sf::SoundBuffer*> soundCache;
	static ResourceManager* m_instance;

public:
	sf::Texture* loadTexture(std::string _path);
	void unloadTexture(std::string _path);

	sf::Font* loadFont(std::string _path);
	void unloadFont(std::string _path);

	sf::SoundBuffer* loadSound(std::string _path);
	void unloadSound(std::string _path);

	// Desktop loads assets from next to the executable (copied there by the
	// build's post-build step); the web build preloads Game/Assets into the
	// Emscripten virtual filesystem at a fixed mount point instead. Exposed
	// (not just used internally by loadTexture/loadFont/loadSound) so other
	// engine code that streams its own asset type - e.g. AudioManager's
	// background Music, which isn't cached here - resolves paths the same way.
	static std::string resolveAssetPath(std::string _path);

	static ResourceManager* instance();
};
