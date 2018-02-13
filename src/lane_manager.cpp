#include "lane_manager.hpp"

bool LaneManager::init()
{
  m_lanes[direction::NORTH] = new Lane(direction::NORTH);
  m_lanes[direction::EAST] = new Lane(direction::EAST);
  m_lanes[direction::SOUTH] = new Lane(direction::SOUTH);
  m_lanes[direction::WEST] = new Lane(direction::WEST);
  lanes[0] = { 450.f,400.f };
  lanes[1] = { 400.f,540.f };
  lanes[2] = { 550.f,600.f };
  lanes[3] = { 600.f,450.f };

  m_time_remaining = m_time_per_action;

  return true;
}

void LaneManager::destroy()
{
  m_lanes.clear();
}

bool LaneManager::update(float ms)
{
	if (get_cars_in_lane(direction::WEST).size() > 0) {
		Car& new_car = get_cars_in_lane(direction::WEST).back();
		new_car.update(ms);
	}
	if (get_cars_in_lane(direction::SOUTH).size() > 0) {
		Car& new_car = get_cars_in_lane(direction::SOUTH).back();
		new_car.update(ms);
	}
	if (get_cars_in_lane(direction::NORTH).size() > 0) {
		Car& new_car = get_cars_in_lane(direction::NORTH).back();
		new_car.update(ms);
	}
	if (get_cars_in_lane(direction::EAST).size() > 0) {
		Car& new_car = get_cars_in_lane(direction::EAST).back();
		new_car.update(ms);
	}

  for(std::map<direction,Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++)
  {
    it->second->update(ms);
  }

  m_time_remaining -= ms;
  if (m_time_remaining <= 0)
  {
    this->add_car();
    m_time_remaining = m_time_per_action;
  }
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

std::vector<Car> LaneManager::get_cars_in_lane(direction dir) {
	if (dir == direction::NORTH) {
		return m_lanes[direction::NORTH]->get_cars();
	}
	else if (dir == direction::WEST) {
		return m_lanes[direction::WEST]->get_cars();
	}
	else if (dir == direction::EAST) {
		return m_lanes[direction::EAST]->get_cars();
	}
	else if (dir == direction::SOUTH) {
		return m_lanes[direction::SOUTH]->get_cars();
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
