#include "ai.hpp"

bool AI::init()
{
	std::srand(std::time(nullptr));
	return true;
}

void AI::make_villains_decide(std::map<direction, Lane*> lanes)
{
	std::map<direction, Lane*> new_lanes = deep_copy_lanes(lanes);
	std::map<direction, int> cars_in_intersection;

	// Prune cars in the intersection
	for (std::map<direction, Lane*>::iterator it = new_lanes.begin(); it != new_lanes.end(); it++)
	{
		cars_in_intersection[it->first] = 0;

		std::deque<Car> copiedCarsInLane = it->second->get_cars();
		for (auto car = copiedCarsInLane.begin(); car != copiedCarsInLane.end(); car++) {
			if (car->is_in_beyond_intersec()) {
				cars_in_intersection[it->first] += 1;
				it->second->erase_first_car();
			}
		}
	}

	for (std::map<direction, Lane*>::iterator it = lanes.begin(); it != lanes.end(); it++)
	{
		std::deque<Car> originalCarsInLane = it->second->get_cars();
		std::deque<Car> copiedCarsInLane = new_lanes[it->first]->get_cars();

		// Villains should never remain villains after making it to second in line in the intersection.
		// vehicles generated into lanes without cars already in them are not eligible to be villains
		if (copiedCarsInLane.size() > 1) {
			if (copiedCarsInLane[1].is_villain()) {
				direction new_direction = make_villain_decide(it->first, new_lanes);
				it->second->set_car_direction(new_direction, 1 + cars_in_intersection[it->first]);
			}
		}
	}
}

direction AI::make_villain_decide(direction active_lane, std::map<direction, Lane*> lanes) {
	PlayerOutcome bestPlayerOutcome = { 
		0,
		direction::NORTH, // This will never be the outcome because the player will always move at least one car
	};

	get_best_turning_direction(active_lane, lanes, 0, 0, false, &bestPlayerOutcome);

	return bestPlayerOutcome.most_villainous_direction;
}

void AI::get_best_turning_direction(
	direction active_lane, 
	std::map<direction, Lane*> current_lanes,
	int current_ply, 
	int cars_turned, 
	bool villain_at_front, 
	PlayerOutcome *bestPlayerOutcome) 
{
	if (villain_at_front) {
		// The villain is now at the front of its lane. Time to decide the most dastardly villainous move for this scenario!

		// If current_cars_turned_per_move is not better than the best outcome found so far, then the player won't do this
		// so there's no reason for us to do any calculations here
		float current_cars_turned_per_move = (float)cars_turned / (float)current_ply;
		if (current_cars_turned_per_move > bestPlayerOutcome->cars_turned_per_move) {
			bestPlayerOutcome->cars_turned_per_move = current_cars_turned_per_move;
			bestPlayerOutcome->most_villainous_direction = get_most_villainous_direction(active_lane, current_lanes);
		}

		return;
	}

	if (current_ply == MaxPly) {
		// We've reached our ply depth without moving our villain to the front of the line. If this is the best move
		// for the player then it doesn't matter what we set our villain to... If we set our MaxPly to infinity then
		// this will never occur and eventually the villain will reach the front of the line, but that would be
		// computationally exhausting and, in fact, wouldn't terminate.

		// If current_cars_turned_per_move is not better than the best outcome found so far, then the player won't do this
		// so there's no reason for us to do any calculations here
		float current_cars_turned_per_move = (float)cars_turned / (float)current_ply;
		if (current_cars_turned_per_move > bestPlayerOutcome->cars_turned_per_move) {
			bestPlayerOutcome->cars_turned_per_move = current_cars_turned_per_move;
			direction new_direction = active_lane;
			while (active_lane == new_direction) {
				new_direction = direction(rand() % 4);
			}

			bestPlayerOutcome->most_villainous_direction = new_direction;
		}

		return;
	}

	// If the villain isn't at the front of his lane and we aren't at our max depth, then let's build out our tree for each
	// possible (valid) move that the player could make.

	// Don't want to modify the real game lanes
	std::map<direction, Lane*> new_lanes = deep_copy_lanes(current_lanes);
	std::vector<std::set<direction>> possible_moves = generate_moves(new_lanes);
	int next_ply = current_ply + 1;
	
	for (std::vector<std::set<direction>>::iterator it = possible_moves.begin(); it != possible_moves.end(); it++)
	{
		apply_move_to_lanes(*it, new_lanes);
		bool is_moving_active_lane = it->find(active_lane) != it->end();
		int next_total_cars_turned = cars_turned + it->size();

		get_best_turning_direction(active_lane, new_lanes, next_ply, next_total_cars_turned, is_moving_active_lane, bestPlayerOutcome);
		for (auto current = new_lanes.begin(); current != new_lanes.end(); ++current) {
			delete current->second;
		}
		new_lanes.clear();
		new_lanes = deep_copy_lanes(current_lanes);
	}
	for (auto current = new_lanes.begin(); current != new_lanes.end(); ++current) {
		delete current->second;
	}
	new_lanes.clear();
}

