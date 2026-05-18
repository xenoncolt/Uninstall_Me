#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Platform.h"
#include "Spike.h"
#include "Door.h"
#include "Levels.h"
#include <iostream>


bool resolveCollision(Player& player, const Platform& platform) {
    sf::FloatRect player_bounds = player.getBounds();
    sf::FloatRect platform_bounds = platform.getBounds();

    if (!platform.solid) {
        bool corrected = false;

        if (player_bounds.position.x < platform_bounds.position.x) {
            player.position.x += platform_bounds.position.x - player_bounds.position.x;
            player.velocityX = 0.f;
            corrected = true;
        } else if (player_bounds.position.x + player_bounds.size.x > platform_bounds.position.x + platform_bounds.size.x) {
            player.position.x += (platform_bounds.position.x + platform_bounds.size.x) - (player_bounds.position.x + player_bounds.size.x);
            player.velocityX = 0.f;
            corrected = true;
        }

        if (player_bounds.position.y < platform_bounds.position.y) {
            player.position.y += platform_bounds.position.y - player_bounds.position.y;
            player.velocityY = 0.f;
            corrected = true;
        } else if (player_bounds.position.y + player_bounds.size.y > platform_bounds.position.y + platform_bounds.size.y) {
            player.position.y += (platform_bounds.position.y + platform_bounds.size.y) - (player_bounds.position.y + player_bounds.size.y);
            player.velocityY = 0.f;
            player.on_ground = true;
            corrected = true;
        }

        return corrected;
    }

    if (!player_bounds.findIntersection(platform_bounds).has_value()) return false;

    
    // overlap for each side
    float overlap_left = (player_bounds.position.x + player_bounds.size.x) - platform_bounds.position.x;
    float overlap_right = (platform_bounds.position.x + platform_bounds.size.x) - player_bounds.position.x;
    float overlap_top = (player_bounds.position.y + player_bounds.size.y) - platform_bounds.position.y;
    float overlap_bottom = (platform_bounds.position.y + platform_bounds.size.y) - player_bounds.position.y;

    bool from_left = overlap_left < overlap_right;
    bool from_top = overlap_top < overlap_bottom;

    float minX = from_left ? overlap_left : overlap_right;
    float minY = from_top ? overlap_top : overlap_bottom;

    if (minY < minX) {
        // vertical collision
        if (from_top) {
            // land on top platform
            player.position.y -= overlap_top;
            player.velocityY = 0.f;
            player.on_ground = true;
            return true;
        } else {
            // hit the underside
            player.position.y += overlap_bottom;
            player.velocityY = 0.f;
        }
    } else {
        // horizontal collision
        if (from_left) player.position.x -= overlap_left;
        else player.position.x += overlap_right;
        player.velocityX = 0.f;
    }

    return false;
};

void loadLevel(int index, Player& player, std::vector<Platform>& platforms, std::vector<Spike>& spikes, Door& door, sf::Color floor_color) {
    LevelDef& def = LEVELS[index];

    player.spawn_point = def.player_start;
    player.position = def.player_start;
    player.velocityX = 0.f;
    player.velocityY = 0.f;
    player.on_ground = false;

    platforms.clear();
    for (const auto& p : def.platforms) {
        float min_x = p.a.x;
        if (p.b.x < min_x) min_x = p.b.x;
        if (p.c.x < min_x) min_x = p.c.x;
        if (p.d.x < min_x) min_x = p.d.x;

        float max_x = p.a.x;
        if (p.b.x > max_x) max_x = p.b.x;
        if (p.c.x > max_x) max_x = p.c.x;
        if (p.d.x > max_x) max_x = p.d.x;

        float min_y = p.a.y;
        if (p.b.y < min_y) min_y = p.b.y;
        if (p.c.y < min_y) min_y = p.c.y;
        if (p.d.y < min_y) min_y = p.d.y;

        float max_y = p.a.y;
        if (p.b.y > max_y) max_y = p.b.y;
        if (p.c.y > max_y) max_y = p.c.y;
        if (p.d.y > max_y) max_y = p.d.y;

        float w = max_x - min_x;
        float h = max_y - min_y;

        platforms.emplace_back(min_x, min_y, w, h, p.solid != 0, floor_color);
    }

    spikes.clear();
    for (auto& s : def.spikes) {
        for (int i = 0; i < s.count; i++) {
            spikes.emplace_back(s.x + i * 16.f, s.y);
        }
    }

    door = Door(def.door.x, def.door.y);
}

