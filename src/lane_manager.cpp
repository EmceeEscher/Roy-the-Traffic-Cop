#include "lane_manager.hpp"
#include "car.hpp"
#include "warning.hpp"




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
  srand(time(NULL));
  spawn_delay = 0;

  return true;
}

void LaneManager::destroy()
{
  m_lanes.clear();
}

void LaneManager::reset()
{
	for (std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++) {
		it->second->clear_lane();
	}
	m_time_remaining = m_time_per_action;
	m_points = 0;
	spawn_delay = 0;
}

bool LaneManager::update(float ms)
{
	bool amb_init = false;
	//For loop for all the lanes if m_is_is_beyond_intersection is true, then add to vector/list/deque to check for collisions between those cars

	if (lane_queue(m_lanes[direction::NORTH], m_lane_coords[direction::NORTH], ms) ||
		lane_queue(m_lanes[direction::WEST], m_lane_coords[direction::EAST], ms) ||
		lane_queue(m_lanes[direction::SOUTH], m_lane_coords[direction::SOUTH], ms) ||
		lane_queue(m_lanes[direction::EAST], m_lane_coords[direction::WEST], ms)) {
		// lane_queue returns true if its time has expired.
		// If this is the case, we should readjust new villains.
		m_ai->make_villains_decide(m_lanes);
	}
	for (int i = 0; i < m_warning.size(); i++) {
		m_warning[i].update(ms);
		amb_init = m_warning[i].amb_init;
	}
	for (int i = 0; i < m_ambulance.size(); i++) {
		m_ambulance[i].update(ms,amb_init);
	}

	spawn_delay -= ms;
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
			Car* car;
			for (int i = 0; i < curr_cars.size(); i++) {
				car = &(curr_cars[i]);
				if (car->is_in_beyond_intersec()) {
					cars_in_intersec.push_back(car);
				}
			}
		}
	}

	for (int i = 0; i < cars_in_intersec.size(); i++) {
		Car* first_car = cars_in_intersec[i];
		rect_bounding_box first_bb = first_car->get_bounding_box();

		for (int j = i + 1; j < cars_in_intersec.size(); j++) {
			Car* second_car = cars_in_intersec[j];
			rect_bounding_box second_bb = second_car->get_bounding_box();

			LaneManager::collisionTuple collision_triangles = {-1, -1};
			// First car is victim
			if (first_car->check_collision(second_bb.bottom_left)
				|| first_car->check_collision(second_bb.bottom_right)
				|| first_car->check_collision(second_bb.top_right)
				|| first_car->check_collision(second_bb.top_left)) {
				collision_occurring = true;
				collision_triangles = mesh_collision_check(second_car, first_car);
				int victim_triangle = collision_triangles.victim_index;
				int attacker_triangle = collision_triangles.attacker_index;
				if (victim_triangle != -1) {
					first_car->collided(victim_triangle);
					second_car->collided(attacker_triangle);
				}
			}
			//Second car is victim
			else if (second_car->check_collision(first_bb.bottom_left)
				|| second_car->check_collision(first_bb.bottom_right)
				|| second_car->check_collision(first_bb.top_right)
				|| second_car->check_collision(first_bb.top_left)) {
				collision_occurring = true;
				collision_triangles = mesh_collision_check(second_car, first_car);
				int victim_triangle = collision_triangles.victim_index;
				int attacker_triangle = collision_triangles.attacker_index;
				if (victim_triangle != -1) {
					first_car->collided(attacker_triangle);
					second_car->collided(victim_triangle);
				}
			}
		}
		//Ambulance check

		for (int k = 0; k < m_ambulance.size(); k++) {
			Ambulance* curr_amb = &m_ambulance[k];
			rect_bounding_box amb_bb = curr_amb->get_bounding_box();
			if (first_car->check_collision(amb_bb.bottom_left)
				|| first_car->check_collision(amb_bb.bottom_right)
				|| first_car->check_collision(amb_bb.top_right)
				|| first_car->check_collision(amb_bb.top_left)) {
					collision_occurring = true;
					int victim_triangle = amb_collision_check(first_car, curr_amb);
					if (victim_triangle != -1) {
						first_car->collided(victim_triangle);
					}
				}
		}
	}
	return collision_occurring;
}

LaneManager::collisionTuple LaneManager::mesh_collision_check(Car* attacker_car, Car* victim_car) {

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
	// Depending on which triangle gets hit first, call different collision responses on victim_car

	int vic_counter = 0;
	int attack_counter;
	LaneManager::collisionTuple collisionTriangles;
	collisionTriangles.victim_index = -1;
	collisionTriangles.attacker_index = -1;
	for (Car::Triangle victim_tri : victim_triangles) {
		attack_counter = 0;
		for (Car::Triangle attacker_tri : attacker_triangles) {
		if (victim_car->check_mesh_collision(attacker_tri.a, victim_tri)
				|| victim_car->check_mesh_collision(attacker_tri.b, victim_tri)
				|| victim_car->check_mesh_collision(attacker_tri.c, victim_tri)) {
					collisionTriangles.victim_index = vic_counter;
					collisionTriangles.attacker_index = attack_counter;
					return collisionTriangles;
				}
				attack_counter++;
		}
		vic_counter++;
	}
	return collisionTriangles;
}

