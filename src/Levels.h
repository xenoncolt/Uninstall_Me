#pragma once
#include <SFML/Graphics.hpp>


struct RectDef
{
    sf::Vector2f a, b, c, d;
    int solid;
};
// RectDef { A, B, C, D, solid }
inline RectDef RectAC(sf::Vector2f a, sf::Vector2f c, int solid)
{
    return {a, {c.x, a.y}, c, {a.x, c.y}, solid};
}

// SpikeDef { x, y, count, trigger_radius, direction }
struct SpikeDef
{
    float x, y;
    int   count;
    float trigger_radius = 90.f;
    int   direction = 0;
};

// CoinDef { x, y, fake }
struct CoinDef
{
    float x, y;
    bool  fake = false;
};

// DoorDef { x, y, trigger_radius, flee_x, flee_y }
struct DoorDef
{
    float x, y;
    float trigger_radius = 0.f;
    float flee_x = 0.f, flee_y = 0.f;
};

struct WallDef
{
    float x, y, w, h;
    float push_dist;
};

struct LevelDef
{
    sf::Vector2f          player_start;
    DoorDef               door;
    std::vector<RectDef>  platforms;
    std::vector<SpikeDef> spikes;
    std::vector<CoinDef>  coins;
    std::vector<WallDef>  walls;
    float jump_speed = 250.f;
};

inline LevelDef LEVELS[] = {

    {
        {169.f, 330.f},
        {650.f, 345.f},
        {
            RectAC({100.f, 200.f}, {700.f, 400.f}, 0),
        },
        {
            
            {533.f, 385.f, 3, 50.f, 0},
        },
        {
            // {200.f, 380.f, false},
            // {260.f, 380.f, false},
            // {320.f, 395.f, true},
            // {380.f, 380.f, false},
            // {440.f, 380.f, false},
        },
        {},
        250.f,  
    },
    {
        {650.f, 250.f},
        {150.f, 345.f},
        {
            RectAC({100.f, 200.f}, {700.f, 400.f}, 0),
            RectAC({550.f, 300.f}, {700.f, 320.f}, 1),
            RectAC({464.f, 370.f}, {477.f, 400.f}, 1),
        },
        {
            {420.f, 390.f, 3, 50.f, 0},
            {480.f, 390.f, 3, 50.f, 0},
        },
        {},
        {},
        250.f,
    },
    {
        {650.f, 250.f},
        {150.f, 445.f, 50.f, 650.f, 445.f},
        {
            RectAC({100.f, 200.f}, {700.f, 500.f}, 0),
            RectAC({550.f, 300.f}, {700.f, 320.f}, 1),
            RectAC({464.f, 470.f}, {477.f, 500.f}, 1),
        },
        {
            {420.f, 490.f, 3, 50.f, 0},
            {480.f, 490.f, 3, 50.f, 0},
        },
        {},
        {},
        250.f,
    },
    {
        {169.f, 430.f},
        {650.f, 445.f},
        {
            RectAC({100.f, 200.f}, {700.f, 500.f}, 0),
        },
        {},
        {
            {200.f, 485.f, false},
            {260.f, 485.f, false},
            {320.f, 485.f, true},
            {380.f, 485.f, false},
            {440.f, 485.f, false},
            {500.f, 485.f, true},
            {560.f, 485.f, false},
            {620.f, 485.f, false},
        },
        {},
        260.f,
    },

    {
        {80.f, 510.f},                       
        {660.f, 505.f, 75.f, 130.f, 265.f}, 
        {
            RectAC({50.f,  100.f}, {750.f, 560.f}, 0),  
            RectAC({100.f, 320.f}, {260.f, 340.f}, 1),  
            RectAC({510.f, 240.f}, {680.f, 260.f}, 1),  
        },
        {
            
            {100.f, 546.f, 1, 12.f, 0},  
            {150.f, 546.f, 3, 18.f, 0},  
            {500.f, 546.f, 3, 18.f, 0},  
            {580.f, 546.f, 3, 30.f, 0}, 


            {310.f, 100.f, 5, 90.f, 1},  
            {540.f, 100.f, 3, 80.f, 1},  
        },
        {
            
            {170.f, 305.f, false},
           
            {680.f, 515.f, true},
        },
        {
            
            {370.f, 100.f, 15.f, 360.f, 35.f},
        },
        1000.f,
    },


    {
        {700.f, 510.f},                      
        {60.f, 505.f, 60.f, 650.f, 245.f},  
        {
            RectAC({20.f,  80.f},  {780.f, 560.f}, 0),  
            RectAC({50.f,  420.f}, {220.f, 440.f}, 1),  
            RectAC({560.f, 300.f}, {730.f, 320.f}, 1),  
            RectAC({290.f, 460.f}, {510.f, 480.f}, 1),  
        },
        {

            {40.f,  546.f, 3, 25.f, 0},  
            {350.f, 546.f, 3, 15.f, 0},  
            {630.f, 546.f, 3, 15.f, 0},  

            {55.f,  406.f, 2, 55.f, 0},


            {40.f,  80.f, 3, 85.f, 1},  
            {290.f, 80.f, 7, 95.f, 1},   
            {590.f, 80.f, 3, 85.f, 1},   


            {558.f, 286.f, 2, 65.f, 0},  
        },
        {
            {320.f, 445.f, false},
            {355.f, 445.f, true},   
            {390.f, 445.f, false},
            {425.f, 445.f, true},   
            {460.f, 445.f, false},
        },
        {

            {248.f, 80.f, 15.f, 340.f, 35.f},

            {517.f, 240.f, 15.f, 320.f, 35.f},
        },
        1000.f,
    },

};

inline int LEVEL_COUNT = 6;