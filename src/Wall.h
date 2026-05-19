#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <cmath>

struct Wall {
    sf::RectangleShape shape;
    float push_dist;

    Wall(float x, float y, float w, float h, float push_dist, sf::Color color = {80, 50, 20})
        : push_dist(push_dist)
    {
        shape.setPosition({x, y});
        shape.setSize({w, h});
        shape.setFillColor(color);
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

    // pushes player 
    void pushPlayer(Player& player) {
        sf::FloatRect wb = getBounds();
        float pcx = player.position.x + Player::width  * 0.5f;
        float pcy = player.position.y + Player::height * 0.5f;

        
        float nx = std::max(wb.position.x, std::min(pcx, wb.position.x + wb.size.x));
        float ny = std::max(wb.position.y, std::min(pcy, wb.position.y + wb.size.y));

        
        float dx = pcx - nx;
        float dy = pcy - ny;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < push_dist) {
            float len     = dist < 0.01f ? 1.f : dist;
            float dir_x   = dist < 0.01f ? 1.f : dx / len;
            float dir_y   = dist < 0.01f ? 0.f : dy / len;
            float push    = push_dist - dist;

            player.position.x += dir_x * push;
            player.position.y += dir_y * push;



            float dot = player.velocityX * (-dir_x) + player.velocityY * (-dir_y);
            if (dot > 0.f) {
                player.velocityX += dir_x * dot;
                player.velocityY += dir_y * dot;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};