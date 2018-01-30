#pragma once

#include "common.hpp"
#include "car.hpp"

class Lane : public Renderable
{
public:
	const int MaxCarsPerLane = 5;

	// Creates instance
	bool init();

	// Releases instance
	void destroy();

	// Returns the current time remaining on the timer
    float get_time_remaining() const;

    // Returns the cars in the lane
    Car* get_cars() const;

private:
    Car* m_cars; // Cars in the lane
    float m_time_remaining; // Time remaining on timer
};