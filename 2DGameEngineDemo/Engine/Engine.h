#pragma once
#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "SceneManager.h"
#include "ResourceManager.h"
#include "PhysicSystem.h"

class Engine
{
    static Engine* m_instance;

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
    void init();
    PhysicSystem* getPhysicSystem();

private:
    void events();
    void update(float _deltaTime);
    void render();
};