std::map<direction, Lane*> AI::deep_copy_lanes(std::map<direction, Lane*> lanes) {
	std::map<direction, Lane*> new_lanes;
	for (std::map<direction, Lane*>::iterator it = lanes.begin(); it != lanes.end(); it++) 
	{
		new_lanes[it->first] = new Lane(*(it->second));
	}

	return new_lanes;
}

std::vector<std::set<direction>> AI::generate_moves(std::map<direction, Lane*> current_lanes) {
	// This is the lane that is next going to auto-turn the car
	direction most_urgent_lane = calculate_most_urgent_direction(current_lanes);
	
	// We need to generate a compatibility matrix. This matrix is 4x4 and each cell represents a 
	// relationship between two lanes. If the value of the cell is "true" then that means that the cars
	// in both of those lanes can be moved at the same time without causing a collision. If the value in
	// the cell is false, that means that if both of those lanes are moved simultaneously, a collision will
	// result.
	std::vector<direction> directions{ direction::SOUTH, direction::EAST, direction::NORTH, direction::WEST };
	std::vector<std::vector<bool>> compatibility_matrix = calculate_compatibility_matrix(current_lanes, directions);
	

	std::vector<std::vector<int>> combinations{ {0,1,2,3}, {0,1,2}, {0,1,3}, {0,2,3}, {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}, {0}, {1}, {2}, {3} }; // hacky
	std::vector<std::set<direction>> moves;

	// Check to see if each possible move is valid. For instance, if {0,1,2,3} is valid, then all four lanes can move simultaneously.
	// If {1,2} is valid then the East and North lanes can move simultaneously. Each valid combination is a valid move that the user could
	// take. At least one combination must be valid unless all lanes are empty, in which case this function will never be called.
	for (int i = 0; i < combinations.size(); i++) {
		std::vector<int> combination = combinations[i];
		int combination_size = combination.size(); // The number of lanes that would simultaneously turn if the player took this move
		if (moves.size() > 0) {
			if (moves.back().size() > combination_size) {
				// If we've found a combination with more lanes moving than the current combination, then let's move on.
				// A user should never turn only n lanes if he can instead move more than n, so there's no need to proceed.
				break;
			}
		}
		bool valid_combination = true;
		bool contains_most_urgent_lane = false;
		std::set<direction> move;
		for (int j = 0; j < combination_size; j++) {
			int current_proposed_lane = combination[j];

			if (directions[current_proposed_lane] == most_urgent_lane) {
				// This combination will involve the player moving the lane that's closest to going on its own.
				contains_most_urgent_lane = true;
			}
			for (int k = j + 1; k < combination_size; k++) {
				// If any two lanes in this combination aren't able to move at the same time, then this combination
				// is invalid.
				valid_combination = valid_combination && compatibility_matrix[current_proposed_lane][combination[k]];
			}

			if (current_lanes[direction(current_proposed_lane)]->get_cars().size() == 0) {
				// If this lane is empty, it can't be moved
				valid_combination = false;
			}
			
			if (!valid_combination) {
				// If any two of the lanes can't move in unison then this move is invalid so no point in checking more.
				break;
			}

			// Convert integer to direction
			move.insert(directions[combination[j]]);
		}

		// If there's very low time left on a lane's clock, then a move isn't valid unless it involves moving the car in that lane.
		float most_urgent_lane_time_remaining = current_lanes[most_urgent_lane]->get_time_remaining();
		if (most_urgent_lane_time_remaining < EstimatedPlayerMoveTime) {
			valid_combination = valid_combination && contains_most_urgent_lane;
		}
		
		// Only return this combination as a move if it's a valid move.
		if (valid_combination) {
			moves.push_back(move);
		}
	}

	return moves;
}

