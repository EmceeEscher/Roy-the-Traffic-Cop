// Header
#include "lane.hpp"

// stlib
#include <vector>
#include <algorithm>

// Releases all graphics resources
bool Lane::init(int num)
{
	m_cars = new Car[MaxCarsPerLane];
	m_lane_num = num;
	return true;
}

void Lane::destroy()
{
	delete m_cars;
}

int Lane::get_lane_num()const
{
	return m_lane_num;
}

float Lane::get_time_remaining() const
{
    return m_time_remaining;
}

void Lane::set_stop_sign(vec2 loc)
{
	float x = loc.x;
	float y = loc.y;
	m_stop_sign_loc.x = x;
	m_stop_sign_loc.y = y;
}

vec2 Lane::get_stop_sign()const
{
	return m_stop_sign_loc;
}

Car* Lane::get_cars() const
{
    return m_cars;
}

