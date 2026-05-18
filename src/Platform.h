#pragma once
#include <SFML/Graphics.hpp>

struct Platform {
    sf::RectangleShape shape;
    bool solid = true;

    Platform(float x, float y, float w, float h, bool solid, sf::Color color = sf::Color(240, 180, 90)) : solid(solid) {
        shape.setPosition({x, y});
        shape.setSize({w, h});
        if (solid) {
            shape.setFillColor(color);
        } else {
            shape.setFillColor({200, 200, 200, 100});
            shape.setOutlineThickness(2.f);
            shape.setOutlineColor({80, 80, 80});
        }
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};