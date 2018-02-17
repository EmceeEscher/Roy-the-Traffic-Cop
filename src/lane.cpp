// Header
#include "lane.hpp"

#define PI 3.14159265

Lane::Lane(direction dir)
{
	m_dir = dir;
	m_time_remaining = m_max_time_per_car;
}


// Releases all graphics resources
bool Lane::init(direction dir)
{
	lanes_rot[0] = PI;			// North
	lanes_rot[1] = PI / 2.0;		// West
	lanes_rot[2] = 0;			// South
	lanes_rot[3] = 3.0*PI / 2.0;	// East
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

std::deque<Car> Lane::get_cars() const
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
			if (m_dir == direction::NORTH) {
				new_car.set_position({ 450.f,-100.f });
				new_car.set_rotation(PI / 2);
				new_car.set_lane(direction::NORTH);
			}
			else if (m_dir == direction::WEST) {
				new_car.set_position({ -100.f,537.f });
				new_car.set_lane(direction::WEST);
			}
			else if (m_dir == direction::EAST) {
				new_car.set_position({ 1100.f,445.f });
				new_car.set_rotation(PI);
				new_car.set_lane(direction::EAST);
			}
			else if (m_dir == direction::SOUTH) {
				new_car.set_position({ 550,1100.f });
				new_car.set_rotation(3.0*PI / 2.0);
				new_car.set_lane(direction::SOUTH);
			}
			m_cars.emplace_back(new_car);
			//new_car.enter_lane(direction dir); <-- function to animate moving car new up to previous car in line
		}
	}
}

void Lane::turn_car()
{
	//TODO Bug where references only the front car until it is popped out of screen.
	//This means that there is a delay in execution between the 1st car and 2nd car.
	if (!this->is_lane_empty()) {
		int index = 0;
		for (Car& car : m_cars) {
			if (car.is_in_beyond_intersec()) {
				++index;
			}
		}
		if (index < m_cars.size()) {
			printf("Index: %d\n", index);
			printf("Array Size: %d\n", m_cars.size());
			Car& new_car = m_cars.at(index);
			if (m_dir == direction::WEST || m_dir == direction::EAST) {
				if (new_car.get_vel().x <= 0.f) {
					new_car.signal_to_move();
					new_car.speed_up();
				}
			}

			if (m_dir == direction::NORTH || m_dir == direction::SOUTH) {
				if (new_car.get_vel().y <= 0.f) {
					new_car.signal_to_move();
					new_car.speed_up();
				}
			}
		}
		//wait...
		//this->erase_first_car();
		for(std::deque<Car>::iterator it = m_cars.begin(); it != m_cars.end(); it++)
		{
			//*it.advance(); //<-- tell remaining cars to move up in the lane
			//Do we need to tell them? I'm assuming that they will just move forward until a hit the previous car's collision
			//box, or until it's touch a stop collision box in which case turn car will then allow them to proceed through. How feasible is this?
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
	return m_cars.size() == 0;
}

void Lane::erase_first_car()
{
	m_cars.erase(m_cars.begin());
}
