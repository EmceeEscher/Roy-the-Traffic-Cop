#pragma once

#include "common.hpp"
#include "car.hpp"

#include <vector>

class Lane : public Renderable
{
public:
	const int MaxCarsPerLane = 4;

	// Creates instance
	bool init(float world_scale);

	// Releases instance
	void destroy();

	// Returns the current time remaining on the timer
    float get_time_remaining() const;

    // Returns the cars in the lane
    std::vector<Car> get_cars() const;

    // Adds the given car to the back of the lineup
    void add_car(Car new_car);

    // Removes the car at the front of the lane
    void remove_car();

    // Returns true if the lane has MaxCarsPerLane cars in it
    bool is_lane_full() const;

private:
    std::vector<Car> m_cars; // Cars in the lane
    float m_time_remaining; // Time remaining on timer of car at front of lane
};
