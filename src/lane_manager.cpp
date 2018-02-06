#include "lane_manager.hpp"

bool LaneManager::init(float world_scale)
{
  lanes[direction::NORTH] = new Lane(world_scale, direction::NORTH);
  lanes[direction::EAST] = new Lane(world_scale, direction::EAST);
  lanes[direction::SOUTH] = new Lane(world_scale, direction::SOUTH);
  lanes[direction::WEST] = new Lane(world_scale, direction::WEST);

  return true;
}

void LaneManager::destroy()
{
  lanes.clear();
}

void LaneManager::add_car()
{
  //TODO: this function
}

void LaneManager::turn_car(direction dir)
{
  lanes[dir]->turn_car();
}
