#pragma once
#include <SFML/System/Vec2.hpp>

// Generic polling-style touch state, mirroring how sf::Keyboard::isKeyPressed
// is polled directly by gameplay code (see PlayerBehavior). VRSFML dropped
// the old sf::Touch polling class, so this reconstructs the same idiom from
// the sf::Event::TouchBegan/Moved/Ended events Engine::events() forwards
// here. Single-finger only (multi-touch is not needed by anything today).
class TouchInput
{
    static TouchInput* m_instance;

    bool down = false;
    sf::Vec2f position;

public:
    static TouchInput* instance();

    bool isDown() const;
    sf::Vec2f getPosition() const;

    // Called from Engine::events() only.
    void setDown(bool _down, sf::Vec2f _position);
    void setPosition(sf::Vec2f _position);
};
