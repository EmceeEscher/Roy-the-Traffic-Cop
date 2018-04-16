#pragma once
#include "game_timer.hpp"
#include "common.hpp"
#include "lane_manager.hpp"
#include "high_scores.hpp"


using std::string;

class LevelManager
{
public:
	// Creates all the associated render resources and default transform
	bool init(HighScores high_scores);

	// Releases all associated resources
	void destroy();

	// Return current game level
	int get_level();

	// Return if game is over
	bool get_game_over();

	void update(int points, CurrentTime game_time, float elapsed_ms, LaneManager lane_manager);

	// Return total points required for next level
	int get_next_level_point_req();
	
	// Set game to endless mode
	void set_endless_mode(LaneManager lane_manager);


private:
	HighScores m_high_scores;
	int game_level;
	bool is_game_over;
	int year;
	int required_points; //minimum amount of cars required to proceed to next level
	float VillainSpawnProbability; 
	float max_time_per_car;
	bool ambulance_enabled;
	bool weather_enabled;
	bool four_lanes_enabled;
};