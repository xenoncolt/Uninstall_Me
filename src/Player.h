#pragma once
#include <SFML/Graphics.hpp>

class Player {
    public:
        sf::RectangleShape head;
        sf::RectangleShape body;
        sf::RectangleShape front_arm;
        sf::RectangleShape back_arm;
        sf::RectangleShape front_leg;
        sf::RectangleShape back_leg;
        sf::RectangleShape hit_box;

        // physics
        sf::Vector2f position = {100.f, 300.f};
        sf::Vector2f spawn_point = {100.f, 300.f};
        float velocityX = 0.f;
        float velocityY = 0.f;
        bool on_ground = false;
        int death_count = 0;

        // animation
        float  animation_timer = 0.f;
        int face_direction = 1;
        bool is_walking = false;

        // hitbox size
        static constexpr float width = 10.f;
        static constexpr float height = 30.f;

        // color part
        sf::Color body_color = sf::Color(255, 165, 0);

        
        Player() {
            // head size 10 x 9
            // body 22 x 18
            // hand will 6 x 12
            // legs 8 x 14

            head.setSize({10.f, 9.f});
            head.setFillColor(body_color);

            body.setSize({8.f, 10.f});
            body.setFillColor(body_color);

            front_arm.setSize({4.f, 10.f});
            front_arm.setFillColor(body_color);

            back_arm.setSize({4.f, 10.f});
            back_arm.setFillColor(body_color);

            front_leg.setSize({3.f, 7.f});
            front_leg.setFillColor(body_color);

            back_leg.setSize({3.f, 7.f});
            back_leg.setFillColor(body_color);

            hit_box.setSize({width, height}); // whole body but not whole body xD .. 
            hit_box.setFillColor(sf::Color::Transparent);
            hit_box.setOutlineColor(sf::Color::Red);
            hit_box.setOutlineThickness(0.f);
        }

        void handleInput() {
            velocityX = 0.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                velocityX = -200.f;
                face_direction = -1;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                velocityX = 200.f;
                face_direction = 1;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && on_ground) {
                velocityY = -250.f;
                on_ground = false;
            }
        }

        // frame update i love godot
        void update(float delta_time) {
            velocityY += 900.f * delta_time; 
            position.x += velocityX * delta_time;
            position.y += velocityY * delta_time;
            on_ground = false;  // for reseting collusion check
            animation_timer += delta_time;
        }

        sf::FloatRect getBounds() const {
            return sf::FloatRect(position, {width, height});
        }

        // animation + position
        void updateShapes() {
            bool is_walking = (std::abs(velocityX) > 10.f && on_ground);
            bool is_airborne = !on_ground;
            float face_dir = static_cast<float>(face_direction);


            float leg_swing = 0.f,
                  arm_swing = 0.f,
                  head_bob = 0.f,
                  head_lean = 0.f;


            if (is_walking) {
                leg_swing = std::sin(animation_timer * 13.f) * 5.f * face_dir;
                arm_swing = -leg_swing * 0.6f;
                head_bob = std::sin(animation_timer * 13.f) * 1.2f;
                head_lean = face_dir * 1.5f;
            } else if (is_airborne) {
                leg_swing = face_dir * 3.f;
                arm_swing = face_dir * -2.f;
                head_lean = face_dir * 2.f;
            } else {
                head_bob = std::sin(animation_timer * 2.f) * 0.8f;
                leg_swing = std::sin(animation_timer * 2.f) * 0.5f;
            }


            float X = position.x, Y = position.y;
            head.setPosition({
                X + head_lean,
                Y + head_bob
            });
            body.setPosition({
                X + 1.f,
                Y + 9.f
            });
            back_arm.setPosition({
                X + 1.f - arm_swing,
                Y + 10.f
            });
            back_leg.setPosition({
                X + 3.f - leg_swing,
                Y + 19.f
            });
            front_arm.setPosition({
                X + 1.f + arm_swing,
                Y + 10.f
            });
            front_leg.setPosition({
                X + 3.f + leg_swing,
                Y + 19.f
            });
        }


        void draw(sf::RenderWindow& window) {
            updateShapes();  

            window.draw(back_arm);
            window.draw(front_arm);
            window.draw(back_leg);
            window.draw(front_leg);
            window.draw(body);
            window.draw(head);
            //window.draw(hit_box); // for debug
        }

        // death + respawn
        void die() {
            death_count++;
            position = spawn_point;
            velocityX = 0.f;
            velocityY = 0.f;
            animation_timer = 0.f;
            on_ground = false;
        }
};