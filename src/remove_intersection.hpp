#pragma once
#include "lane_manager.hpp"
#include "car.hpp"

class RemoveIntersection {
public:
	bool init();

	void remove_car_hit(std::vector<Car*> hit_cars);

};