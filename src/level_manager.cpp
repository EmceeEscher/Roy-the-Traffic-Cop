// Header
#include "level_manager.hpp"

bool LevelManager::init()
{
	game_level = 1; //level 11 is endless mode
	is_game_over = false;
	year = 2018;
	required_points = 20;
	VillainSpawnProbability = 0;
	max_time_per_car = 10000;
	ambulance_enabled = false;
	weather_enabled = false;
	four_lanes_enabled = false;
	return true;
}

// Releases all graphics resources
void LevelManager::destroy()
{

}

void LevelManager::update(int points, CurrentTime game_time, float elapsed_ms, LaneManager lane_manager) {
	if (year != game_time.year) {
		if (points < required_points) {
			is_game_over = true;
		}
		else if (game_level <= 10) {
			//progress game
			game_level += 1;
			if (game_level < 7) {
				VillainSpawnProbability += 0.05;
			}else if (game_level >= 7) {
				VillainSpawnProbability = 0.30;
			}
			lane_manager.update_lane_villain_probability(VillainSpawnProbability);
		}
		else {
			//endless mode
			game_level = 11;

		}
		if (max_time_per_car > 6650) {
			max_time_per_car -= 150;
		}
		lane_manager.update_lane_max_time_per_car(max_time_per_car);
		required_points += game_level * 10;
		year = game_time.year;
	}
}

int LevelManager::get_level() {
	return game_level;
}

bool LevelManager::get_game_over() {
	return is_game_over;
}

int LevelManager::get_next_level_point_req() {
	return required_points;
}

void LevelManager::set_endless_mode(LaneManager lane_manager) {
	game_level = 11;
	VillainSpawnProbability = 0.30;
	lane_manager.update_lane_villain_probability(VillainSpawnProbability);
	max_time_per_car = 6500;
	lane_manager.update_lane_max_time_per_car(max_time_per_car);
}

