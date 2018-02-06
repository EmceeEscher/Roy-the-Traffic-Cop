#pragma once

#include "lane.hpp"
#include "car.hpp"
#include "direction.hpp"

#include <map>

class LaneManager
{
public:
    //initializes 4 empty lanes
    //(TODO: pass desired number/type of lanes as parameter? or write a different init function per level?)
    bool init(float world_scale);

    // Releases instance
    void destroy();

    // Moves the game ahead by ms milliseconds
    bool update(float ms);

    // Will try to add a car that will be frustrating for player
    void add_car();

    // Will tell the first car in the lane in direction dir to turn
    void turn_car(direction dir);

private:
    std::map<direction, Lane*> m_lanes;
    float const m_time_per_action = 5000;
    float m_time_remaining;
};
