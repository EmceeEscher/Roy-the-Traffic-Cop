#include "lane_manager.hpp"
#include "car.hpp"
#include <windows.h>



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

	lane_queue(m_lanes[direction::NORTH], m_lane_coords[direction::NORTH], ms);
	lane_queue(m_lanes[direction::WEST], m_lane_coords[direction::EAST], ms);
	lane_queue(m_lanes[direction::SOUTH], m_lane_coords[direction::SOUTH], ms);
	lane_queue(m_lanes[direction::EAST], m_lane_coords[direction::WEST], ms);

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
			//printf("first_car getting hit! indices: %u vertex pos: (%f,%f) m_position: (%f, %f)\n", first_car->get_index(4), first_car->get_vertex_pos(5).x, first_car->get_vertex_pos(0).y, first_car->get_position().x, first_car->get_position().y);
			// TODO: if this happens, check triangles in mesh, then give car new velocity
			// should this return the triangles which connected?
			if (first_car->check_collision(second_bb.bottom_left)) {
				printf("first car getting hit by second bottom left\n");
				collision_occurring = true;
				mesh_collision_check(second_car, first_car, second_bb.bottom_left);
				first_car->change_color();
			}
			if (first_car->check_collision(second_bb.bottom_right)) {
				printf("first car getting hit by second bottom right\n");
				collision_occurring = true;
				mesh_collision_check(second_car, first_car, second_bb.bottom_right);
				first_car->change_color();
			}
			if (first_car->check_collision(second_bb.top_left)) {
				printf("first car getting hit by second top left\n");
				collision_occurring = true;
				mesh_collision_check(second_car, first_car, second_bb.top_left);
				first_car->change_color();
			}
			if (first_car->check_collision(second_bb.top_right)) {
				printf("first car getting hit by second top right\n");
				collision_occurring = true;
				mesh_collision_check(second_car, first_car, second_bb.top_right);
				first_car->change_color();
			}

			if (second_car->check_collision(first_bb.bottom_left)) {
				printf("second car getting hit by first bottom left\n");
				collision_occurring = true;
				mesh_collision_check(first_car, second_car, first_bb.bottom_left);
				first_car->change_color();
			}
			if (second_car->check_collision(first_bb.bottom_right)) {
				printf("second car getting hit by first bottom right\n");
				collision_occurring = true;
				mesh_collision_check(first_car, second_car, first_bb.bottom_right);
				first_car->change_color();
			}
			if (second_car->check_collision(first_bb.top_right)) {
				printf("second car getting hit by first top right\n");
				collision_occurring = true;
				mesh_collision_check(first_car, second_car, first_bb.top_right);
				first_car->change_color();
			}
			if (second_car->check_collision(first_bb.top_left)) {
				printf("second car getting hit by first top left\n");
				collision_occurring = true;
				mesh_collision_check(first_car, second_car, first_bb.top_left);
				first_car->change_color();
				// TODO: if mesh_collision happens, then give car new velocity
			}
		}
	}

	return collision_occurring;
}

int LaneManager::mesh_collision_check(Car* attacker_car, Car* victim_car, vec2 impact_vertex) {
	// ??? DOES this need the intersection point from second_car as input?
	// for each triangle in first_Car
	//		calculate area1 (points a,b,c)
	//		calculate area2, sum of 3 triangles with intersection point (abd, adc, bdc)
	//		if (area1 == area2), return (?) this triangle
	// BEST way to find out which part of first_car hit?
	Car::Triangle triangles[10];

	triangles[0].a = victim_car->get_vertex_pos(0);
	triangles[0].b = victim_car->get_vertex_pos(1);
	triangles[0].c = victim_car->get_vertex_pos(2);

	triangles[1].a = victim_car->get_vertex_pos(1);
	triangles[1].b = victim_car->get_vertex_pos(2);
	triangles[1].c = victim_car->get_vertex_pos(3);

	triangles[2].a = victim_car->get_vertex_pos(0);
	triangles[2].b = victim_car->get_vertex_pos(1);
	triangles[2].c = victim_car->get_vertex_pos(4);

	triangles[3].a = victim_car->get_vertex_pos(3);
	triangles[3].b = victim_car->get_vertex_pos(4);
	triangles[3].c = victim_car->get_vertex_pos(5);

	triangles[4].a = victim_car->get_vertex_pos(3);
	triangles[4].b = victim_car->get_vertex_pos(5);
	triangles[4].c = victim_car->get_vertex_pos(6);

	triangles[5].a = victim_car->get_vertex_pos(5);
	triangles[5].b = victim_car->get_vertex_pos(6);
	triangles[5].c = victim_car->get_vertex_pos(7);

	triangles[6].a = victim_car->get_vertex_pos(6);
	triangles[6].b = victim_car->get_vertex_pos(7);
	triangles[6].c = victim_car->get_vertex_pos(8);

	triangles[7].a = victim_car->get_vertex_pos(8);
	triangles[7].b = victim_car->get_vertex_pos(9);
	triangles[7].c = victim_car->get_vertex_pos(10);

	triangles[8].a = victim_car->get_vertex_pos(7);
	triangles[8].b = victim_car->get_vertex_pos(9);
	triangles[8].c = victim_car->get_vertex_pos(11);

	triangles[9].a = victim_car->get_vertex_pos(9);
	triangles[9].b = victim_car->get_vertex_pos(10);
	triangles[9].c = victim_car->get_vertex_pos(11);

	// Determine victim_car's triangle coordinates
	// Determine which corner of first_car bounding box hit second_car bounding box
	// For each triangle, check if impact corner is inside
	// TODO: Depending on which triangle gets hit first, call different collision responses on victim_car

	int counter = 0;
	for (Car::Triangle t : triangles) {
		//printf("Triangle %i (%f, %f) (%f,%f) (%f,%f) Area %f Point hitting (%f, %f) \n", counter, t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y, first_car->get_triangle_area(triangles[3].a, triangles[3].b, triangles[3].c), impact_vertex.x, impact_vertex.y);
		if (victim_car->check_mesh_collision(impact_vertex, t)) {
			printf("triangle %i hit\n", counter);
			Sleep(2500);
			return counter;
		}
		counter++;
	}
	return -1;
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

void LaneManager::lane_queue(Lane* lane, vec2 lane_intersection, float ms) {
	lane->update(ms);
	if (lane->get_time_remaining() <= 0)
	{
		lane->turn_car();
		lane->set_time_remaining(lane->MaxTimePerCar);
		// m_ai->make_villains_decide(m_lanes); TODO (JORDAN): MAKE THIS WORK!!!!!
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
			lane->m_cars.pop_front();
		}
	}
}