direction AI::calculate_most_urgent_direction(std::map<direction, Lane*> current_lanes) {
	direction most_urgent_lane;
	float most_urgent_lane_time_remaining = std::numeric_limits<float>::max();

	for (std::map<direction, Lane*>::iterator it = current_lanes.begin(); it != current_lanes.end(); it++)
	{
		float current_lane_time_remaining = it->second->get_time_remaining();
		if (current_lane_time_remaining < most_urgent_lane_time_remaining) {
			most_urgent_lane_time_remaining = current_lane_time_remaining;
			most_urgent_lane = it->first;
		}
	}

	return most_urgent_lane;
}

std::vector<std::vector<bool>> AI::calculate_compatibility_matrix(std::map<direction, Lane*> current_lanes, std::vector<direction> directions) {
	std::vector<std::vector<bool>> compatibility_matrix;
	compatibility_matrix.resize(4, std::vector<bool>(4, false)); // Matrix is now 4x4

	for (int i = 0; i < directions.size() - 1; i++) {
		for (int j = i + 1; j < directions.size(); j++) {
			direction direction1 = directions[i];
			direction direction2 = directions[j];
			std::deque<Car> lane1_cars = current_lanes[direction1]->get_cars();
			std::deque<Car> lane2_cars = current_lanes[direction2]->get_cars();
			if (lane1_cars.size() == 0 || lane2_cars.size() == 0) {
				// Can't move both lanes at once if one lane can't move at all because it has no car
				compatibility_matrix[i][j] = false;
				break;
			}
			Car car1 = lane1_cars[0];
			Car car2 = lane2_cars[0];
			if (car1.is_villain() || car2.is_villain()) {
				// If either car is a villain then assume that they won't allow the player to move them at once.
				// This avoids complicated double recursive non-terminating logic where each villain needs to know
				// about the other villain before it can come to a decision.
				compatibility_matrix[i][j] = false;
			}
			else {
				compatibility_matrix[i][j] = can_lanes_go_at_once(
					get_lane_position_from_direction(direction1),
					get_lane_position_from_direction(direction2),
					get_lane_position_from_direction(car1.get_desired_direction()),
					get_lane_position_from_direction(car2.get_desired_direction())
				);
			}
		}
	}

	return compatibility_matrix;
}

void AI::apply_move_to_lanes(std::set<direction> move, std::map<direction, Lane*> lanes) {
	for (std::map<direction, Lane*>::iterator it = lanes.begin(); it != lanes.end(); it++) 
	{
		Lane *currentLane = it->second;
		if (move.find(it->first) != move.end()) {
			// If the move involves this lane then turn the car in this lane and reset the time remaining.
			currentLane->set_time_remaining(currentLane->MaxTimePerCar);
			currentLane->erase_first_car();
		}
		else {
			// If the move doesn't involve this lane, then subtract our estimated time remaining to simulate
			// time passing. Don't move the car in the lane.
			// It's theoretically possible for the time remaining to drop below zero for a lane but that
			// actually isn't a problem as far as the AI is concerned.
			currentLane->set_time_remaining(currentLane->get_time_remaining() - EstimatedPlayerMoveTime);
		}
	}
}

