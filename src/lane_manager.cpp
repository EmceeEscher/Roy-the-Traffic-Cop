#include "lane_manager.hpp"

bool LaneManager::init(float world_scale)
{
  m_lanes[direction::NORTH] = new Lane(world_scale, direction::NORTH);
  m_lanes[direction::EAST] = new Lane(world_scale, direction::EAST);
  m_lanes[direction::SOUTH] = new Lane(world_scale, direction::SOUTH);
  m_lanes[direction::WEST] = new Lane(world_scale, direction::WEST);

  m_time_remaining = m_time_per_action;

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
}

void LaneManager::add_car()
{
  //TODO: this function
}

void LaneManager::turn_car(direction dir)
{
  m_lanes[dir]->turn_car();
}
