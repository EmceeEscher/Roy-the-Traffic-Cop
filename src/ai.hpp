#pragma once

#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <cmath> 
#include <cstdlib>
#include <ctime>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

struct PlayerOutcome
{
	float cars_turned_per_move;
	direction most_villainous_direction;
};

class AI
{
public:
	const int MaxPly = 3;

	// Estimated time it takes for the player to execute a move.
	// Used to determine if the player must move a specific lane due to a time constraint
	const float EstimatedPlayerMoveTime = 400;

	bool init();

	// Will set any villains in the second position in any lane to a concrete turning choice
	void make_villains_decide(std::map<direction, Lane*> lanes);

private:
	// Will make a specific villain in the second position of a lane choose a concrete turning choice
	direction make_villain_decide(direction villain_lane, std::map<direction, Lane*> lanes);

	// For a given set of lanes, generate every possible move that the player could take. 
	// A move consists of turning one or more cars without causing a collision.
	std::vector<std::set<direction>> generate_moves(std::map<direction, Lane*> current_lanes);

	// For a given set of lanes, calculate which lane requires most urgent attention
	direction calculate_most_urgent_direction(std::map<direction, Lane*> current_lanes);

	// For a given set of lanes, calculate a compatibility matrix to determine which lanes can move at the same time
	std::vector<std::vector<bool>> calculate_compatibility_matrix(std::map<direction, Lane*> current_lanes, std::vector<direction> directions);

	// Make deepcopy of the lanes so we can simulate moves without affecting the true gamestate
	std::map<direction, Lane*> deep_copy_lanes(std::map<direction, Lane*>);
	
	// For a specific villain, do the work of finding out which way he should turn
	void get_best_turning_direction(direction active_lane, std::map<direction, Lane*> current_lanes,
		int current_ply, int cars_turned, bool villain_at_front, PlayerOutcome *bestPlayerOutcome);

	// Mutate the (previously deepcopied) lanes to reflect the new reality after a move is simulated
	void apply_move_to_lanes(std::set<direction> move, std::map<direction, Lane*> lanes);

	// Given a situation where our villain has reached the front of the lane, 
	// return the direction would be the worst for the player
	direction get_most_villainous_direction(direction active_lane, std::map<direction, Lane*> current_lanes);

	// Given two lanes and the cars in them, determine whether or not both lanes can be safely sent at once
	bool can_lanes_go_at_once(int lane_1_position, int lane_2_position, int car_1_destination, int car_2_destination);

	// Given a car's position and destination, returns 90 for a right turn, 180 for going straight and -90 for a left turn
	int convert_angle_to_turn_angle(int car_position, int car_destination);

	// Convert direction to the position of the direction on the compass (0 for south, 90 for east, 180 for north, 270 for west)
	int get_lane_position_from_direction(direction direction);
};
