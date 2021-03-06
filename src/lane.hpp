#pragma once

#include "common.hpp"
#include "car.hpp"
#include "direction.hpp"
#include "car_type.hpp"

#include <deque>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class Lane
{
public:
	const int MaxCarsPerLane = 4;
	float MaxTimePerCar = 10000; // Max time before a car will turn

	// constructor/destructor
	Lane(direction dir, float villainSpawnProbability);
	~Lane();

	// remove all cars from lane (used for resetting the game)
	void clear_lane();

	// Returns the current time remaining on the timer
	float get_time_remaining() const;

	// Sets the time remaining
	void set_time_remaining(float time_remaining);

	// Set stop sign location
	void set_stop_sign(vec2 loc);

	// Get stop sign location
	vec2 get_stop_sign()const;

	// Get the lane label number
	int get_lane_num()const;

	// Returns the cars in the lane
	std::deque<Car> get_cars() const;

	// Moves the game ahead by ms milliseconds
	bool update(float ms);

	// Adds the given car to the back of the lineup
	void add_car(carType type, int game_level);

	// Removes the car at the front of the lane
	void turn_car();

	// Returns true if the lane has MaxCarsPerLane cars in it
	bool is_lane_full() const;

	// Returns true if the lane has at least 1 vehicle in it
	bool is_lane_empty() const;

	// Erases the first car in the lane. (Call this after that car has finished turning)
	void erase_first_car();

	// Sets the desired direction for the car_indexthd car in m_cars
	void set_car_direction(direction dir, int car_index);

	std::deque<Car> m_cars; // Cars in the lane

	// Determine car's level depending on game level
	int determine_car_level_speed(int game_level);

	// Set the Lane's villain spawn probability
	void set_villain_probability(float prob);

	// Set the lane's max time per car
	void set_max_time_per_car(float ms);

private:
	float lanes_rot[4];
	float m_time_remaining; // Time remaining on timer of car at front of lane
	direction m_dir;
	float m_villain_spawn_probability; // Probability that a car spawned in this lane will be a villain
	std::map<direction, vec2> m_lane_coords;
	vec2 m_stop_sign_loc;
	int m_lane_num;
	bool car_is_honking;

	Mix_Chunk* m_car_horn;
	Mix_Chunk* m_car_rev;
};
