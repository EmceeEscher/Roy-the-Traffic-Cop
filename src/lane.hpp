#pragma once

#include "common.hpp"
#include "car.hpp"
#include "direction.hpp"
#include "car_type.hpp"

#include <vector>

class Lane
{
public:
	const int MaxCarsPerLane = 4;

    // constructor/destructor
    Lane(float world_scale, direction dir);
    ~Lane();

	// Returns the current time remaining on the timer
    float get_time_remaining() const;

    // Returns the cars in the lane
    std::vector<Car> get_cars() const;

    // Moves the game ahead by ms milliseconds
    bool update(float ms);

    // Adds the given car to the back of the lineup
    void add_car(carType type);

    // Removes the car at the front of the lane
    void turn_car();

    // Returns true if the lane has MaxCarsPerLane cars in it
    bool is_lane_full() const;

    // Returns true if the lane has at least 1 vehicle in it
    bool is_lane_empty() const;

    // Erases the first car in the lane. (Call this after that car has finished turning)
    void erase_first_car();

private:
    std::vector<Car> m_cars; // Cars in the lane
    float const m_max_time_per_car = 20000; // Max time before a car will turn
    float m_time_remaining; // Time remaining on timer of car at front of lane
    float m_world_scale;
    direction m_dir;
};