int main() {
    // window
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Uninstall Me");
    window.setFramerateLimit(0);


    // colors
    sf::Color bg_color(178, 120, 20);
    sf::Color floor_color(240, 180, 90);



    // font + dead count text
    sf::Font font;
    if (!font.openFromFile("assets/fonts/PorkyBold.otf")) {
        std::cout << "font not found" << std::endl;
        return -1;
    }

    sf::Text death_text(font);
    death_text.setCharacterSize(20);
    death_text.setFillColor(sf::Color::White);
    death_text.setPosition({10.f, 10.f});


    sf::Text level_text(font);
    level_text.setCharacterSize(28);
    level_text.setFillColor(sf::Color::White);
    level_text.setString("Level Complete!");
    level_text.setPosition({270.f, 260.f});


    // sound
    sf::SoundBuffer jump_buffer;
    bool jump_sound_loaded = jump_buffer.loadFromFile("assets/sounds/jump.wav");
    sf::Sound jump_sound(jump_buffer);
    if (jump_sound_loaded) {
        jump_sound.setVolume(50.f);
        jump_sound.setPitch(1.4f);
    }

    sf::SoundBuffer win_buffer;
    bool win_sound_loaded = win_buffer.loadFromFile("assets/sounds/not-bad-not-bad.mp3");
    sf::Sound win_sound(win_buffer);
    if (win_sound_loaded) {
        win_sound.setVolume(50.f);
        win_sound.setPitch(1.2f);
    }


    // player
    Player player;
    std::vector<Platform> platforms;
    std::vector<Spike> spikes;
    Door door(0, 0);

    int current_level = 0;
    bool level_complete = false;

    loadLevel(current_level, player, platforms, spikes, door, floor_color);



    // clock for delta time
    sf::Clock clock;



    // main loop
    while (window.isOpen())
    {
        float delta_time = clock.restart().asSeconds();
        // nabeeha pls buy new pc .. unga bunga flying 
        if (delta_time > 0.05f) delta_time = 0.05f;
        


        // events
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            // jump sound
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Space && player.on_ground && jump_sound_loaded && !level_complete) {
                    jump_sound.play();
                }

                if (level_complete) {
                    current_level++;
                    if (current_level >= LEVEL_COUNT) current_level = 0;
                    level_complete = false;
                    loadLevel(current_level, player, platforms, spikes, door, floor_color);
                }
            }
        }

        if (!level_complete) {
            // physics update
            player.handleInput();
            player.update(delta_time);

            // collision
            for (const auto& plat : platforms) {
                resolveCollision(player, plat);
            }

            // Spikes
            for (const auto& spike : spikes) {
                if (spike.kills(player)) {
                    player.die();
                    break;
                }
            }


            // fell means dieeee
            if (player.position.y > 620.f) player.die();

            if (!door.triggered && player.getBounds().findIntersection(door.getBounds()).has_value()) door.triggered = true;

            door.update(delta_time);

            if (door.done) level_complete = true;
        }



        death_text.setString("Death: " + std::to_string(player.death_count));


        if (level_complete) {
            win_sound.play();
            level_text.setString("Level " + std::to_string(current_level) + " Complete!\nPress any key...");
        }

        window.clear(bg_color);
        for (auto& plat: platforms) plat.draw(window);
        for (auto& spike: spikes) spike.draw(window);
        door.draw(window);

        player.draw(window);
    
        window.draw(death_text);

        if (level_complete) window.draw(level_text);

        window.display();
    }

    return 0;
}