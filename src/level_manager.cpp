// Header
#include "level_manager.hpp"

bool LevelManager::init()
{
	game_level = 1;
	is_game_over = false;
	year = 2018;
	required_points = 20;
	return true;
}

// Releases all graphics resources
void LevelManager::destroy()
{

}

void LevelManager::update(int points, CurrentTime game_time, float elapsed_ms) {
	if (year != game_time.year) {
		if (points < required_points) {
			is_game_over = true;
		}
		else if (game_level <= 10) {
			game_level += 1;
		}
		else {
			game_level = 11;
		}
		required_points += game_level * 20;
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

