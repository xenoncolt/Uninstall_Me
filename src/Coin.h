#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

struct Coin {
    sf::CircleShape shape;
    bool fake;
    bool collected = false;

    static constexpr float RADIUS = 8.f;

    Coin(float x, float y, bool fake) : fake(fake) {
        shape.setRadius(RADIUS);
        shape.setOrigin({RADIUS, RADIUS});
        shape.setPosition({x, y});
        // real = gold, fake = same gold (player cant tell!)
        shape.setFillColor({255, 210, 0});
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor({200, 160, 0});
    }

    bool touches(const Player& p) const {
        if (collected) return false;
        return shape.getGlobalBounds().findIntersection(p.getBounds()).has_value();
    }

    void draw(sf::RenderWindow& w) const {
        if (collected) return;
        w.draw(shape);
    }
};