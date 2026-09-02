#include "TouchInput.h"

TouchInput* TouchInput::m_instance = nullptr;

TouchInput* TouchInput::instance() {
    if (m_instance == nullptr) {
        m_instance = new TouchInput();
    }
    return m_instance;
}

bool TouchInput::isDown() const {
    return down;
}

sf::Vec2f TouchInput::getPosition() const {
    return position;
}

void TouchInput::setDown(bool _down, sf::Vec2f _position) {
    down = _down;
    position = _position;
}

void TouchInput::setPosition(sf::Vec2f _position) {
    position = _position;
}
