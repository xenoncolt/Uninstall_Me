#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Platform.h"
#include <iostream>


bool resolveCollision(Player& player, const Platform& platform) {
    sf::FloatRect player_bounds = player.getBounds();
    sf::FloatRect platform_bounds = platform.getBounds();

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

    bool landed_on_top = false;

    if (minY < minX) {
        // vertical collision
        if (from_top) {
            // land on top platform
            player.position.y -= overlap_top;
            player.velocityY = 0.f;
            player.on_ground = true;
            landed_on_top = true;
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

    return landed_on_top;
};

int main() {
    // window
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Uninstall Me");
    window.setFramerateLimit(0);


    // colors
    sf::Color bg_color(178, 120, 20);
    sf::Color floor_color(240, 180, 90);



    // platforms
    std::vector<Platform> platforms;
    platforms.emplace_back(80.f, 270.f, 600.f, 120.f, floor_color);
    platforms.emplace_back(720.f, 270.f, 250.f, 120.f, floor_color);
    // deadzone is x= 680 to x = 720 .. 40 px 



    // door
    sf::RectangleShape door_frame({40.f, 55.f});
    door_frame.setFillColor({178, 140, 50});
    door_frame.setPosition({790.f, 215.f});

    sf::RectangleShape door_inner({30.f, 45.f});
    door_inner.setFillColor({200, 200, 200});
    door_inner.setPosition({795.f, 225.f});



    // clock for delta time
    sf::Clock clock;



    // player
    Player player;
    player.position = {120.f, 230.f}; // start here



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



    // sound
    sf::SoundBuffer jump_buffer;
    bool jump_sound_loaded = jump_buffer.loadFromFile("assets/sounds/jump.wav");
    sf::Sound jump_sound(jump_buffer);
    if (jump_sound_loaded) {
        jump_sound.setVolume(50.f);
        jump_sound.setPitch(1.4f);
    }


    
    // fell off screen
    float deathY = 600.f;




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
                if (key->code == sf::Keyboard::Key::Space && player.on_ground && jump_sound_loaded) {
                    jump_sound.play();
                }
            }
        }

        // physics update
        player.handleInput();
        player.update(delta_time);



        // collision 
        for (const auto& plat : platforms) resolveCollision(player, plat);
        



        // fell means dieeee
        if (player.position.y > deathY) player.die();

        death_text.setString("Death: " + std::to_string(player.death_count));





        // drawing 
        window.clear(bg_color);
        for (auto& plat: platforms) plat.draw(window);

        window.draw(door_frame);
        window.draw(door_inner);

        player.draw(window);
    
        window.draw(death_text);

        window.display();
    }
    

    // sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Uninstall Me - TEST");
    // sf::RectangleShape player(sf::Vector2f({50.f, 50.f}));
    // player.setFillColor(sf::Color::Green);
    // player.setPosition({100.f, 300.f});

    // // frame
    // sf::Clock clock;

    // // gravity
    // float velocityY = 0.f;
    // float gravity = 0.5f;
    // bool onGround = false;

    // // drawing text
    // sf::Font font;;
    // if (!font.openFromFile("assets/fonts/PorkyBold.otf")) {
    //     return -1;
    // }
    
    // sf::Text text(font);
    // text.setString("Death: 0");
    // text.setCharacterSize(24);
    // text.setFillColor(sf::Color::White);
    // text.setPosition({10.f, 10.f});


    // // sound
    // sf::SoundBuffer jump_buffer;
    // if (!jump_buffer.loadFromFile("assets/sounds/jump.wav")) {
    //     return -1;
    // } 
    // sf::Sound jump_sound(jump_buffer);
    // jump_sound.setVolume(100.f);

    // bool spaceWasDown = false;

    // while (window.isOpen()) {
    //     float delta_time = clock.restart().asSeconds();
    //     while (const auto event = window.pollEvent()) {
    //         if (event->is<sf::Event::Closed>())
    //             window.close();
    //     }

    //     float move_x = 0.f;
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
    //         move_x -= 200.f * delta_time;
    //     }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
    //         move_x += 200.f * delta_time;
    //     }

    //     player.move({move_x, 0.f});
    //     velocityY += gravity;
    //     player.move({0.f, velocityY * delta_time});

    //     if (player.getPosition().y + player.getSize().y >= 600.f) {
    //         player.setPosition({player.getPosition().x, 600.f - player.getSize().y});
    //         velocityY = 0.f;
    //         onGround = true;
    //     }

    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && onGround) {
    //         velocityY = -12.f;
    //         onGround = false;
    //     }

    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
    //         jump_sound.play();
    //     }

    //     window.clear(sf::Color::Black);
    //     window.draw(player);
    //     window.draw(text);
    //     window.display();
    // }
    return 0;
}