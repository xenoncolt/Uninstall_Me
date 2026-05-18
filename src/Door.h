#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

struct Door {
    float x, y;
    static constexpr float W = 40.f, H = 55.f;
    static constexpr float IW = 30.f, IH = 40.f; // inner opening

    sf::RectangleShape frame;
    sf::RectangleShape inner;
    sf::RectangleShape panel_left;
    sf::RectangleShape panel_right;

    bool triggered = false;
    bool done = false;
    float timer = 0.f;
    float duration = 0.5f;

    Door(float x, float y) : x(x), y(y) {
        frame.setSize({W, H});
        frame.setFillColor({150, 100, 30});
        frame.setPosition({x, y});

        inner.setSize({IW, IH});
        inner.setFillColor({210, 210, 210});
        inner.setPosition({x + 5.f, y + 15.f});

        panel_left.setFillColor({70, 45, 10});
        panel_right.setFillColor({70, 45, 10});
    }

    sf::FloatRect getBounds() const {
        return sf::FloatRect({x, y}, {W, H});
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