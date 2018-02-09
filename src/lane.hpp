#pragma once

#include "common.hpp"
#include "car.hpp"

class Lane : public Renderable
{
public:
	const int MaxCarsPerLane = 5;

	// Creates instance
	bool init(int num);

	// Releases instance
	void destroy();

	// Returns the current time remaining on the timer
    float get_time_remaining() const;

	// Set stop sign location
	void set_stop_sign(vec2 loc);

	// Get stop sign location
	vec2 get_stop_sign()const;

	// Get the lane label number
	int get_lane_num()const;

    // Returns the cars in the lane
    Car* get_cars() const;

private:
    Car* m_cars; // Cars in the lane
    float m_time_remaining; // Time remaining on timer
	vec2 m_stop_sign_loc;
	int m_lane_num;
};