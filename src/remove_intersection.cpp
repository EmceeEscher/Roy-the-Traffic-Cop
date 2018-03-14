#include "remove_intersection.hpp"
#include "car.hpp"
#include "lane_manager.hpp"


bool RemoveIntersection::init()
{
	return true;
}

void RemoveIntersection::remove_car_hit(std::vector<Car*> hit_cars) {
	for (int i = 0; i < hit_cars.size(); i++) {
		if (hit_cars[i]->is_hit) {

		}
	}
}