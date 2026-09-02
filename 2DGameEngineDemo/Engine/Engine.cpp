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
#else
        // Generic touch polling state, mirroring sf::Keyboard::isKeyPressed -
        // harmless on platforms/games that never read TouchInput (nothing in
        // the platformer does). Native only - see updateWebTouchInput() for
        // why the web build can't use SFML's touch events for this.
        if (const auto* touchBegan = event->getIf<sf::Event::TouchBegan>()) {
            TouchInput::instance()->setDown(true, touchBegan->position.toVec2f());
        }
        else if (const auto* touchMoved = event->getIf<sf::Event::TouchMoved>()) {
            TouchInput::instance()->setPosition(touchMoved->position.toVec2f());
        }
        else if (const auto* touchEnded = event->getIf<sf::Event::TouchEnded>()) {
            TouchInput::instance()->setDown(false, touchEnded->position.toVec2f());
        }
#endif
    }

#ifdef __EMSCRIPTEN__
    updateWebTouchInput();
#endif
}

#ifdef __EMSCRIPTEN__
void Engine::updateWebTouchInput()
{
    // VRSFML creates a hidden 1x1 probe window to query WebGL capabilities
    // during startup (SDLGlContext.cpp), which briefly attaches its own
    // pointermove/pointerdown JS listeners to the SAME #canvas element our
    // real game window uses, then gets destroyed without ever removing
    // them. Because SDL's per-finger touch state is global (keyed only by
    // finger id, not by window), every real touchmove fires both the dead
    // probe window's listener and our real one; the SFML_EVENT_FINGER_MOTION
    // event that carries the actual delta ends up tagged with the probe
    // window's (now-unregistered) id and gets silently dropped by SFML's
    // window lookup, so sf::Event::TouchMoved never arrives on the web
    // build (isDown from TouchBegan/TouchEnded happens to survive this,
    // moves never do). Bypassing SFML's touch pipeline for continuous
    // position tracking - reading the finger's current position straight
    // from a plain JS listener we own - sidesteps the bug entirely.
    // Flat globals instead of one window.__rawTouch = {...} object literal:
    // EM_ASM's underlying macro is a variadic C preprocessor macro, which
    // only tracks () for argument grouping, not {} - a comma sitting inside
    // braces but outside any parens (like the ones between object-literal
    // properties here) gets misread as a macro-argument separator and
    // splits the JS mid-statement, breaking the build.
    static bool listenerInstalled = false;
    if (!listenerInstalled) {
        listenerInstalled = true;
        EM_ASM({
            window.__rawTouchDown = false;
            window.__rawTouchXNorm = 0;
            window.__rawTouchYNorm = 0;
            var canvas = document.getElementById('canvas');
            var updateFromTouch = function (e) {
                if (e.touches.length > 0) {
                    var t = e.touches[0];
                    var rect = canvas.getBoundingClientRect();
                    window.__rawTouchXNorm = (t.clientX - rect.left) / rect.width;
                    window.__rawTouchYNorm = (t.clientY - rect.top) / rect.height;
                    window.__rawTouchDown = true;
                } else {
                    window.__rawTouchDown = false;
                }
            };
            canvas.addEventListener('touchstart', updateFromTouch, { passive: true });
            canvas.addEventListener('touchmove', updateFromTouch, { passive: true });
            canvas.addEventListener('touchend', updateFromTouch, { passive: true });
            canvas.addEventListener('touchcancel', updateFromTouch, { passive: true });
        });
    }

    const bool down = EM_ASM_INT({ return window.__rawTouchDown ? 1 : 0; }) != 0;
    if (down) {
        const double xNorm = EM_ASM_DOUBLE({ return window.__rawTouchXNorm; });
        const double yNorm = EM_ASM_DOUBLE({ return window.__rawTouchYNorm; });
        const sf::Vec2f windowSize = window.getSize().toVec2f();
        TouchInput::instance()->setDown(true, { static_cast<float>(xNorm) * windowSize.x, static_cast<float>(yNorm) * windowSize.y });
    }
    else if (TouchInput::instance()->isDown()) {
        TouchInput::instance()->setDown(false, TouchInput::instance()->getPosition());
    }
}
#endif

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
