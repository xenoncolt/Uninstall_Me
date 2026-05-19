#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

struct Spike {
    sf::ConvexShape shape;
    float base_y;
    bool triggered = false;
    float anim_timer = 0.f;
    float trigger_radius;
    int direction;
    static constexpr float ANIM_DUR = 0.3f;

    Spike(float x, float y, float trigger_radius = 90.f, int direction = 0)
        : base_y(y), trigger_radius(trigger_radius), direction(direction)
    {
        shape.setPointCount(3);

        if (direction == 0) {

            shape.setPoint(0, {0.f,  14.f});
            shape.setPoint(1, {6.f,   0.f});
            shape.setPoint(2, {12.f, 14.f});

            shape.setPosition({x, y + 14.f});
        } else {

            shape.setPoint(0, {0.f,   0.f});
            shape.setPoint(1, {6.f,  14.f});
            shape.setPoint(2, {12.f,  0.f});

            shape.setPosition({x, y - 14.f});
        }

        shape.setFillColor({20, 20, 20});
    }

    void reset() {
        triggered  = false;
        anim_timer = 0.f;
        sf::Vector2f pos = shape.getPosition();

        if (direction == 0)
            shape.setPosition({pos.x, base_y + 14.f});
        else
            shape.setPosition({pos.x, base_y - 14.f});
    }

    void update(const Player& p, float dt) {
        float px = p.position.x + Player::width  * 0.5f;
        float py = p.position.y + Player::height * 0.5f;
        sf::Vector2f pos = shape.getPosition();
        float cx = pos.x + 6.f;
        float cy = base_y + 7.f;
        float dx = px - cx, dy = py - cy;
        if (!triggered && (dx*dx + dy*dy) <= trigger_radius * trigger_radius)
            triggered = true;
        if (triggered && anim_timer < ANIM_DUR) {
            anim_timer += dt;
            if (anim_timer > ANIM_DUR) anim_timer = ANIM_DUR;
            float progress = anim_timer / ANIM_DUR;

            float offset = (1.f - progress) * 14.f;
            if (direction == 0)
                shape.setPosition({pos.x, base_y + offset});
            else
                shape.setPosition({pos.x, base_y - offset});
        }
    }

    bool kills(const Player& p) const {
        if (anim_timer < ANIM_DUR) return false;
        return shape.getGlobalBounds().findIntersection(p.getBounds()).has_value();
    }

    void draw(sf::RenderWindow& w) {
        if (!triggered) return;
        w.draw(shape);
    }
};