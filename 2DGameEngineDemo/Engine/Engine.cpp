#include "Engine.h"
#include "SceneManager.h"
#include "TouchInput.h"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "WebBridge.h"
#endif

Engine* Engine::m_instance = nullptr;
sf::Vec2u Engine::s_windowSize = { 1920u, 1080u };
sf::Utf8String Engine::s_windowTitle = "Platformer";

Engine* Engine::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Engine();
    }
    return m_instance;
}

void Engine::configureWindow(sf::Vec2u _size, sf::Utf8String _title) {
    s_windowSize = _size;
    s_windowTitle = _title;
}

Engine::Engine() :
    graphicsContext(sf::GraphicsContext::create().value()),
    window(sf::RenderWindow::create({ .size = s_windowSize, .title = s_windowTitle }).value())
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

        // Generic touch polling state, mirroring sf::Keyboard::isKeyPressed -
        // harmless on platforms/games that never read TouchInput (nothing in
        // the platformer does).
        if (const auto* touchBegan = event->getIf<sf::Event::TouchBegan>()) {
            TouchInput::instance()->setDown(true, touchBegan->position.toVec2f());
        }
        else if (const auto* touchMoved = event->getIf<sf::Event::TouchMoved>()) {
            TouchInput::instance()->setPosition(touchMoved->position.toVec2f());
        }
        else if (const auto* touchEnded = event->getIf<sf::Event::TouchEnded>()) {
            TouchInput::instance()->setDown(false, touchEnded->position.toVec2f());
        }
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
