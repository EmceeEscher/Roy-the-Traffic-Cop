#pragma once

#include "lane.hpp"
#include "ai.hpp"
#include "car.hpp"
#include "direction.hpp"
#include "car_type.hpp"

#include <map>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class LaneManager
{
public:
	struct collisionTuple {
		int attacker_index;
		int victim_index;
	};
	const float VillainSpawnProbability = 0.25; // We may want to make this level dependent in the future. Revise when levels are added.


    //initializes 4 empty lanes
    //(TODO: pass desired number/type of lanes as parameter? or write a different init function per level?)
    bool init(AI ai);

    // Releases instance
    void destroy();

    // Moves the game ahead by ms milliseconds
    bool update(float ms);

    // Will try to add a car that will be frustrating for player
    void add_car();

		bool car_delete(vec2 pos);

		std::deque<Car> get_cars_in_lane(direction dir);

    // Will tell the first car in the lane in direction dir to turn
    void turn_car(direction dir);

		void input_create_cars(direction dir);

		int points();

		bool lane_collision_check(Car& current_car, Car& front_car);

		bool lane_queue(Lane* lane, vec2 lane_intersection,float ms);

		// Will return true if any cars in the process of turning are colliding
		bool intersection_collision_check();

		// Doesn't need boolean return. If intersection_collision_check() is true, there must be a mesh collision. 
		LaneManager::collisionTuple mesh_collision_check(Car* first_car, Car* second_car);

		std::map<direction, Lane*> m_lanes;


private:
    float const m_time_per_action = 5000;
    float m_time_remaining;
	AI* m_ai;
	std::map<direction, vec2> m_lane_coords;
	unsigned int m_points;
	
	Mix_Chunk* m_crash;
};
