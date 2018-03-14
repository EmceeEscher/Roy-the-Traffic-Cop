#include "lane_manager.hpp"
#include "car.hpp"
//DEBUG
//#include <windows.h>



bool LaneManager::init(AI ai)
{
  m_lanes[direction::NORTH] = new Lane(direction::NORTH, VillainSpawnProbability);
  m_lanes[direction::EAST] = new Lane(direction::EAST, VillainSpawnProbability);
  m_lanes[direction::SOUTH] = new Lane(direction::SOUTH, VillainSpawnProbability);
  m_lanes[direction::WEST] = new Lane(direction::WEST, VillainSpawnProbability);
	m_lane_coords[direction::NORTH] = { 450.f,398.f };
	m_lane_coords[direction::EAST] = { 400.f,540.f };
	m_lane_coords[direction::SOUTH] = { 550.f,590.f };
	m_lane_coords[direction::WEST] = { 610.f,450.f };

  m_time_remaining = m_time_per_action;
  m_points = 0;
  m_ai = &ai;

  return true;
}

void LaneManager::destroy()
{
  m_lanes.clear();
}

bool LaneManager::update(float ms)
{

	//For loop for all the lanes if m_is_is_beyond_intersection is true, then add to vector/list/deque to check for collisions between those cars

	if (lane_queue(m_lanes[direction::NORTH], m_lane_coords[direction::NORTH], ms) ||
		lane_queue(m_lanes[direction::WEST], m_lane_coords[direction::EAST], ms) ||
		lane_queue(m_lanes[direction::SOUTH], m_lane_coords[direction::SOUTH], ms) ||
		lane_queue(m_lanes[direction::EAST], m_lane_coords[direction::WEST], ms)) {
		// lane_queue returns true if its time has expired.
		// If this is the case, we should readjust new villains.
		m_ai->make_villains_decide(m_lanes);
	}

	add_car();
	intersection_collision_check();
	return true;
}

bool LaneManager::intersection_collision_check() {
	std::vector<Car*> cars_in_intersec;
	bool collision_occurring = false;

	for(std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++) {
		std::deque<Car> &curr_cars = it->second->m_cars;
		if (curr_cars.size() > 0) {
			Car* first_car = &(curr_cars[0]);
			if (first_car->is_in_beyond_intersec()) {
				cars_in_intersec.push_back(first_car);
			}
		}
	}

	for (int i = 0; i < cars_in_intersec.size(); i++) {
		Car* first_car = cars_in_intersec[i];
		rect_bounding_box first_bb = first_car->get_bounding_box();

		for (int j = i + 1; j < cars_in_intersec.size(); j++) {
			Car* second_car = cars_in_intersec[j];
			rect_bounding_box second_bb = second_car->get_bounding_box();
			if (first_car->check_collision(second_bb.bottom_left)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(second_car, first_car, second_bb.bottom_left);
				if (collided_triangle != -1) {
					first_car->collided(collided_triangle);
					second_car->collided(-1);
					printf("first car getting hit by second bottom left\n");
					//first_car->change_color();
				}
				//DEBUG


				//TODO: If mesh_collision_check != -1, apply collision depending on triangle returned
			}
			else if (first_car->check_collision(second_bb.bottom_right)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(second_car, first_car, second_bb.bottom_right);
				if (collided_triangle != -1) {
					first_car->collided(collided_triangle);
					second_car->collided(-1);
					printf("first car getting hit by second bottom right\n");
					//first_car->change_color();
				}
			}
			else if (first_car->check_collision(second_bb.top_left)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(second_car, first_car, second_bb.top_left);
				if (collided_triangle != -1) {
					first_car->collided(collided_triangle);
					second_car->collided(-1);
					printf("first car getting hit by second top left\n");
					//first_car->change_color();
				}
			}
			else if (first_car->check_collision(second_bb.top_right)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(second_car, first_car, second_bb.top_right);
				if (collided_triangle != -1) {
					first_car->collided(collided_triangle);
					second_car->collided(-1);
					printf("first car getting hit by second top right\n");
					//first_car->change_color();
				}
			}

			else if (second_car->check_collision(first_bb.bottom_left)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(first_car, second_car, first_bb.bottom_left);
				if (collided_triangle != -1) {
					first_car->collided(-1);
					second_car->collided(collided_triangle);
					printf("second car getting hit by first bottom left\n");
					//first_car->change_color();
				}
			}
			else if (second_car->check_collision(first_bb.bottom_right)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(first_car, second_car, first_bb.bottom_right);
				if (collided_triangle != -1) {
					first_car->collided(-1);
					second_car->collided(collided_triangle);
					printf("second car getting hit by first bottom right\n");
					//first_car->change_color();
				}
			}
			else if (second_car->check_collision(first_bb.top_right)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(first_car, second_car, first_bb.top_right);
				if (collided_triangle != -1) {
					first_car->collided(-1);
					second_car->collided(collided_triangle);
					printf("second car getting hit by first top right\n");
					//first_car->change_color();
				}
			}
			else if (second_car->check_collision(first_bb.top_left)) {
				collision_occurring = true;
				int collided_triangle = mesh_collision_check(first_car, second_car, first_bb.top_left);
				if (collided_triangle != -1) {
					first_car->collided(-1);
					second_car->collided(collided_triangle);
					printf("second car getting hit by first top left\n");
					//first_car->change_color();
				}
			}
		}
	}

	return collision_occurring;
}

