#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

struct Door {
    float orig_x, orig_y;
    float x, y;
    float flee_x, flee_y;
    float trigger_radius;

    static constexpr float W = 40.f, H = 55.f;
    static constexpr float IW = 30.f, IH = 40.f;

    sf::RectangleShape frame;
    sf::RectangleShape inner;
    sf::RectangleShape panel_left;
    sf::RectangleShape panel_right;

   
    bool fled = false;
    bool triggered = false;
    bool done = false;
    float timer = 0.f;
    float duration = 0.5f;



    Door(float x, float y, float flee_x = 0.f, float flee_y = 0.f, float trigger_radius = 0.f)
        : orig_x(x), orig_y(y), x(x), y(y), flee_x(flee_x), flee_y(flee_y), trigger_radius(trigger_radius)
    {
        buildShapes();
    }



    void buildShapes() {
        frame.setSize({W, H});
        frame.setFillColor({150, 100, 30});
        frame.setPosition({x, y});

        inner.setSize({IW, IH});
        inner.setFillColor({210, 210, 210});
        inner.setPosition({x + 5.f, y + 15.f});

        panel_left.setSize({0.f, IH});
        panel_left.setFillColor({70, 45, 10});
        panel_left.setPosition({x + 5.f, y + 15.f});

        panel_right.setSize({0.f, IH});
        panel_right.setFillColor({70, 45, 10});
        panel_right.setPosition({x + 5.f + IW, y + 15.f});
    }

    sf::FloatRect getBounds() const {
        return sf::FloatRect({x, y}, {W, H});
    }

    
    // change door location
    void checkFlee(const Player& player) {
        if (fled || trigger_radius <= 0.f || triggered) return;
        float px = player.position.x + Player::width  * 0.5f;
        float py = player.position.y + Player::height * 0.5f;
        float cx = x + W * 0.5f;
        float cy = y + H * 0.5f;
        float dx = px - cx, dy = py - cy;
        if (dx * dx + dy * dy <= trigger_radius * trigger_radius) {
            x = flee_x;
            y = flee_y;
            fled = true;
            buildShapes();
        }
    }


    // reset after dead
    void reset() {
        x = orig_x;
        y = orig_y;
        fled      = false;
        triggered = false;
        done      = false;
        timer     = 0.f;
        buildShapes();
    }



    void update(float delta_time) {
        if (!triggered || done) return;
        timer += delta_time;
        float progress = std::min(timer / duration, 1.f);
        float panel_w = progress * (IW / 2.f);
        panel_left.setSize({panel_w, IH});
        panel_left.setPosition({x + 5.f, y + 15.f});
        panel_right.setSize({panel_w, IH});
        panel_right.setPosition({x + 5.f + IW - panel_w, y + 15.f});
        if (progress >= 1.f) done = true;
    }



    void draw(sf::RenderWindow& window) {
        window.draw(frame);
        window.draw(inner);
        if (triggered) {
            window.draw(panel_left);
            window.draw(panel_right);
        }
    }
};