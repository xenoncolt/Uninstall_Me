#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Platform.h"
#include "Spike.h"
#include "Coin.h"
#include "Door.h"
#include "Wall.h"   
#include "Levels.h"
#include <iostream>
#include <cmath>
#include <random>

static const char* DEATH_MSGS[] = {
    "bro really thought he could do it",
    "have you considered uninstalling?",
    "skill issue (not affectionate)",
    "the game is working as intended.\nYOU are not.",
    "even the spikes feel bad for you",
    "L + ratio + touch grass",
    "death speedrun any% WR",
    "maybe stick to tic-tac-toe?",
    "the DOOR is literally running away from you.",
    "cope + seethe + die again",
    "your family is watching. just saying.",
    "bro fought the spike and lost. to a triangle.",
    "i put a fake coin there specifically for you",
    "you're the reason this game is called Uninstall Me",
};
static constexpr int DEATH_MSG_COUNT = 14;

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

    float overlap_left   = (player_bounds.position.x + player_bounds.size.x) - platform_bounds.position.x;
    float overlap_right  = (platform_bounds.position.x + platform_bounds.size.x) - player_bounds.position.x;
    float overlap_top    = (player_bounds.position.y + player_bounds.size.y) - platform_bounds.position.y;
    float overlap_bottom = (platform_bounds.position.y + platform_bounds.size.y) - player_bounds.position.y;

    bool from_left = overlap_left < overlap_right;
    bool from_top  = overlap_top  < overlap_bottom;

    float minX = from_left ? overlap_left  : overlap_right;
    float minY = from_top  ? overlap_top   : overlap_bottom;

    if (minY < minX) {
        if (from_top) {
            player.position.y -= overlap_top;
            player.velocityY   = 0.f;
            player.on_ground   = true;
            return true;
        } else {
            player.position.y += overlap_bottom;
            player.velocityY   = 0.f;
        }
    } else {
        if (from_left) player.position.x -= overlap_left;
        else           player.position.x += overlap_right;
        player.velocityX = 0.f;
    }

    return false;
}


void loadLevel(int index, Player& player, std::vector<Platform>& platforms,
               std::vector<Spike>& spikes, std::vector<Coin>& coins,
               std::vector<Wall>& walls, Door& door, sf::Color floor_color)
{
    LevelDef& def = LEVELS[index];

    player.spawn_point = def.player_start;
    player.position    = def.player_start;
    player.velocityX   = 0.f;
    player.velocityY   = 0.f;
    player.on_ground   = false;
    player.jump_speed  = def.jump_speed;

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

        platforms.emplace_back(min_x, min_y, max_x - min_x, max_y - min_y, p.solid != 0, floor_color);
    }

    spikes.clear();
    for (auto& s : def.spikes)
        for (int i = 0; i < s.count; i++)
            spikes.emplace_back(s.x + i * 16.f, s.y, s.trigger_radius, s.direction);

    coins.clear();
    for (const auto& c : def.coins)
        coins.emplace_back(c.x, c.y, c.fake);

    walls.clear();
    for (const auto& w : def.walls)
        walls.emplace_back(w.x, w.y, w.w, w.h, w.push_dist);

    door = Door(def.door.x, def.door.y,
                def.door.flee_x, def.door.flee_y,
                def.door.trigger_radius);
}

