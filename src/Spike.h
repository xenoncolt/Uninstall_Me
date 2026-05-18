#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

struct Spike {
    sf::ConvexShape shape;

    Spike(float x, float y) {
        shape.setPointCount(3);
        shape.setPoint(0, {0.f,  14.f});   // bottom left
        shape.setPoint(1, {6.f,   0.f});   // tip (top center)
        shape.setPoint(2, {12.f, 14.f});   // bottom right
        shape.setFillColor({20, 20, 20});  // same dark color as player
        shape.setPosition({x, y});
    }

    bool kills(const Player& p) const {
        return shape.getGlobalBounds().findIntersection(p.getBounds()).has_value();
    }

    void draw(sf::RenderWindow& w) {
        w.draw(shape);
    }
};