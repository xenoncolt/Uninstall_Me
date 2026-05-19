#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <cmath>

struct Wall {
    sf::RectangleShape shape;
    float push_dist;
    bool  player_inside = false;   // true when player was in zone last frame

    Wall(float x, float y, float w, float h, float push_dist, sf::Color color = {80, 50, 20})
        : push_dist(push_dist)
    {
        shape.setPosition({x, y});
        shape.setSize({w, h});
        shape.setFillColor(color);
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

    void pushPlayer(Player& player) {
        sf::FloatRect wb = getBounds();
        float pcx = player.position.x + Player::width  * 0.5f;
        float pcy = player.position.y + Player::height * 0.5f;

        // Nearest point on wall rectangle
        float nx = std::max(wb.position.x, std::min(pcx, wb.position.x + wb.size.x));
        float ny = std::max(wb.position.y, std::min(pcy, wb.position.y + wb.size.y));

        float dx   = pcx - nx;
        float dy   = pcy - ny;
        float dist = std::sqrt(dx * dx + dy * dy);

        bool in_zone = (dist > 0.01f && dist < push_dist);

        if (in_zone && !player_inside) {
            float len = dist;
            float dir_x = dx / len;
            float dir_y = dy / len;

            player.push_vel_x = dir_x * 600.f;
            player.push_vel_y = dir_y * 250.f;
        }

        player_inside = in_zone;
    }

    // Call this on player death so the wall can shove again on respawn
    void reset() { player_inside = false; }

    void draw(sf::RenderWindow& window) { window.draw(shape); }
};