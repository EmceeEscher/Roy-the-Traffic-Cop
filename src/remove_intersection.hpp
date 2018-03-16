#pragma once
#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class RemoveIntersection {
public:
	RemoveIntersection();
	~RemoveIntersection();

	bool init();

	void check_lanes(std::map<direction, Lane*> m_lanes);

private:
	std::vector<Car*> cars_in_intersec;

};