int LaneManager::mesh_collision_check(Car* attacker_car, Car* victim_car, vec2 impact_vertex) {

	Car::Triangle victim_triangles[14];

	victim_triangles[0].a = victim_car->get_vertex_pos(0);
	victim_triangles[0].b = victim_car->get_vertex_pos(1);
	victim_triangles[0].c = victim_car->get_vertex_pos(2);

	victim_triangles[1].a = victim_car->get_vertex_pos(1);
	victim_triangles[1].b = victim_car->get_vertex_pos(3);
	victim_triangles[1].c = victim_car->get_vertex_pos(2);

	victim_triangles[2].a = victim_car->get_vertex_pos(0);
	victim_triangles[2].b = victim_car->get_vertex_pos(4);
	victim_triangles[2].c = victim_car->get_vertex_pos(1);

	victim_triangles[3].a = victim_car->get_vertex_pos(1);
	victim_triangles[3].b = victim_car->get_vertex_pos(10);
	victim_triangles[3].c = victim_car->get_vertex_pos(3);

	victim_triangles[4].a = victim_car->get_vertex_pos(3);
	victim_triangles[4].b = victim_car->get_vertex_pos(10);
	victim_triangles[4].c = victim_car->get_vertex_pos(12);

	victim_triangles[5].a = victim_car->get_vertex_pos(1);
	victim_triangles[5].b = victim_car->get_vertex_pos(4);
	victim_triangles[5].c = victim_car->get_vertex_pos(10);

	victim_triangles[6].a = victim_car->get_vertex_pos(4);
	victim_triangles[6].b = victim_car->get_vertex_pos(11);
	victim_triangles[6].c = victim_car->get_vertex_pos(10);

	victim_triangles[7].a = victim_car->get_vertex_pos(5);
	victim_triangles[7].b = victim_car->get_vertex_pos(12);
	victim_triangles[7].c = victim_car->get_vertex_pos(10);

	victim_triangles[8].a = victim_car->get_vertex_pos(10);
	victim_triangles[8].b = victim_car->get_vertex_pos(6);
	victim_triangles[8].c = victim_car->get_vertex_pos(5);

	victim_triangles[9].a = victim_car->get_vertex_pos(11);
	victim_triangles[9].b = victim_car->get_vertex_pos(9);
	victim_triangles[9].c = victim_car->get_vertex_pos(10);

	victim_triangles[10].a = victim_car->get_vertex_pos(10);
	victim_triangles[10].b = victim_car->get_vertex_pos(9);
	victim_triangles[10].c = victim_car->get_vertex_pos(6);

	victim_triangles[11].a = victim_car->get_vertex_pos(5);
	victim_triangles[11].b = victim_car->get_vertex_pos(6);
	victim_triangles[11].c = victim_car->get_vertex_pos(7);

	victim_triangles[12].a = victim_car->get_vertex_pos(6);
	victim_triangles[12].b = victim_car->get_vertex_pos(8);
	victim_triangles[12].c = victim_car->get_vertex_pos(7);

	victim_triangles[13].a = victim_car->get_vertex_pos(6);
	victim_triangles[13].b = victim_car->get_vertex_pos(9);
	victim_triangles[13].c = victim_car->get_vertex_pos(8);

	Car::Triangle attacker_triangles[14];

	attacker_triangles[0].a = attacker_car->get_vertex_pos(0);
	attacker_triangles[0].b = attacker_car->get_vertex_pos(1);
	attacker_triangles[0].c = attacker_car->get_vertex_pos(2);

	attacker_triangles[1].a = attacker_car->get_vertex_pos(1);
	attacker_triangles[1].b = attacker_car->get_vertex_pos(3);
	attacker_triangles[1].c = attacker_car->get_vertex_pos(2);

	attacker_triangles[2].a = attacker_car->get_vertex_pos(0);
	attacker_triangles[2].b = attacker_car->get_vertex_pos(4);
	attacker_triangles[2].c = attacker_car->get_vertex_pos(1);

	attacker_triangles[3].a = attacker_car->get_vertex_pos(1);
	attacker_triangles[3].b = attacker_car->get_vertex_pos(10);
	attacker_triangles[3].c = attacker_car->get_vertex_pos(3);

	attacker_triangles[4].a = attacker_car->get_vertex_pos(3);
	attacker_triangles[4].b = attacker_car->get_vertex_pos(10);
	attacker_triangles[4].c = attacker_car->get_vertex_pos(12);

	attacker_triangles[5].a = attacker_car->get_vertex_pos(1);
	attacker_triangles[5].b = attacker_car->get_vertex_pos(4);
	attacker_triangles[5].c = attacker_car->get_vertex_pos(10);

	attacker_triangles[6].a = attacker_car->get_vertex_pos(4);
	attacker_triangles[6].b = attacker_car->get_vertex_pos(11);
	attacker_triangles[6].c = attacker_car->get_vertex_pos(10);

	attacker_triangles[7].a = attacker_car->get_vertex_pos(5);
	attacker_triangles[7].b = attacker_car->get_vertex_pos(12);
	attacker_triangles[7].c = attacker_car->get_vertex_pos(10);

	attacker_triangles[8].a = attacker_car->get_vertex_pos(10);
	attacker_triangles[8].b = attacker_car->get_vertex_pos(6);
	attacker_triangles[8].c = attacker_car->get_vertex_pos(5);

	attacker_triangles[9].a = attacker_car->get_vertex_pos(11);
	attacker_triangles[9].b = attacker_car->get_vertex_pos(9);
	attacker_triangles[9].c = attacker_car->get_vertex_pos(10);

	attacker_triangles[10].a = attacker_car->get_vertex_pos(10);
	attacker_triangles[10].b = attacker_car->get_vertex_pos(9);
	attacker_triangles[10].c = attacker_car->get_vertex_pos(6);

	attacker_triangles[11].a = attacker_car->get_vertex_pos(5);
	attacker_triangles[11].b = attacker_car->get_vertex_pos(6);
	attacker_triangles[11].c = attacker_car->get_vertex_pos(7);

	attacker_triangles[12].a = attacker_car->get_vertex_pos(6);
	attacker_triangles[12].b = attacker_car->get_vertex_pos(8);
	attacker_triangles[12].c = attacker_car->get_vertex_pos(7);

	attacker_triangles[13].a = attacker_car->get_vertex_pos(6);
	attacker_triangles[13].b = attacker_car->get_vertex_pos(9);
	attacker_triangles[13].c = attacker_car->get_vertex_pos(8);

	// Determine victim_car's triangle coordinates
	// Determine which corner of first_car bounding box hit second_car bounding box
	// For each triangle, check if impact corner is inside
	// TODO: Depending on which triangle gets hit first, call different collision responses on victim_car

	int vic_counter = 0;
	int attack_counter;
	for (Car::Triangle victim_tri : victim_triangles) {
		//printf("Triangle %i (%f, %f) (%f,%f) (%f,%f) Area %f Point hitting (%f, %f) \n", counter, t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y, first_car->get_triangle_area(victim_triangles[3].a, victim_triangles[3].b, victim_triangles[3].c), impact_vertex.x, impact_vertex.y);

		attack_counter = 0;
		for (Car::Triangle attacker_tri : attacker_triangles) {
			if (victim_car->check_mesh_collision(attacker_tri.a, victim_tri)
				|| victim_car->check_mesh_collision(attacker_tri.b, victim_tri)
			  || victim_car->check_mesh_collision(attacker_tri.c, victim_tri)) {
					printf("triangle vic: %i, attack: %i hit\n", vic_counter, attack_counter);
					return vic_counter;
				}
				attack_counter++;
		}
		// if (victim_car->check_mesh_collision(impact_vertex, victim_tri)) {
		// 	printf("triangle %i hit\n", counter);
		// //	DEBUG to use Sleep, you need to include <windows.h>
		// //	Sleep(1000);
		// 	return counter;
		// }
		vic_counter++;
	}
	return -1;
}

