#include "ResourceManager.h"
#include <SFML/System/Path.hpp>

#ifdef __EMSCRIPTEN__
#else
#include <Windows.h>
#include <filesystem>
#endif

ResourceManager* ResourceManager::m_instance = nullptr;

namespace {
	// Desktop loads assets from next to the executable (copied there by the
	// build's post-build step); the web build preloads Game/Assets into the
	// Emscripten virtual filesystem at a fixed mount point instead.
	std::string resolveAssetPath(const std::string& _path) {
#ifdef __EMSCRIPTEN__
		return "/Assets/" + _path;
#else
		TCHAR buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, sizeof(buffer));
		std::string absolutePath = std::filesystem::path(buffer).parent_path().string();
		return absolutePath + "\\" + _path;
#endif
	}
}

sf::Texture* ResourceManager::loadTexture(std::string _path) {

	if (!textureCache.contains(_path)) {
		textureCache.insert({ _path, new sf::Texture(sf::Texture::loadFromFile(resolveAssetPath(_path)).value()) });
	}

	return textureCache[_path];
}

void ResourceManager::unloadTexture(std::string _path) {
	textureCache.erase(_path);
}

sf::Font* ResourceManager::loadFont(std::string _path) {

	if (!fontCache.contains(_path)) {
		fontCache.insert({ _path, new sf::Font(sf::Font::openFromFile(resolveAssetPath(_path)).value()) });
	}

	return fontCache[_path];
}

void ResourceManager::unloadFont(std::string _path) {
	fontCache.erase(_path);
}

ResourceManager* ResourceManager::instance()
{
	if (m_instance == nullptr) {
		m_instance = new ResourceManager();
	}
	return m_instance;
}