direction AI::get_most_villainous_direction(direction active_lane, std::map<direction, Lane*> current_lanes) {
	std::vector<direction> directions{ direction::SOUTH, direction::EAST, direction::NORTH, direction::WEST };
	direction best_direction = direction::SOUTH;
	int best_direction_score = 5;

	// Test all four directions and see which one involves the fewest cars moving on the player's next turn.
	Car villain = current_lanes[active_lane]->get_cars()[0];
	for (int i = 0; i < directions.size(); i++) {
		direction current_direction = directions[i];
		if (current_direction == active_lane) {
			continue; // No U-Turns
		}
		villain.set_desired_direction(current_direction);

		// If the villain chooses this direction, what moves will be available to the player
		std::vector<std::set<direction>> moves = generate_moves(current_lanes);
		
		// generate_moves will only generate moves that move the most number of cars possible
		// the number of simultaneous lanes moving will be constant across all moves in moves
		int best_move_size = moves.begin()->size();

		if (best_move_size < best_direction_score) {
			best_direction = current_direction;
			best_direction_score = best_move_size;
		}
	}

	return best_direction;
}

// INVARIANT: lane_1_position must be less than lane_2_position
// therefore car1 is always from a lower position lane than car2
// lane positions:
// 0 - South
// 90 - East
// 180 - North
// 270 - West
bool AI::can_lanes_go_at_once(
	int lane_1_position, 
	int lane_2_position, 
	int car_1_destination, 
	int car_2_destination
) {
	int lane_angle = lane_2_position - lane_1_position; // Angle (> 0) between the two lanes

	int car_1_turn_desire = convert_angle_to_turn_angle(lane_1_position, car_1_destination);
	int car_2_turn_desire = convert_angle_to_turn_angle(lane_2_position, car_2_destination);

	switch (lane_angle) {
	case 90:
		// The lanes are 90 degrees apart
		// Lane 1 is to the LEFT of lane 2
		switch (car_1_turn_desire) {
		case 90:
			// The first car wants to turn right
			// It's always safe for both cars to go
			return true;
		case -90:
		case 180:
			// The first car wants to go straight or turn left
			// It's never safe for both cars to go
			return false;
		default:
			throw std::invalid_argument("received invalid car destinations");
		}
	case 180:
		// The lanes are across from eachother
		switch (car_1_turn_desire) {
		case -90:
			// The first car wants to go left
			// It's never safe for both cars to go
			return false;
		case 90:
		case 180:
			// The first car wants to go straight or right
			// It's safe to go as long as the second car doesn't want to turn left
			return car_2_turn_desire != -90;
		default:
			throw std::invalid_argument("received invalid car destinations");
		}
	case 270:
		// The lanes are 90 degrees apart
		// Lane 1 is to the RIGHT of lane 2
		switch (car_2_turn_desire) {
		case 90:
			// The second car wants to turn right
			// It's always safe for both cars to go
			return true;
		case -90:
		case 180:
			// The second car wants to go straight or turn left
			// It's never safe for both cars to go
			return false;
		default:
			throw std::invalid_argument("received invalid car destinations");
		}
	default:
		throw std::invalid_argument("received invalid lane positions");
	}
}

int AI::convert_angle_to_turn_angle(int car_position, int car_destination) {
	int angle = car_destination - car_position;
	switch (angle) {
	case -90:
	case 270:
		return -90;
	case 180:
	case -180:
		return 180;
	case 90:
	case -270:
		return 90;
	default:
		throw std::invalid_argument("received invalid lane positions");
	}
}

int AI::get_lane_position_from_direction(direction direction) {
	switch (direction) {
	case direction::SOUTH:
		return 0;
	case direction::EAST:
		return 90;
	case direction::NORTH:
		return 180;
	case direction::WEST:
		return 270;
	default:
		throw std::invalid_argument("received invalid direction");
	}
}