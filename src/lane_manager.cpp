#include "lane_manager.hpp"

bool LaneManager::init(AI ai)
{
  m_lanes[direction::NORTH] = new Lane(direction::NORTH, VillainSpawnProbability);
  m_lanes[direction::EAST] = new Lane(direction::EAST, VillainSpawnProbability);
  m_lanes[direction::SOUTH] = new Lane(direction::SOUTH, VillainSpawnProbability);
  m_lanes[direction::WEST] = new Lane(direction::WEST, VillainSpawnProbability);

  m_time_remaining = m_time_per_action;

  m_ai = &ai;

  return true;
}

void LaneManager::destroy()
{
  m_lanes.clear();
}

bool LaneManager::update(float ms)
{
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

void LaneManager::turn_car(direction dir)
{
  m_lanes[dir]->turn_car();
  m_ai->make_villains_decide(m_lanes);
}
