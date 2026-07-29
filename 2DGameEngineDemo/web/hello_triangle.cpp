#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <box2d/box2d.h>

#include <emscripten.h>

#include <utility>

namespace
{
sf::GraphicsContext* graphicsContext = nullptr;
sf::RenderWindow* window = nullptr;
sf::CircleShape* shape = nullptr;
b2WorldId worldId;

void tick()
{
    while (const auto event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            emscripten_cancel_main_loop();
            return;
        }
    }

    b2World_Step(worldId, 1.f / 60.f, 4);

    window->clear();
    window->draw(*shape);
    window->display();
}
} // namespace

int main()
{
    static auto graphicsContextStorage = sf::GraphicsContext::create().value();
    graphicsContext = &graphicsContextStorage;

    static auto windowStorage = sf::RenderWindow::create({ .size = { 800u, 600u }, .title = "Phase 0 - hello triangle" }).value();
    window = &windowStorage;
    window->setFramerateLimit(60);

    static sf::CircleShape shapeStorage(
        { .position = { 300.f, 200.f }, .fillColor = sf::Color::Green, .radius = 100.f, .pointCount = 3 });
    shape = &shapeStorage;

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0.0f, 10.0f };
    worldId = b2CreateWorld(&worldDef);

    emscripten_set_main_loop(tick, 0, 1);
    return 0;
}
