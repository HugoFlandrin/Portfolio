#pragma once
#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Utf8String.hpp>
#include <SFML/Window/Event.hpp>
#include "SceneManager.h"
#include "ResourceManager.h"
#include "PhysicSystem.h"

class Engine
{
    static Engine* m_instance;

    // Read once, by the constructor, when the singleton is first created -
    // see configureWindow(). Defaulted to the platformer's original window,
    // so it needs no changes to keep working exactly as before.
    static sf::Vec2u s_windowSize;
    static sf::Utf8String s_windowTitle;

    sf::GraphicsContext graphicsContext;
    sf::RenderWindow window;

    PhysicSystem* physics;
    SceneManager* sceneManager;

    sf::Clock clock;
    bool running = true;

public:
    Engine();
    void run();
    void tick();
    sf::RenderWindow* getWindow();
    static Engine* instance();
    // Must be called before the first Engine::instance() call (which is
    // when the singleton - and its OS window - actually gets constructed);
    // calling it afterwards has no effect. Lets each game (see Shmup.cpp)
    // pick its own window size/title instead of the platformer's default.
    static void configureWindow(sf::Vec2u _size, sf::Utf8String _title);
    void init();
    PhysicSystem* getPhysicSystem();

private:
    void events();
    void update(float _deltaTime);
    void render();
};
