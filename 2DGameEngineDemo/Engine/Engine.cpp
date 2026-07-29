#include "Engine.h"
#include "SceneManager.h"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "WebBridge.h"
#endif

Engine* Engine::m_instance = nullptr;

Engine* Engine::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Engine();
    }
    return m_instance;
}

Engine::Engine() :
    graphicsContext(sf::GraphicsContext::create().value()),
    window(sf::RenderWindow::create({ .size = { 1920u, 1080u }, .title = "Platformer" }).value())
{
    init();
}

void Engine::init() {
    physics = new PhysicSystem();
    sceneManager = SceneManager::instance();

#ifndef __EMSCRIPTEN__
    // On the web the render loop is already paced by the browser's
    // requestAnimationFrame (locked to the display's refresh rate), and
    // SFML's frame limiter uses a blocking sleep internally that requires
    // Emscripten's Asyncify to work - skip it here instead of paying that
    // cost for something rAF already gives us for free.
    window.setFramerateLimit(60);
#endif
}

void Engine::run()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg([](void* arg) { static_cast<Engine*>(arg)->tick(); }, this, 0, 0);
#else
    while (running)
    {
        tick();
    }
#endif
}

void Engine::tick()
{
    float deltaTime = clock.restart().asSeconds();

    events();
    physics->update(deltaTime);
    update(deltaTime);
    render();
    sceneManager->processChangeScene();

#ifdef __EMSCRIPTEN__
    if (!running)
        emscripten_cancel_main_loop();
#endif
}

sf::RenderWindow* Engine::getWindow() {
    return &window;
}

PhysicSystem* Engine::getPhysicSystem() {
    return physics;
}

void Engine::events()
{
    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            running = false;

#ifdef __EMSCRIPTEN__
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scan::Escape)
                WebBridge::requestQuit();
        }
#endif
    }
}

void Engine::update(float _deltaTime)
{
    sceneManager->getCurrentScene()->update(_deltaTime);
}

void Engine::render()
{
    window.clear(sf::Color::Black);

    AScene* scene = sceneManager->getCurrentScene();

    if (scene) {
        sf::RenderStates states;
        states.view = *scene->getCamera();
        window.draw(*scene, states);
    }

    window.display();
}
