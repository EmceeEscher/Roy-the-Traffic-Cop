// Header
#include "lane.hpp"

// stlib
#include <vector>
#include <algorithm>

// Releases all graphics resources
bool Lane::init()
{
	m_cars = new Car[MaxCarsPerLane];
	return true;
}

void Lane::destroy()
{
	delete m_cars;
}

float Lane::get_time_remaining() const
{
    return m_time_remaining;
}

Car* Lane::get_cars() const
{
    return m_cars;
}

