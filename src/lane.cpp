// Header
#include "lane.hpp"

// stlib

#include <algorithm>

bool Lane::init(float world_scale)
{
	return true;
}

void Lane::destroy()
{
	m_cars.clear();
}

float Lane::get_time_remaining() const
{
    return m_time_remaining;
}

std::vector<Car> Lane::get_cars() const
{
    return m_cars;
}

void Lane::add_car(Car new_car)
{
	if (m_cars.size() < MaxCarsPerLane) {
		m_cars.push_back(new_car);
	} else {
		//throw error? How do I do that in C++...
	}
}

void Lane::remove_car()
{
	if (m_cars.size() > 0) {
		m_cars.erase(m_cars.begin());
	} else {
		//throw error...
	}
}

bool Lane::is_lane_full() const
{
	return m_cars.size() >= MaxCarsPerLane;
}