int LaneManager::amb_collision_check(Car* victim_car, Ambulance* amb) {
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

	Car::Triangle amb_triangles[14];

	amb_triangles[0].a = amb->get_vertex_pos(0);
	amb_triangles[0].b = amb->get_vertex_pos(1);
	amb_triangles[0].c = amb->get_vertex_pos(2);

	amb_triangles[1].a = amb->get_vertex_pos(1);
	amb_triangles[1].b = amb->get_vertex_pos(3);
	amb_triangles[1].c = amb->get_vertex_pos(2);

	amb_triangles[2].a = amb->get_vertex_pos(0);
	amb_triangles[2].b = amb->get_vertex_pos(4);
	amb_triangles[2].c = amb->get_vertex_pos(1);

	amb_triangles[3].a = amb->get_vertex_pos(1);
	amb_triangles[3].b = amb->get_vertex_pos(10);
	amb_triangles[3].c = amb->get_vertex_pos(3);

	amb_triangles[4].a = amb->get_vertex_pos(3);
	amb_triangles[4].b = amb->get_vertex_pos(10);
	amb_triangles[4].c = amb->get_vertex_pos(12);

	amb_triangles[5].a = amb->get_vertex_pos(1);
	amb_triangles[5].b = amb->get_vertex_pos(4);
	amb_triangles[5].c = amb->get_vertex_pos(10);

	amb_triangles[6].a = amb->get_vertex_pos(4);
	amb_triangles[6].b = amb->get_vertex_pos(11);
	amb_triangles[6].c = amb->get_vertex_pos(10);

	amb_triangles[7].a = amb->get_vertex_pos(5);
	amb_triangles[7].b = amb->get_vertex_pos(12);
	amb_triangles[7].c = amb->get_vertex_pos(10);

	amb_triangles[8].a = amb->get_vertex_pos(10);
	amb_triangles[8].b = amb->get_vertex_pos(6);
	amb_triangles[8].c = amb->get_vertex_pos(5);

	amb_triangles[9].a = amb->get_vertex_pos(11);
	amb_triangles[9].b = amb->get_vertex_pos(9);
	amb_triangles[9].c = amb->get_vertex_pos(10);

	amb_triangles[10].a = amb->get_vertex_pos(10);
	amb_triangles[10].b = amb->get_vertex_pos(9);
	amb_triangles[10].c = amb->get_vertex_pos(6);

	amb_triangles[11].a = amb->get_vertex_pos(5);
	amb_triangles[11].b = amb->get_vertex_pos(6);
	amb_triangles[11].c = amb->get_vertex_pos(7);

	amb_triangles[12].a = amb->get_vertex_pos(6);
	amb_triangles[12].b = amb->get_vertex_pos(8);
	amb_triangles[12].c = amb->get_vertex_pos(7);

	amb_triangles[13].a = amb->get_vertex_pos(6);
	amb_triangles[13].b = amb->get_vertex_pos(9);
	amb_triangles[13].c = amb->get_vertex_pos(8);

	// Determine victim_car's triangle coordinates
	// Determine which corner of first_car bounding box hit ambulance bounding box
	// For each triangle, check if impact corner is inside
	// Depending on which triangle gets hit first, call different collision responses on victim_car

	int vic_counter = 0;
	int attack_counter;
	for (Car::Triangle victim_tri : victim_triangles) {
		attack_counter = 0;
		for (Car::Triangle amb_tri : amb_triangles) {
			if (victim_car->check_mesh_collision(amb_tri.a, victim_tri)
				|| victim_car->check_mesh_collision(amb_tri.b, victim_tri)
				|| victim_car->check_mesh_collision(amb_tri.c, victim_tri)) {
					return vic_counter;
				}
				attack_counter++;
		}
		vic_counter++;
	}

	//no collision between triangles, just bounding boxes
	return -1;
}

void LaneManager::add_car()
{
  std::map<direction, Lane*>::iterator it = m_lanes.begin();
  std::advance(it, rand()%4);
  {
    if (!it->second->is_lane_full() && spawn_delay < 0)
    {
      it->second->add_car(carType::REGULAR);
	  spawn_delay = rand() % 600 + 200.f;
    }
  }
}

void LaneManager::add_ambulance(direction dir)
{
//TODO for testing purposes
//	if (m_warning.size() == 0) {
		Warning new_warning;
		new_warning.init(dir);
		m_warning.emplace_back(new_warning);
		Ambulance new_ambulance;
		new_ambulance.init(dir);
		m_ambulance.emplace_back(new_ambulance);
//	}
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

std::deque<Warning> LaneManager::get_warning() const {
	return m_warning;
}

std::deque<Ambulance> LaneManager::get_ambulance() const {
	return m_ambulance;
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
			cars[i].release_audio_res();
			lane->m_cars.erase(lane->m_cars.begin() + i);
			++m_points;
		}
	}

	return time_expired;
}

void LaneManager::clear_intersection() {
	for (std::map<direction, Lane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); it++) {
		std::deque<Car> &curr_cars = it->second->m_cars;
		if (curr_cars.size() > 0) {
			Car* car;
			for (int i = curr_cars.size() - 1; i >= 0; i--) {
				car = &(curr_cars[i]);
				if (car->is_hit()) {
					car->release_audio_res();
					curr_cars.erase(curr_cars.begin() + i);
				}
			}
		}
	}
}
