#pragma once
#include <SFML/Graphics.hpp>

struct Platform {
    sf::RectangleShape shape;

    Platform(float x, float y, float w, float h, sf::Color color = sf::Color(240, 180, 90)) {
        shape.setPosition({x, y});
        shape.setSize({w, h});
        shape.setFillColor(color);
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};