void resetOnDeath(std::vector<Spike>& spikes, std::vector<Coin>& coins, Door& door, std::vector<Wall>& walls) {
    for (auto& s : spikes) s.reset();
    for (auto& c : coins)  c.collected = false;
    door.reset();
    for (auto& w : walls)  w.reset();
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

    sf::Text coin_text(font);
    coin_text.setCharacterSize(20);
    coin_text.setFillColor(sf::Color::White);
    coin_text.setPosition({10.f, 35.f});

    sf::Text level_text(font);
    level_text.setCharacterSize(28);
    level_text.setFillColor(sf::Color::White);
    level_text.setPosition({270.f, 260.f});

    sf::RectangleShape popup_box;
    popup_box.setSize({420.f, 180.f});
    popup_box.setPosition({190.f, 210.f});
    popup_box.setFillColor({30, 30, 30, 230});
    popup_box.setOutlineColor({220, 50, 50});
    popup_box.setOutlineThickness(3.f);

    sf::Text popup_title(font);
    popup_title.setCharacterSize(26);
    popup_title.setFillColor({220, 50, 50});
    popup_title.setPosition({210.f, 220.f});

    sf::Text popup_msg(font);
    popup_msg.setCharacterSize(17);
    popup_msg.setFillColor(sf::Color::White);
    popup_msg.setPosition({210.f, 258.f});

    sf::RectangleShape btn_box;
    btn_box.setSize({300.f, 36.f});
    btn_box.setPosition({250.f, 340.f});
    btn_box.setFillColor({80, 20, 20});
    btn_box.setOutlineColor({200, 60, 60});
    btn_box.setOutlineThickness(2.f);

    sf::Text btn_text(font);
    btn_text.setCharacterSize(16);
    btn_text.setFillColor(sf::Color::White);
    btn_text.setString("[ try again (you will fail) ]");
    btn_text.setPosition({262.f, 348.f});

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

    sf::SoundBuffer death_buffer;
    bool death_sound_loaded = death_buffer.loadFromFile("assets/sounds/ew1.mp3");
    sf::Sound death_sound(death_buffer);
    if (death_sound_loaded) {
        death_sound.setVolume(50.f);
        death_sound.setPitch(1.3f);
    }

    sf::SoundBuffer cat_laugh_buffer;
    bool cat_laugh_sound_loaded = cat_laugh_buffer.loadFromFile("assets/sounds/cat-laugh.mp3");
    sf::Sound cat_laugh_sound(cat_laugh_buffer);
    if (cat_laugh_sound_loaded) {
        cat_laugh_sound.setVolume(50.f);
        cat_laugh_sound.setPitch(1.4f);
    }

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> death_sound_pick(0, 1);
    auto playRandomDeathSound = [&]() {
        if (!death_sound_loaded && !cat_laugh_sound_loaded) return;
        if (death_sound_loaded && !cat_laugh_sound_loaded) {
            death_sound.play();
            return;
        }
        if (!death_sound_loaded && cat_laugh_sound_loaded) {
            cat_laugh_sound.play();
            return;
        }

        if (death_sound_pick(rng) == 0) death_sound.play();
        else cat_laugh_sound.play();
    };

    // player
    Player player;
    std::vector<Platform> platforms;
    std::vector<Spike> spikes;
    std::vector<Coin> coins;
    std::vector<Wall> walls;   
    Door door(0, 0);

    int  current_level  = 0;
    bool level_complete = false;

    bool show_death_popup = false;
    const char* death_msg = DEATH_MSGS[0];

    loadLevel(current_level, player, platforms, spikes, coins, walls, door, floor_color);

    // clock for delta time
    sf::Clock clock;

    // main loop
    while (window.isOpen())
    {
        float delta_time = clock.restart().asSeconds();
        // nabeeha pls buy new pc .. unga bunga flying 
        if (delta_time > 0.05f) delta_time = 0.05f;

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Space && player.on_ground && jump_sound_loaded && !level_complete && !show_death_popup)
                    jump_sound.play();

                if (level_complete) {
                    current_level++;
                    if (current_level >= LEVEL_COUNT) current_level = 0;
                    level_complete = false;
                    loadLevel(current_level, player, platforms, spikes, coins, walls, door, floor_color);
                }


                if (show_death_popup) {
                    show_death_popup = false;
                    player.respawn();
                }
            }

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (show_death_popup) {
                    sf::Vector2f mouse = {(float)click->position.x, (float)click->position.y};
                    if (btn_box.getGlobalBounds().contains(mouse)) {
                        show_death_popup = false;
                        player.respawn();
                    }
                }
            }
        }

        if (!level_complete && !player.is_dead && !show_death_popup) {
            // physics update
            player.handleInput();
            player.update(delta_time);

            for (const auto& plat : platforms)
                resolveCollision(player, plat);

            for (auto& wall : walls)
                wall.pushPlayer(player);

            for (auto& spike : spikes) {
                spike.update(player, delta_time);
                if (spike.kills(player)) {
                    death_msg = DEATH_MSGS[player.death_count % DEATH_MSG_COUNT];
                    player.startDeath();
                    playRandomDeathSound();
                    resetOnDeath(spikes, coins, door, walls);
                    break;
                }
            }

            for (auto& coin : coins) {
                if (coin.touches(player)) {
                    if (coin.fake) {
                        death_msg = DEATH_MSGS[player.death_count % DEATH_MSG_COUNT];
                        player.startDeath();
                        playRandomDeathSound();
                        resetOnDeath(spikes, coins, door, walls);
                        break;
                    } else {
                        coin.collected = true;
                    }
                }
            }

            if (player.position.y > 620.f) {
                death_msg = DEATH_MSGS[player.death_count % DEATH_MSG_COUNT];
                player.startDeath();
                playRandomDeathSound();
                resetOnDeath(spikes, coins, door, walls);
            }

            door.checkFlee(player);

            if (!door.triggered && player.getBounds().findIntersection(door.getBounds()).has_value())
                door.triggered = true;

            door.update(delta_time);
            if (door.done) level_complete = true;
        }

        if (player.is_dead && !show_death_popup) {
            bool finished = player.updateDeath(delta_time);
            if (finished) {
                show_death_popup = true;
                popup_title.setString("YOU DIED  x" + std::to_string(player.death_count));
                popup_msg.setString(death_msg);
            }
        }

        int collected = 0, total_real = 0;
        for (const auto& c : coins) {
            if (!c.fake) {
                total_real++;
                if (c.collected) collected++;
            }
        }

        death_text.setString("Death: " + std::to_string(player.death_count));
        if (total_real > 0)
            coin_text.setString("Coins: " + std::to_string(collected) + "/" + std::to_string(total_real));
        else
            coin_text.setString("");

        if (level_complete) {
            win_sound.play();
            level_text.setString("Level " + std::to_string(current_level) + " Complete!\nPress any key...");
        }

        window.clear(bg_color);
        for (auto& plat  : platforms) plat.draw(window);
        for (auto& wall  : walls)     wall.draw(window);
        for (auto& spike : spikes)    spike.draw(window);
        for (auto& coin  : coins)     coin.draw(window);
        door.draw(window);

        player.draw(window);
    
        window.draw(death_text);

        if (total_real > 0) window.draw(coin_text);
        if (level_complete) window.draw(level_text);

        
        if (show_death_popup) {
            window.draw(popup_box);
            window.draw(popup_title);
            window.draw(popup_msg);
            window.draw(btn_box);
            window.draw(btn_text);
        }

        window.display();
    }

    return 0;
}