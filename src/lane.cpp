// Header
#include "lane.hpp"

Lane::Lane(float world_scale, direction dir)
{
	m_world_scale = world_scale;
	m_dir = dir;
}

Lane::~Lane()
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

void Lane::turn_car()
{
	if (m_cars.size() > 0) {
		//m_cars[0].turn(); //tell the car at the front of the lane to turn
		m_cars.erase(m_cars.begin()); //Will this work? have to be careful that it won't delete the car mid-turn. Might need to like pass this as a callback or something...
	} else {
		//throw error...
	}
}

bool Lane::is_lane_full() const
{
	return m_cars.size() >= MaxCarsPerLane;
}
