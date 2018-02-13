// Header
#include "lane.hpp"

Lane::Lane(direction dir)
{
	m_dir = dir;
	m_time_remaining = m_max_time_per_car;
}


// Releases all graphics resources
bool Lane::init(direction dir)
{
	m_dir = dir;
	return true;
}

Lane::~Lane()
{
	m_cars.clear();
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

std::vector<Car> Lane::get_cars() const
{
    return m_cars;
}

bool Lane::update(float ms)
{
	m_time_remaining -= ms;
	if (m_time_remaining <= 0)
	{
		this->turn_car();
		m_time_remaining = m_max_time_per_car;
	}
	return true;
}

void Lane::add_car(carType type)
{
	if (this->is_lane_full()) {
		// change following code based on carType once we have more than one
		fprintf(stderr, "lane is full");
	}
	else{
		Car new_car;
		if(new_car.init()){
			m_cars.emplace_back(new_car);
			//new_car.enter_lane(direction dir); <-- function to animate moving car new up to previous car in line
		}
	}
}

void Lane::turn_car()
{
	if (!this->is_lane_empty()) {
		//m_cars[0].turn(); //tell the car at the front of the lane to turn
		//wait...
		this->erase_first_car();
		for(std::vector<Car>::iterator it = m_cars.begin(); it != m_cars.end(); it++)
		{
			//*it.advance(); //<-- tell remaining cars to move up in the lane
		}
	}
}

bool Lane::is_lane_full() const
{
	printf("%zu", m_cars.size());
	return m_cars.size() >= MaxCarsPerLane;
}

bool Lane::is_lane_empty() const
{
	return m_cars.size() > 0;
}

void Lane::erase_first_car()
{
	m_cars.erase(m_cars.begin());
}
