#include "lane_manager.hpp"

bool LaneManager::init()
{
  m_lanes[direction::NORTH] = new Lane(direction::NORTH);
  m_lanes[direction::EAST] = new Lane(direction::EAST);
  m_lanes[direction::SOUTH] = new Lane(direction::SOUTH);
  m_lanes[direction::WEST] = new Lane(direction::WEST);
  lanes[0] = { 450.f,400.f };
  lanes[1] = { 400.f,540.f };
  lanes[2] = { 550.f,590.f };
  lanes[3] = { 760.f,450.f };

  m_time_remaining = m_time_per_action;

  return true;
}

void LaneManager::destroy()
{
  m_lanes.clear();
}

bool LaneManager::update(float ms)
{
	std::deque<Car> &cars = m_lanes[direction::WEST]->m_cars;
	//Finding out which is currently front
	int index = 0;
	if (!m_lanes[direction::WEST]->is_lane_empty()) {
		for (Car& car : cars) {
			if (car.is_in_beyond_intersec()) {
				++index;
			}
		}
	}
	bool occupied_front_boundary_box = false;

	for (Car& car : cars) {
		if (car.is_at_stop(lanes[1])) {
			occupied_front_boundary_box = true;
			break;
		}
		else {
			occupied_front_boundary_box = false;
		}
	}
	//if (occupied_front_boundary_box) {
	//	printf("true");
	//}
	//else {
	//	printf("false");
	//}
	for (int i = 0; i < cars.size(); i++) {
		cars[i].update(ms);
		//For all cars current in lane
		if (!cars[i].is_in_beyond_intersec()) {
			//If first car in queue
			if (&cars[i] == &cars[index]) {
				//If within stopping boundary
				if (cars[i].is_approaching_stop(lanes[1])) {
					//if has positive movement, slow down
					if (cars[i].get_acc().x > 0.f) {
						cars[i].slow_down();
					}
					//if has negative movement, move slowly to the end of lane
					else if (cars[i].get_acc().x < 0.f) {
						if (cars[i].get_vel().x <= 0) {
							if (!cars[i].is_at_stop(lanes[1])) {
								//need to figure out how to do this properly
								cars[i].move_slowly();
							}
						}
					}
				}
				//If not within the stopping boundary 
				else if (!cars[i].is_approaching_stop(lanes[1])) {
					//If negative movement, move
					if (cars[i].get_acc().x < 0.f) {
						if (cars[i].get_vel().x <= 0) {
							printf("move");
							cars[i].speed_up();
						}
					}
				}
			}
			//If not first car
			else if (&cars[i] != &cars[index]) {
				if (lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().x > 0.f) {
					printf("not ocuupied hit\n");
					cars[i].slow_down();
				}
				if (!lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().x < 0.f) {
					printf("not ocuupied  no collision\n");
					if (cars[i].get_vel().x <= 0) {
						cars[i].speed_up();
					}
				}
			}

		}
		if (car_delete(cars[i].get_position())) {
			m_lanes[direction::WEST]->m_cars.pop_front();
		}
	}

	for (Car& car : m_lanes[direction::NORTH]->m_cars) {
		if (car.is_approaching_stop(lanes[0]) && car.get_acc().y > 0.f)
		{
			car.slow_down();
		}
		car.update(ms);
		if (car_delete(car.get_position())) {
			m_lanes[direction::NORTH]->m_cars.pop_front();
		}
	}

	for (Car& car : m_lanes[direction::EAST]->m_cars) {
		if (car.is_approaching_stop(lanes[3]) && car.get_acc().x > 0.f)
		{
			car.slow_down();
		}
		car.update(ms);
		if (car_delete(car.get_position())) {
			m_lanes[direction::EAST]->m_cars.pop_front();
		}
	}

	for (Car& car : m_lanes[direction::SOUTH]->m_cars) {
		if (car.is_approaching_stop(lanes[2]) && car.get_acc().y > 0.f)
		{
			car.slow_down();
		}
		car.update(ms);
		if (car_delete(car.get_position())) {
			m_lanes[direction::SOUTH]->m_cars.pop_front();
		}

	}

	/*  for(std::map<direction,Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++)
	  {
		it->second->update(ms);
	  }

	  m_time_remaining -= ms;
	  if (m_time_remaining <= 0)
	  {
		this->add_car();
		m_time_remaining = m_time_per_action;
	  }
	  */
	return true;
}

void LaneManager::add_car()
{
  int longest_line = 0;
  for(std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++)
  {
    int cars_in_lane = it->second->get_cars().size();
    if (cars_in_lane > longest_line)
    {
      longest_line = cars_in_lane;
    }
  }

  for(std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++)
  {
    int cars_in_lane = it->second->get_cars().size();
    if ((cars_in_lane < longest_line || it->first == direction::WEST) && !it->second->is_lane_full())
    {
      it->second->add_car(carType::REGULAR);
      break;
    }
  }
}

std::deque<Car> LaneManager::get_cars_in_lane(direction dir) {
	if (dir == direction::NORTH) {
		return this->m_lanes[direction::NORTH]->get_cars();
	}
	else if (dir == direction::WEST) {
		return this->m_lanes[direction::WEST]->get_cars();
	}
	else if (dir == direction::EAST) {
		return this->m_lanes[direction::EAST]->get_cars();
	}
	else if (dir == direction::SOUTH) {
		return this->m_lanes[direction::SOUTH]->get_cars();
	}
}

void LaneManager::turn_car(direction dir)
{
	 m_lanes[dir]->turn_car();
}

//Temporary manual input to test before implementation of AI

void LaneManager::input_create_cars(direction dir) {
	m_lanes[dir]->add_car(carType::REGULAR);
}

bool LaneManager::car_delete(vec2 pos) {
	if (pos.x > 1100 && 518 < pos.y && pos.y < 590) {
		printf("destroy east");
		return true;
	}
	if (pos.x < -100 && 400 < pos.y && pos.y < 478) {
		printf("destroy west");
		return true;
	}
	if (pos.y < -100 && 518 < pos.x && pos.x < 591) {
		printf("destroy north");
		return true;
	}
	if (pos.y > 1100 && 407 < pos.x && pos.x < 485) {
		printf("destroy south");
		return true;
	}
	return false;
}
bool LaneManager::lane_collision_check(Car& current_car, Car& front_car) {
	//104 x margin distance away min
	//210 x margin start to slow
	float vel = current_car.get_vel().x;
	float bounding_box_distance = ((190 * vel / 200.f) + (150 * (1 - vel / 200.f)));
	float x_margin = abs(current_car.get_position().x - front_car.get_position().x);
	float y_margin = abs(current_car.get_position().y - front_car.get_position().y);
	//printf("%f%f\n", x_margin, y_margin);
	if (std::max(x_margin, y_margin) <= bounding_box_distance)
		return true;

	return false;
}