void LaneManager::add_car()
{
  for(std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++)
  {
    if (!it->second->is_lane_full())
    {
      it->second->add_car(carType::REGULAR);
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
  m_lanes[dir]->set_time_remaining(m_lanes[dir]->MaxTimePerCar);
  m_ai->make_villains_decide(m_lanes);
}

//Temporary manual input to test before implementation of AI

void LaneManager::input_create_cars(direction dir) {
	m_lanes[dir]->add_car(carType::REGULAR);
}

bool LaneManager::car_delete(vec2 pos) {
	if (pos.x > 1100 && 518 < pos.y && pos.y < 590) {
		printf("destroy east\n");
		return true;
	}
	if (pos.x < -100 && 400 < pos.y && pos.y < 478) {
		printf("destroy west\n");
		return true;
	}
	if (pos.y < -100 && 518 < pos.x && pos.x < 591) {
		printf("destroy north\n");
		return true;
	}
	if (pos.y > 1100 && 407 < pos.x && pos.x < 485) {
		printf("destroy south\n");
		return true;
	}
	return false;
}

int LaneManager::points() {
	return m_points;
}
bool LaneManager::lane_collision_check(Car& current_car, Car& front_car) {
	//104 x margin distance away min
	//210 x margin start to slow
	//TODO
	float vel = current_car.get_vel().x;

	if (current_car.get_lane() == direction::NORTH || current_car.get_lane() == direction::SOUTH) {
		vel = current_car.get_vel().y;
	}
	float bounding_box_distance = ((200.f * vel / 200.f) + (160.f * (1.f - vel / 200.f)));
	float x_margin = abs(current_car.get_position().x - front_car.get_position().x);
	float y_margin = abs(current_car.get_position().y - front_car.get_position().y);
	if (std::max(x_margin, y_margin) <= bounding_box_distance)
		return true;

	return false;
}

bool LaneManager::lane_queue(Lane* lane, vec2 lane_intersection, float ms) {
	lane->update(ms);
	bool time_expired = lane->get_time_remaining() <= 0;
	if (time_expired)
	{
		lane->turn_car();
		lane->set_time_remaining(lane->MaxTimePerCar);
	}
	std::deque<Car> &cars = lane->m_cars;
	//Finding out which is currently front
	bool occupied_front_boundary_box = false;

	for (Car& car : cars) {
		if (car.is_at_stop(lane_intersection)) {
			occupied_front_boundary_box = true;
			break;
		}
		else {
			occupied_front_boundary_box = false;
		}
	}

	for (int i = 0; i < cars.size(); i++) {
		//car being updated
		cars[i].update(ms);
		//Had to nest the for loop to prevent deque o
		int index = 0;
		if (!lane->is_lane_empty()) {
			for (Car& car : cars) {
				if (car.is_in_beyond_intersec()) {
					++index;
				}
			}
		}
		//For all cars current in lane
		if (!cars[i].is_in_beyond_intersec()) {
			if (lane == m_lanes[direction::WEST] || lane == m_lanes[direction::EAST]) {
				//If first car in queue
				if (&cars[i] == &cars[index]) {
					//If within stopping boundary
					if (cars[i].is_approaching_stop(lane_intersection)) {
						//if has positive movement, slow down
						if (cars[i].get_acc().x > 0.f) {
							cars[i].slow_down();
						}
						//if has negative movement, move slowly to the end of lane
						else if (cars[i].get_acc().x < 0.f) {
							if (cars[i].get_vel().x <= 0) {
								if (!occupied_front_boundary_box) {
									cars[i].speed_up();
								}
							}
						}
					}
					//If not within the stopping boundary
					else if (!cars[i].is_approaching_stop(lane_intersection)) {
						//If negative movement, move
						if (cars[i].get_acc().x < 0.f) {
							if (cars[i].get_vel().x <= 0) {
								if (!occupied_front_boundary_box) {
									cars[i].speed_up();
								}
							}
						}
					}
				}
				//If not first car
				else if (&cars[i] != &cars[index]) {
					if (lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().x > 0.f) {
						cars[i].slow_down();
					}
					if (!lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().x < 0.f) {
						if (cars[i].get_vel().x <= 0) {
							cars[i].speed_up();
						}
					}
				}
			}
			else if (lane == m_lanes[direction::SOUTH] || lane == m_lanes[direction::NORTH]) {
				//If first car in queue
				if (&cars[i] == &cars[index]) {
					//If within stopping boundary
					if (cars[i].is_approaching_stop(lane_intersection)) {
						//if has positive movement, slow down
						if (cars[i].get_acc().y > 0.f) {
							cars[i].slow_down();
						}
						//if has negative movement, move slowly to the end of lane
						else if (cars[i].get_acc().y < 0.f) {
							if (cars[i].get_vel().y <= 0) {
								if (!occupied_front_boundary_box) {
									cars[i].speed_up();
								}
							}
						}
					}
					//If not within the stopping boundary
					else if (!cars[i].is_approaching_stop(lane_intersection)) {
						//If negative movement, move
						if (cars[i].get_acc().y < 0.f) {
							if (cars[i].get_vel().y <= 0) {
								if (!occupied_front_boundary_box) {
									cars[i].speed_up();
								}
							}
						}
					}
				}
				//If not first car
				else if (&cars[i] != &cars[index]) {
					if (lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().y > 0.f) {
						cars[i].slow_down();
					}
					if (!lane_collision_check(cars[i], cars[i - 1]) && cars[i].get_acc().y < 0.f) {
						if (cars[i].get_vel().y <= 0) {
							cars[i].speed_up();
						}
					}
				}
			}
		}
		if (car_delete(cars[i].get_position())) {
			lane->m_cars.erase(lane->m_cars.begin() + i);
			++m_points;
		}
	}

	return time_expired;
}
